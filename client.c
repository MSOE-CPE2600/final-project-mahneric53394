/**********************************************************
* Lab 13 (Final Project)
* Filename: client.c
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
int client_fd;
struct sockaddr_un server_addr;
char buffer[BUFFER_SIZE];

void signal_handler() {
    close(client_fd);
    exit(1);
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    //Create the socket
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    //Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. Start chatting!\n");
    printf("Type '-exit' in order to exit chat!\n");
    pid_t pid = fork();

    //Infinite loop for the chat
    while (1) {

        if(pid == 0) {
            
            printf("You: ");
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
                perror("Send failed");
                break;
            }

            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("Exiting client\n");
                kill(getppid(), SIGINT);
                break;
            }

        }
        else if (pid > 0) {
            
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(client_fd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("Server disconnected.\n");
                kill(pid, SIGINT);
                break;
            }
            printf("\nServer: %s", buffer);
            printf("You: ");
            fflush(stdout);

            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("\nServer exited. Closing client.\n");
                kill(getppid(), SIGINT);
                break;
            }

        }

    }


    close(client_fd);
    if(pid > 0) {
        kill(pid, SIGKILL);
    }
    return 0;
}
