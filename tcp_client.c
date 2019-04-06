#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main(){
	//create a socket (1)
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	//specify an address for the socket
	struct sockaddr_in server_address;       //specify where we wnat to connect to
	server_address.sin_family = AF_INET;

	//specify the port that we're going to connet to
	server_address.sin_port = htons(9002);
	//htnos function : converting actual port number to the appropriate data format so that our structure can understand the port number and we know exactly where we're supposed to be connecting to

	server_address.sin_addr.s_addr = INADDR_ANY;

	// (2)
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	//check for error with the connection
	if(connection_status == -1){
			printf("There was an error making a connection to the remote socket \n\n");
	}

	//recieve data from the server (3)
	char server_response[256];
	read(network_socket, &server_response, sizeof(server_response)); //read? recv?

	//print out the server's response
	printf("The server sent the data, %s\n", server_response);

	//and then close the socket
	close(network_socket);

	return 0;
}
