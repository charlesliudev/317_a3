#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "dir.h"
#include "usage.h"
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFFSIZE 1024

// Here is an example of how to use the above function. It also shows
// one how to get the arguments passed on the command line.
/* this function is run by the second thread */

void *inc_x()
{
  printf("x increment finished\n");
  return NULL;
}

int main(int argc, char **argv) {

    // This is some sample code feel free to delete it
    // This is the main program for the thread version of nc

    int i;
    int sockfd, newsockfd, portno, clientresponse;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char recv_buf[BUFFSIZE];


    
    // Check the command line arguments
    if (argc != 2) {
      usage(argv[0]);
      return -1;
    }
    int port = atoi(argv[1]);
    printf("%d", port);


    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if (sockfd == -1){
      printf("socket creation failed...\n");
      exit(0);


    }
    else
    printf("socket created..\n");
    printf("%d", port);



    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);
    printf("%d", port);
    
    // bind socket to port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR on binding");
    }

    // listen for incoming connections
    listen(sockfd, 1);
    clilen = sizeof(cli_addr);
    
    // accept incoming connection
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        perror("ERROR on accept");

    // receive data from the client
    bzero(recv_buf, BUFFSIZE);
    int n = recv(newsockfd, recv_buf, BUFFSIZE - 1, 0);
    if (n < 0)
        perror("ERROR reading from socket");

    // process the received data
    printf("Received message from client: %s\n", recv_buf);

    // send a response to the client
    n = send(newsockfd, "220", 15, 0);
    if (n < 0)
        perror("ERROR writing to socket");
  
    int buffer[BUFFSIZE];
    


    close(newsockfd);
    close(sockfd);

    return 0;

}
