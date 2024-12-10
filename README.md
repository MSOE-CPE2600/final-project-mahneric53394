## Lab 13 Systems Programming Section 121
# Description
  Chat program between two terminals using UNIX sockets. Either side can send and receive messages at the same time using forks in the program. The fork system call is used in order to allow both the server and client looking for sent messages and recieved messages at the same time, and if one is sent or recieved the interface is updated accordingly. For this implementation, the child process was the recieve message and parent process was the send message for the server, and inversely the child process was the send message and parent process was the recieve message for the client. There is also implementation to make sure there are no memory leaks in the program. There is also signal handling if the user exits the program using CTRL-C or other ways instead of using the '-exit' command in the program. If there is an interrupt signal, the program is gracefully cleaned up and exited. 

# Console Image (Example)
![Lab%13%-%Systems%Programming.png](https://github.com/MSOE-CPE2600/final-project-mahneric53394/blob/main/Lab%2013%20-%20Systems%20Programming.png)

# Names
Eric Mahn and Braydon Hanson
