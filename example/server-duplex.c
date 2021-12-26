
#include<arpa/inet.h>





#include<ctype.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "sock_dg_inet.h"
  
int main(){

	char linha[100];

	int sock_fd;
	sock_fd= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(SOCK_PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;



	int err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}



	printf(" socket created and binded \n ");
	printf("Ready to receive messages\n");
	char buffer[100];
	int nbytes;


	char reply_message[100];
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(struct sockaddr_in);
	while(1){
		printf("waiting for 100 bytes\n");
		nbytes = recvfrom(sock_fd, buffer, 100, 0,
		                  ( struct sockaddr *)&client_addr, &client_addr_size);

		char remote_addr_str[100];
		int remote_port = ntohs(client_addr.sin_port);
		if (inet_ntop(AF_INET, &client_addr.sin_addr, remote_addr_str, 100) == NULL){
			perror("converting remote addr: ");
		}

		printf("received %d bytes from %s %d:\n", nbytes, remote_addr_str, remote_port);
		for (int i = 0 ; i < nbytes ; i++){
			printf("%d - %c \n", i, buffer[i]);
			reply_message[i] = toupper(buffer[i]);
		}
		printf("going to send a reply (press enter to continue\n");
		fgets(linha, 100, stdin);

		nbytes = sendto(sock_fd,
	                    reply_message, strlen(reply_message)+1, 0,
	                    (const struct sockaddr *) &client_addr, client_addr_size);
		printf("\nsent %d %s\n\n", nbytes, reply_message);

	}

	close(sock_fd);
	exit(0);
}
