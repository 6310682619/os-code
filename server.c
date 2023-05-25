#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>

#define MAX_QUESTIONS 25
#define QUESTION_TIMEOUT 5

typedef struct {
    int client_socket;
    int client_id;
} ClientArgs;

void *handle_client(void *args) {
    ClientArgs *client_args = (ClientArgs *) args;
    int client_socket = client_args->client_socket;
    int client_id = client_args->client_id;

    // Send game name and rules to client
    char game_info[] = "Quick Math Game\n\nRules:\n1. Answer each question within 5 seconds.\n2. Answer correctly to continue.\n3. Wrong answer or timeout means game over.\n";
    send(client_socket, game_info, strlen(game_info), 0);

    srand(time(NULL));

    int total_questions = 0;
    int correct_answers = 0;

    // Start sending questions
    while (total_questions < MAX_QUESTIONS) {
        // Generate random numbers for the question
        int num1 = rand() % 15 + 1;
        int num2 = rand() % 15 + 1;
        int sum = num1 + num2;

        // Compose the question
        char question[50];
        sprintf(question, "Q%d: What is %d + %d?\n", total_questions + 1, num1, num2);

        // Send the question to the client
        send(client_socket, question, strlen(question), 0);

        // Set up the file descriptor set for select
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(client_socket, &fds);

        // Set up the timeout value for select
        struct timeval timeout;
        timeout.tv_sec = QUESTION_TIMEOUT;
        timeout.tv_usec = 0;

        // Wait for client response with a timeout
        int select_result = select(client_socket + 1, &fds, NULL, NULL, &timeout);

        if (select_result == -1) {
            perror("Error in select");
            break;
        } else if (select_result == 0) {
            // Timeout reached, game over
            break;
        } else {
            // Receive the client's answer
            char client_answer[10];
            ssize_t bytes_received = recv(client_socket, client_answer, sizeof(client_answer), 0);

            if (bytes_received <= 0) {
                // Error or connection closed by the client
                break;
            }

            // Null-terminate the received data
            client_answer[bytes_received] = '\0';

            // Check the client's answer
            int answer = atoi(client_answer);
            if (answer == sum) {
                // Correct answer
                correct_answers++;
                total_questions++;
            } else {
                // Wrong answer
                break;
            }
        }
    }

    // Game over
    char game_result_message[50];
    if (correct_answers == MAX_QUESTIONS) {
        sprintf(game_result_message, "Congratulations! You answered all questions correctly!\n");
    } else {
        sprintf(game_result_message, "Game Over! Total Questions: %d, Correct Answers: %d\n", total_questions, correct_answers);
    }
    send(client_socket, game_result_message, strlen(game_result_message), 0);

    // Ask the client if they want to play again
    char play_again_message[] = "Do you want to play again? (yes/no): ";
    send(client_socket, play_again_message, strlen(play_again_message), 0);

    // Receive the client's response
    char client_response[10];
    ssize_t bytes_received = recv(client_socket, client_response, sizeof(client_response), 0);

    if (bytes_received > 0) {
        // Null-terminate the received data
        client_response[bytes_received] = '\0';

        // Check the client's response
        if (strcmp(client_response, "yes") == 0) {
            // Play again
            handle_client(args);  // Start a new game with the same client
        }
        else if (strcmp(client_response, "no") == 0) {
        // Close the connection
        close(client_socket);
        }
    }

    // Close the client socket
    close(client_socket);

    free(args);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Socket bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Socket listen failed");
        return 1;
    }

    printf("Server started. Waiting for connections...\n");

    int client_id = 1;

    while (1) {
        // Accept client connection
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_length);

        if (client_socket == -1) {
            perror("Failed to accept client connection");
            continue;
        }

        printf("Client %d connected. Creating a new thread.\n", client_id);

        // Create thread to handle the client
        pthread_t thread_id;
        ClientArgs *client_args = (ClientArgs *) malloc(sizeof(ClientArgs));
        client_args->client_socket = client_socket;
        client_args->client_id = client_id;

        if (pthread_create(&thread_id, NULL, handle_client, (void *) client_args) < 0) {
            perror("Failed to create thread");
            continue;
        }

        // Detach the thread to clean up after it
        pthread_detach(thread_id);

        client_id++;
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
