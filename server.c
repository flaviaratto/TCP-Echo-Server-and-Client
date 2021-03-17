#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>

//Written Function (returns number of bytes written or -1 for error):
int written(int descriptor, char data[], int dataLength){
	int sent = send(descriptor, data, dataLength, 0); //send message to the socket
	
	if(sent == -1){ //if error in sending, return 0 bytes sent
	
		if (errno == EINTR){ //if process is interrupted, resend it again:
		
			printf("Server: Write Interrupted. Rewriting Now.\n");
			written(descriptor,data,dataLength);
		}
		
		perror("Server: Write Error");
		return -1;
	}
	
	return sent; //if transmitting message is successful, return number of bytes sent
}

//Reading Function lets server read data received from client:
int reading(int descriptor, char data[], int dataLength){
	int readBytes = recv(descriptor, data, dataLength, 0); //reads message from client
	
	if (dataLength == -1){ //if recv() returns -1, there is an error
		perror("Server: read error so read again");
		
		while(dataLength == -1){ //keep reading until no error 
			dataLength = recv(descriptor, data, dataLength, 0); 
		}
	}
	return readBytes; //return number of bytes read
}

int main(int argc, char *argv[]){ //command line: echos port (echos is name of program, port is port number)

	//Declare parameters/variables used:
	int sckt; //socket descriptor
	struct sockaddr_in my_addr; //server socket address
	struct sockaddr_in nxt_client; //client socket address wanting to connect in port
	int new_sckt; //next socket descriptor to connect to client
	char msg[1024]; //message to be echoed
	char emp[1024]; memset(&emp,'\0', sizeof(emp)); //comparator to detect "no text" from client (detects whitespace)
	char emp1[1024]; memset(&emp1,'\0', sizeof(emp1)); //comparator to detect "no text" from client (detects tab character)
	int msgLength; //length of the message recieved by server
	int num_bytes; //number of bytes written to socket using written()
	int pid; //child process identification
	int sizeAddr = sizeof(struct sockaddr_in); //size of socket address
	
	int yes = 1;
	
	//Establish socket descriptor:
	if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) == -1){ //assign socket descriptor
		perror("Server: socket"); // handling error
		exit(-1);
	}
	
	if (setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){ //reuse port if address is already in use
		perror("Server: setsockopt"); // handling error
		exit(-1);
	}
	
	//Get information from socket address:
	my_addr.sin_family = AF_INET; //host byte order
	
	if (argc < 2){ //make sure command line sets proper number of arguments
		printf("Please specify the port number\n");
		return 0;
	}
	else{
		my_addr.sin_port = htons(atoi(argv[1])); // short, network byte order (atoi to convert input argument from char to integer)
	}
	
	my_addr.sin_addr.s_addr = INADDR_ANY; //automatically fill with server's IP	
	
	memset(&my_addr.sin_zero, '\0', 8); //clean lingering bytes used previously in address
	
	printf("Server: Starting connection to socket\n");
	
	//Bind:
	if (bind(sckt, (struct sockaddr*) &my_addr, sizeAddr) == -1){
		perror("Server: Bind Error");
		exit(-1);
	}
	
	//Listen:
	if (listen(sckt, 10) == -1){
		perror("Server: Listen Error");
		exit(-1);
	}
	
	printf("Server: Listening in port %s\n", argv[1]);
	
	//Accept thread:
	while(1){
		
		//accept clients wanting to connect:
		if ((new_sckt = accept(sckt, (struct sockaddr*) &nxt_client, &sizeAddr)) == -1){
			perror("Server: Accept Error");
			continue;
		}
		
		printf("Server: Accepted %s client's request to communicate\n", inet_ntoa(nxt_client.sin_addr));
		
		if (setsockopt(new_sckt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){ //reuse port if address is already in use
			perror("Server: setsockopt"); // handling error
			exit(-1);
		}
		
		pid = fork(); //create child process for communicating with each client
		
		if (pid != 0){
			printf("Server: Child process with pid %d\n", pid);
		}
		
		if (!pid){ //child processes
			close(sckt); //child process no longer needs this socket descriptor
			memset(&msg, '\0', sizeof(msg)); //initialize message to be empty
			msgLength = reading(new_sckt, msg, 1024); //keep reading message from client
			
			while(1){ //echo back messages from this client until client no longer send a message
				if ((&msg[msgLength-1] == NULL) || (msgLength <= 0)){ //if last character inputed is indicates EOF, it means client is disconnected
					break;
				}
				
				memset(&emp,' ', msgLength-1); //set comparator as whitespace of the same length of message except the termination ('\n')
				memset(&emp1,'\t', msgLength-1); //set comparator as tab of the same length of message except the termination ('\n')
				
				msg[msgLength-1] = '\0'; //delete last charater which is '\n'
				if ((strcmp(msg,emp) != 0) && (strcmp(msg,emp1) != 0)){ //do not echo when no text/character is sent (whitespace is not considered)
					
					msg[msgLength-1] = '\n'; //include the '\n' back into message
					
					printf("Server: Recieved Message of %d bytes from %s: %s", msgLength, inet_ntoa(nxt_client.sin_addr), msg); //print message recieved by server
				
					num_bytes = written(new_sckt, msg, msgLength); //write back the same message to client
					
					//Check for error in writing message to socket:
					if (num_bytes != -1){
						
						//Print result to terminal with the number of bytes written:
						printf("Server: Echoed Message of %d bytes from %s: %s", num_bytes, inet_ntoa(nxt_client.sin_addr), msg);
					}
					else{
						printf("Server: Error in echoing message\n");
					}
				}
				else{
					written(new_sckt, "\0", 1); //write nothing back to client
				}
				memset(&msg, '\0', sizeof(msg)); //clear/reset message
				memset(&emp,'\0', sizeof(emp)); memset(&emp1,'\0', sizeof(emp1));//clear/reset comparators
				msgLength = reading(new_sckt, msg, 1024); //keep reading message from client
			}
			
			//EOF for client so close this socket:
			printf("Server: %s client is disconnected\n", inet_ntoa(nxt_client.sin_addr));
			close(new_sckt); //close communication this socket
			exit(0);
		}
		close(new_sckt); //close communication this socket
	}
	
	return 0;
}