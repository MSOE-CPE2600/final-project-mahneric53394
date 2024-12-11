/**********************************************************
* Lab 13 (Final Project)
* Filename: client.c
* Section: 121
* Description: Client can join once the server is up, and
* can have a conversation with the server terminal until
* the chat is exited.
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
int client_fd;
struct sockaddr_un server_addr;
char buffer[BUFFER_SIZE];

//Signal handler function
void signal_handler() {
    //Cleanup
    close(client_fd);
    exit(1);
}

int main() {
    //Handle a signal interrupt
    signal(SIGINT, signal_handler);
    //Ignore pipe signal
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

    //Print initial steps
    printf("Connected to the server. Start chatting!\n");
    printf("Type '-exit' in order to exit chat!\n");

    //Fork so that it can read and write at the same time
    pid_t pid = fork();

    //Infinite loop for the chat
    while (1) {
        //Child process (send message)
        if(pid == 0) {
            //Wait for user input
            printf("You: ");
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            //Send message to the server
            if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
                perror("Send failed");
                break;
            }

            //Check to see if sent message is exit command
            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("Exiting client\n");
                kill(getppid(), SIGINT);
                break;
            }
        }
        //Parent process (recieve message)
        else if (pid > 0) {
            //Set memory
            memset(buffer, 0, BUFFER_SIZE);
            //Wait for a recieve message from server
            if (recv(client_fd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("Server disconnected.\n");
                kill(pid, SIGINT);
                break;
            }
            //Reprint the prompt and the received message
            printf("\r\033[K");  // Clear the current line
            printf("Server: %s", buffer);
            printf("You: ");
            fflush(stdout);

            //Check if message sent from server is exit command
            if (strncmp(buffer, "-exit", 5) == 0) {
                printf("\nServer exited. Closing client.\n");
                kill(getppid(), SIGINT);
                break;
            }
        }
    }

    //Clean up by closing client and killing any current pid
    close(client_fd);
    if(pid > 0) {
        kill(pid, SIGKILL);
    }

    //Exit program
    return 0;
}
