#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

void cleanExit();

int main(){
	signal(SIGTERM, cleanExit);
	signal(SIGINT, cleanExit);

	int n;
	socklen_t clilen;
	//send message to any client to connect
	char server_message[256] = "You have reached the server!";
	char buf[256];

	// create the server socket (1)
	int server_socket; //sockfd
	int client_socket; //new
	server_socket = socket(AF_INET, SOCK_STREAM, 0); // int domain, int type, int protocol
	if(server_socket < 0){
		error("ERROR opening socket");
	}

	//define the server address
	struct sockaddr_in server_address, client_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to our specified IP and port (2)
	if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0){
		error("ERROR on binding");
	}
	// int socket_file_discriptor, struct sockaddr* myaddr, int addrlen

	// Put socket into passive state(wait for connections rather than initiate a connection)
	listen(server_socket, 5); // (3)

	clilen = sizeof(client_address);

	while(1){
		// Accept connections (4)
		client_socket = accept(server_socket, (struct sockaddr * ) &client_address, &clilen); //Return file descriptor or -1
		// int sockfd, struct sockaddr* cliaddr, int* addrlen
		
		if(client_socket < 0){
			error("ERROR on accept");
		}

		// read client's data request
		bzero(buf, 256);
		n = read(client_socket, buf, 255);
		if(n < 0){
			error("ERROR reading from socket");
		}
		printf("[Dumping]client's request is : %s", buf);

		if(strncmp(buf, "exit", 4) == 0){
			printf("Connection End\n");
			break;
		}

		// send the message, that is... write the message to client
		n = write(client_socket, server_message, sizeof(server_message)); //write? send?
		if(n < 0){
			error("ERROR writing to socket");
		}

	}
	// close the socket
	close(server_socket);
	close(client_socket);

	return 0;
}

void cleanExit(){
	exit(0);
}
