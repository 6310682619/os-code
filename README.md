# Quick Math Game!

This repository contains a simple server-client program for playing a math game. The server program allows clients to connect and play a math game, while the client program connects to the server and interacts with it.

## Features

- The server program:
  - Listens for incoming connections from clients
  - Sends game information and rules to clients
  - Generates random addition questions for the math game
  - Tracks the number of correct answers from clients
  - Notifies clients of the game result
  - Allows clients to play the game again if desired

- The client program:
  - Connects to the server program
  - Receives game information and rules from the server
  - Provides an interface for users to input their answers
  - Sends user answers to the server for evaluation
  - Receives and displays game results from the server
  - Allows users to play again or close the connection with the server

## Usage

1. Compile the server and client programs separately using a C compiler (e.g., `gcc`):
   ```
   gcc server.c -o server -pthread
   gcc client.c -o client
   ```

2. Start the server by running the compiled server program:
   ```
   ./server <port>
   ```
   Replace `<port>` with the desired port number for the server to listen on.

3. Start the client by running the compiled client program:
   ```
   ./client <server_ip> <port>
   ```
   Replace `<server_ip>` with the IP address of the server and `<port>` with the corresponding port number.

4. Follow the instructions on the client program's interface to play the math game.

5. To exit the client program, enter "no" when prompted to play again.