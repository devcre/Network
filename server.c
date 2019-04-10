/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void cleanExit(){
    exit(0);
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, cleanExit);
    signal(SIGINT, cleanExit);
    int opt = 1;

    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno; // port number
    socklen_t clilen;

    char buffer[256];

    // HTTP request data
    char *req_data[3];
    char *pot;
    int j, count;

    /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, cli_addr;

    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    /*Create a new socket
    AF_INET: Address Domain is Internet 
    SOCK_STREAM: Socket Type is STREAM Socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("ERROR opening socket");
    }
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); //atoi converts from String to Integer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
    serv_addr.sin_port = htons(portno); //convert from host to network byte order

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ //Bind the socket to the server address
        error("ERROR on binding");
    }
    listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5

    clilen = sizeof(cli_addr);
    /*accept function: 
    1) Block until a new connection is established
    2) the new socket descriptor will be used for subsequent communication with the newly connected client.
    */
    count = 0;
    while(1){
        bzero(buffer,256);
        //bzero(req_data,3);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
            error("ERROR on accept");
        }

        n = read(newsockfd,buffer,255); //Read is a block function. It will read at most 255 bytes
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);

        if(strncmp(buffer, "exit", 4) == 0){
            printf("End connection\n");
            close(sockfd);        
            close(newsockfd);
            cleanExit();
        }

        // parse data
        // if(count == 0){
        //     pot = strtok(buffer," ");
        //     for(j=0; j<3; j++){
        //         req_data[j] = pot;
        //         printf("%s\n", req_data[j]);
        //         pot = strtok(NULL, " ");
        //     }
        //     count = count + 1;
        // }

        n = write(newsockfd,buffer,sizeof(buffer)); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send
        if (n < 0) error("ERROR writing to socket");
        printf(" Write: %s\n", buffer);
    }
    close(sockfd);
    close(newsockfd);

    return 0; 
}
