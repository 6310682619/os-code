#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CLIENTS 5

// Function to handle client connections
void *handle_client(void *arg);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    pthread_t tid;
    
    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    
    // Set server address parameters
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    while (1) {
        socklen_t addr_len = sizeof(client_addr);
        
        // Accept a new client connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
            perror("Failed to accept");
            continue;
        }
        
        printf("New client connected: %s\n", inet_ntoa(client_addr.sin_addr));
        
        // Create a new thread to handle the client
        if (pthread_create(&tid, NULL, handle_client, (void *)&client_fd) != 0) {
            perror("Failed to create thread");
            continue;
        }
    }
    
    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char buffer[1024];
    int n;
    
    // Send game name and rules to the client
    const char *game_name = "Quick Math Game";
    const char *game_rules = "Welcome to Quick Math Game! Rules: You have 10 seconds to answer each question. Answer correctly to continue. Good luck!\n";
    
    write(client_fd, game_name, strlen(game_name));
    usleep(500000);
    write(client_fd, game_rules, strlen(game_rules));
    
    while (1) {
        // Generate random numbers
        int a = rand() % 40 + 1;
        int b = rand() % 40 + 1;
        int result;
        
        // Generate random operator
        int operator = rand() % 2;
        
        // Send the math question to the client
        if (operator == 0)
            sprintf(buffer, "Question: %d + %d = ?", a, b);
        else
            sprintf(buffer, "Question: %d - %d = ?", a, b);
        
        write(client_fd, buffer, strlen(buffer));
        
        // Set a timer for 10 seconds
        alarm(10);
        
        // Read the client's answer
        n = read(client_fd, buffer, sizeof(buffer));
        buffer[n] = '\0';
        
        // Check if the answer is correct
        if (operator == 0)
            result = a + b;
        else
            result = a - b;
        
        // Check if the answer is correct or time is up
        if (atoi(buffer) == result) {
            // Send a success message to the client
            write(client_fd, "Correct!\n", 9);
        } else {
            // Send a failure message to the client
            write(client_fd, "Wrong Answer!\n", 14);
            break;  // End the game
        }
    }
    
    // Game over, send the total number of questions answered
    int total_questions = 0;
    write(client_fd, "Game over!\n", 11);
    usleep(500000);
    sprintf(buffer, "Total questions answered: %d\n", total_questions);
    write(client_fd, buffer, strlen(buffer));
    
    // Ask the client if they want to play again
    write(client_fd, "Do you want to play again? (yes/no)\n", 36);
    n = read(client_fd, buffer, sizeof(buffer));
    buffer[n] = '\0';
    
    if (strcmp(buffer, "yes\n") == 0) {
        // Client wants to play again, continue the loop
        return handle_client(arg);
    } else {
        // Client doesn't want to play again, close the connection
        close(client_fd);
    }
    
    return NULL;
}
