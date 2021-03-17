# TCP-Echo-Server-and-Client
Implementation of the client and server for a simple TCP echo service
  

ECEN602 Programming Assignment 1
1. Flavia Ratto
2. Eric Lloyd Robles

## Assignment Overview

In this assignment, we implemented the client and server for a simple TCP echo service, which does the following:
1. Start the server first with the command line: echos Port, where echos is the name of the server program and Port is the port number on which the server is listening. The server is able to support multiple simultaneous connections.
2. Start the client second with a command line: echo IPAdr Port, where echo is the name of the client program, IPAdr is the IPv4 address of the server in dotted decimal notation, and Port is the port number on which the server is listening.
3. The client reads a line of text from its standard input and writes the line to the network socket connected to the server.
4. The sever reads the line from its network socket and echoes the line back to the client.
5. The client reads the echoed line and prints it on its standard input.
6. When the client reads an EOF from its standard input (e.g., terminal input of Control-D), it closes the socket and exits. When the client closes the socket, the server will receive a TCP FIN packet, and the server child process’ read() command will return with a 0. The child process will then exit.


## Steps to compile and run

1. In order to compile the code, make sure the makefile, client.c and server.c are in the same directory/folder/. Type in the command:
    ```
    make
    ```
2. To start the server, type in the command line:
    ```
    ./server Port
    ```
3. To start the client, type in the command line: ./client IPAdr Port
    ```
    ./client IPAdr Port
    ```

**Note:** - The code is written in C and is compiled and tested in a Linux environment.
