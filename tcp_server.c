#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int main(){
	//send message to any client to connect
	char server_message[256] = "You have reached the server!";

	// create the server socket (1)
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0); // int domain, int type, int protocol

	//define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to our specified IP and port (2)
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	// int socket_file_discriptor, struct sockaddr* myaddr, int addrlen

	// Put socket into passive state(wait for connections rather than initiate a connection)
	listen(server_socket, 5); // (3)

	// Accept connections (4)
	int client_socket;
	client_socket = accept(server_socket, NULL, NULL); //Return file descriptor or -1
	// int sockfd, struct sockaddr* cliaddr, int* addrlen

	// send the message
	send(client_socket, server_message, sizeof(server_message), 0);

	// close the socket
	close(server_socket);

	return 0;
}
