#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *receive_messages(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
    char server_message[BUFFER_SIZE];
    
    while (1) {
        ssize_t bytes_received = recv(client_socket, server_message, sizeof(server_message), 0);
        
        if (bytes_received <= 0) {
            // Error or connection closed by the server
            break;
        }
        
        // Null-terminate the received data
        server_message[bytes_received] = '\0';
        
        printf("%s", server_message);
        
        // Check if the server requested to close the connection
        if (strstr(server_message, "Closing connection") != NULL) {
            break;
        }
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }
    
    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    
    // Create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }
    
    // Set up server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &(server_address.sin_addr)) <= 0) {
        perror("Invalid server IP address");
        return 1;
    }
    
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        return 1;
    }
    
    printf("Connected to the server.\n");
    
    // Create thread to receive server messages
    pthread_t thread_id;
    
    if (pthread_create(&thread_id, NULL, receive_messages, (void *)&client_socket) < 0) {
        perror("Failed to create thread");
        return 1;
    }
    
    // Read user input and send to server
    char client_message[BUFFER_SIZE];
    
    while (1) {
        printf("> ");
        fgets(client_message, sizeof(client_message), stdin);
        
        // Remove trailing newline character
        client_message[strcspn(client_message, "\n")] = '\0';
        
        // Send message to the server
        send(client_socket, client_message, strlen(client_message), 0);
        
        // Check if the client requested to close the connection
        if (strcmp(client_message, "no") == 0) {
            break;
        }
    }
    
    // Close the client socket
    close(client_socket);
    
    return 0;
}
