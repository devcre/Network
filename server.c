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

char *content_type(char *ext);

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
    char resp_buf[1024];

    // HTTP request data
    char *pot1, *pot2;
    char *extension;

    // html
    FILE* fp;
    FILE* fpp;
    int file_size;
    int data_size;

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

    while(1){
        // printf("bzero\n");
        bzero(buffer,256);
        bzero(resp_buf,1024);
        //bzero(extension,256);
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

        // parse page data and get URL
        pot1 = strtok(buffer," ");
        pot1 = strtok(NULL, " ");
        printf("%s\n", pot1);
        if(strcmp(pot1,"/")==0){
            sprintf(resp_buf, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Type: text/plain\r\n\r\n");
            n = write(newsockfd,resp_buf,strlen(resp_buf));
            printf("%s\n", resp_buf);
            if (n < 0) error("ERROR writing to socket");
        }
        else{
            pot2 = strtok(pot1, ".");
            pot2 = strtok(NULL,"");
            printf("%s\n", pot2);
            // pot = strtok(NULL, " \r\n");
            extension = content_type(pot2);

            // size of html file
            fp = fopen("page.html", "r");
            fpp = fopen("page.html", "r");
            fseek(fp, 0, SEEK_END);
            file_size = ftell(fp);

            char resp_data[file_size];
            bzero(resp_data, file_size);
            data_size = fread(resp_data, 1, file_size, fpp);

            // response message
            sprintf(resp_buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n", file_size, extension);
            printf("%s\n",resp_buf);
            // send response message

            n = write(newsockfd, resp_buf, strlen(resp_buf)); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send
            if (n < 0) error("ERROR writing to socket");
            printf("%d",n);

            // transfer html file data
            n = write(newsockfd, resp_data, data_size); 
            if (n < 0) error("ERROR writing to socket");

            fclose(fp);
            fclose(fpp);
        }
    }
    close(sockfd);
    close(newsockfd);

    return 0; 
}

char *content_type(char *ext){
    if(strcmp(ext,"html")==0){
        return "text/html";
    }
    else if(strcmp(ext,"ico")==0){
        return "image/x-icon";
    }
    else if(strcmp(ext,"mp3")==0){
        return "video/mpeg3";
    }
    else if(strcmp(ext,"pdf")==0){
        return "application/pdf";
    }
    else if(strcmp(ext,"jpeg")==0 || strcmp(ext,"jpg")==0){
        return "image/jpeg";
    }
    else if(strcmp(ext,"gif")==0){
        return "image/gif";
    }
    else{
        return "text/plain";
    }
}

