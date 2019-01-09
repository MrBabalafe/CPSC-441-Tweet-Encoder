#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 24536
#define MAX_MESSAGE_LENGTH 100

int main() {
  struct sockaddr_in address;
  char snd_message[MAX_MESSAGE_LENGTH];
  char rcv_message[MAX_MESSAGE_LENGTH*3];
  char c;

  //ADDRESS INITIALIZATION
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  //SOCKET CREATION
  int socket1;
  socket1 = socket(AF_INET, SOCK_STREAM, 0);
  if(socket1 < 0) {
    printf("socket creation failed!\n");
    exit(-1);
  } else {
    printf("Client socket created\n");
  }

  int connect_status;
  connect_status = connect(socket1, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
  if(connect_status < 0) {
    printf("connect() failed\n");
    exit(-1);
  } else {
    printf("Connected to server!\n");
  }

  while(1) {
    printf("Enter word: ");
    int len = 0;
    //Take user input and put into snd_message
    while((c = getchar()) != '\n') {
      snd_message[len] = c;
      len++;
    }

    //Make sure snd_message ends in 0
    snd_message[len] = '\0';

    //SEND MESSAGE
    send(socket1, snd_message, len, 0);
    printf("Sent: %s\n");
    int recv_status = recv(socket1, rcv_message, MAX_MESSAGE_LENGTH*3, 0);
    if(recv_status <= 0) {
      printf("Server failed\n");
      exit(-1);
    }

    //Make sure rcv_message ends in 0
    rcv_message[recv_status] = '\0';
    printf("Received Word: %s\n", rcv_message);

  }
  
}