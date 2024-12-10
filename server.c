/**********************************************************
* Lab 13 (Final Project)
* Filename: server.c
* Section: 121
* Description: 
* Author: Eric Mahn and Braydon Hanson
* Date: 12/9/24
* Compile Instructions: make 
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "chat_socket"
#define BUFFER_SIZE 100
int server_fd, client_fd;
struct sockaddr_un server_addr, client_addr;
socklen_t client_len;
char buffer[BUFFER_SIZE];

void signal_handler() {
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(1);
}

void client_exit_handler(int signum) {
    printf("\nClient exited. Closing server.\n");
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

int main() {
    //Signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, SIG_IGN); 
    signal(SIGUSR1, client_exit_handler); 

    //Create a socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Set up the server structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    //Remove a socket if one already exists
    unlink(SOCKET_PATH);

    //Bind socket to the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //Listen for an incomming client
    if (listen(server_fd, 1) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for a connection...\n");

    //Accepts the connection from the client
    client_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected. Start chatting!\n");
    printf("Type '-exit' in order to exit chat!\n");
    pid_t pid = fork();

    //Infinite loop for the chat
    while (1) {
        if (pid == 0) {

            memset(buffer, 0, BUFFER_SIZE);
            if (recv(client_fd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("Client disconnected.\n");
                kill(getppid(), SIGUSR1); 
                break;
            }
            printf("\nClient: %s", buffer);
            printf("You: ");
            fflush(stdout);

            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("\nClient exited. Closing server.\n");
                kill(getppid(), SIGUSR1);  
                break;
            }
        } else if (pid > 0) {

            printf("You: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
                perror("Send failed");
                break;
            }

            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("Exiting Server\n");
                kill(pid, SIGINT);
                break;
            }
        }
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
