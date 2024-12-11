/**********************************************************
* Lab 13 (Final Project)
* Filename: server.c
* Section: 121
* Description: Creates the server and waits for the client
* to join. When the client joins they can have a
* conversation with each other until exited.
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

//Initialize global variables
#define SOCKET_PATH "chat_socket"
#define BUFFER_SIZE 100
int server_fd, client_fd;
struct sockaddr_un server_addr, client_addr;
socklen_t client_len;
char buffer[BUFFER_SIZE];

//signal handler function
void signal_handler() {
    //Clean up
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(1);
}

//client exiter function
void client_exit_handler(int signum) {
    //Clean up
    printf("\nClient exited. Closing server.\n");
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

int main() {
    //Handle a signal interrupt
    signal(SIGINT, signal_handler);
    //Ignore the pipe signal
    signal(SIGPIPE, SIG_IGN); 
    //Custom signal to exit handler
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

    //Print initial steps
    printf("Client connected. Start chatting!\n");
    printf("Type '-exit' in order to exit chat!\n");

    //Fork so that it can read and write at the same time
    pid_t pid = fork();

    //Infinite loop for the chat
    while (1) {
        //Child process (recieve message)
        if (pid == 0) {
            //Set memory
            memset(buffer, 0, BUFFER_SIZE);
            //Wait for a recieve message from client
            if (recv(client_fd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("Client disconnected.\n");
                kill(getppid(), SIGUSR1); 
                break;
            }
            //Reprint the prompt and the received message
            printf("\r\033[K");  // Clear the current line
            printf("Client: %s", buffer);
            printf("You: ");
            fflush(stdout);

            //Check if the client sent an exit command
            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("\nClient exited. Closing server.\n");
                kill(getppid(), SIGUSR1);  
                break;
            }
        } 
        //Parent process (send message)
        else if (pid > 0) {
            //Wait for user input
            printf("You: ");
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            //Send message to the client
            if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
                perror("Send failed");
                break;
            }

            //See if server sent exit command
            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("Exiting Server\n");
                kill(pid, SIGINT);
                break;
            }
        }
    }

    //Clean up by closing fd's and unlink socket
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    //Exit program
    return 0;
}
