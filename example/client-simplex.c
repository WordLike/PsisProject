#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>


#include<ctype.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "sock_dg_inet.h"

int main(){

	char buff[100]; 
	int nbytes;


	
	int sock_fd= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	printf(" socket created \n Ready to send\n");


	char linha[100];


	char message[100];

	printf("What is the network address of the recipient? ");
	fgets(linha, 100, stdin);
	linha[strlen(linha)-1] = '\0';

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SOCK_PORT);
	if( inet_pton(AF_INET, linha, &server_addr.sin_addr) < 1){
		printf("no valid address: \n");
		exit(-1);
	}

	while(1){
		printf("Type the message you want to send to the server ");
		fgets(message, 100, stdin);
		printf("Going to send a %lu bytes message (it may contain the \\n and \\0 :p \n", strlen(linha)+1);
		nbytes = sendto(sock_fd,
	                    message, strlen(message)+1, 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
		printf("\nsent %d %s\n\n", nbytes, buff);

	}
	close(sock_fd);
	exit(0);
}
