#include<errno.h>
#include<sys/socket.h>
#include<stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include<stdlib.h>

int main(int argc,char* argv[])
{
  //CREATING A SOCKET
  int c;
  c = socket(AF_INET, SOCK_STREAM, 0);
  if (c==-1)
  {
    perror("The socket could not be created");
    exit(-1);
  }
  puts("Socket Created");
  
  if (argc < 3){ //make sure command line sets proper number of arguments
		printf("Please specify server's IPv4 address and port number\n");
		return 0;
	}


  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(atoi(argv[2]));
  serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
  
  
  //CONNECTING TO SERVER
  if (connect(c, (struct sockaddr *)&serveraddr, sizeof serveraddr) == -1)
  {
    perror("Failed to connect to server");
    exit(-1);
  }
  
  puts("Connected");
  
  //SENDING AND RECEIVING MESSAGE
  char cmsg[100];
  char smsg[100];
  int msg_len, bytes_sent, bytes_recv;
  
  while(1)
  {
    puts("Please Enter Text - ");
    
    //To check if EOF is encountered in the standard input
    if ((fgets(cmsg, 100, stdin) == NULL) || feof(stdin))
    {
      puts("\nEOF Encountered");
      break;
    }
    msg_len = strlen(cmsg);
    //Sending message to server. The +1 is to store null termination
    bytes_sent = send(c, cmsg, msg_len, 0);
    while (bytes_sent == -1)
    {
      puts("Failed to send message. Sending again.");
      bytes_sent = send(c, cmsg, msg_len, 0);
    }
    puts("Client Sent - ");
    puts(cmsg);
	
    //Receiving message from server.
    bytes_recv = recv(c, smsg, 100, 0);
    while (bytes_recv == -1)
    {
      puts("Failed to receive message. Trying again.");
      bytes_recv = recv(c, smsg, 100, 0);
    }
    
  	if (bytes_recv == 1){ //detects empty message
	  if (strlen(smsg) == 0){
		continue;
	  }
  	}
    puts("Server Response - ");
    puts(smsg);
    
    //Clearing the cmsg and smsg buffer
    memset(&cmsg, '\0', sizeof(cmsg));
    memset(&smsg, '\0', sizeof(smsg));
  }
  //CLOSING CONNECTION
  puts("Closing Connection");
  close(c);
} 