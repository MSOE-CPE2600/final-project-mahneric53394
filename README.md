## Lab 13 Systems Programming Section 121
# Description
  Chat program between two terminals using UNIX sockets. Either side can send and receive messages at the same time using forks in the program. A fork is used so that both the server and client are looking for sent messages and recieved messages at the same time, and if one is sent or recieved the interface is updated accordingly. There is also implementation to make sure there are no memory leaks in the program. There is also signal handling if the user exits the program using CTRL-C or other ways instead of using the '-exit' command in the program. If there is an interrupt signal, the program is gracefully cleaned up and exitted. 

# Console Image (Example)
![Lab%13%-%Systems%Programming.png](https://github.com/MSOE-CPE2600/final-project-mahneric53394/blob/main/Lab%2013%20-%20Systems%20Programming.png)

# Names
Eric Mahn and Braydon Hanson
