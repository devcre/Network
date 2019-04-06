#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

void cleanExit();

int main(){
	signal(SIGTERM, cleanExit);
	signal(SIGINT, cleanExit);

	int n;
	//create a socket (1)
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(network_socket < 0){
		error("ERROR opening socket");
	}

	//specify an address for the socket
	struct sockaddr_in server_address;       //specify where we wnat to connect to
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;

	//specify the port that we're going to connet to
	server_address.sin_port = htons(9002);
	//htnos function : converting actual port number to the appropriate data format so that our structure can understand the port number and we know exactly where we're supposed to be connecting to
	server_address.sin_addr.s_addr = INADDR_ANY;

	// (2)
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	//check for error with the connection
	if(connection_status < 0){
		printf("There was an error making a connection to the remote socket \n\n");
		exit(0);
	}

	char buf[256];

	//send data request (3)
	printf("Write message to send >> ");
	bzero(buf, 256);
	fgets(buf, 255, stdin);
	if(strncmp(buf, "exit", 4) == 0){
		printf("End connection\n");
		exit(0);
	}

	n = write(network_socket, buf, strlen(buf));
	if(n < 0){
		error("ERROR writing to socket");
	}

	bzero(buf, 256);
	//recieve data from the server (4)
	n = read(network_socket, buf, sizeof(buf)); //read? recv?
	if(n < 0){
		error("ERROR reading from socket");
	}

	//print out the server's response
	printf("The server sent the data, %s\n", buf);


	//and then close the socket
	close(network_socket);

	return 0;
}

void cleanExit(){
	exit(0);
}
