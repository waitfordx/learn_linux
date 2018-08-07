#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 5000
#define MAXLINE 1500
#define CLIENT_PORT 5566

#define BROADCAST_IP "192.168.236.255"

int main()
{
	struct sockaddr_in serv_addr, clie_addr;
	char buf[MAXLINE];

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	int ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == 0)
		printf("bind ok!\n");

	int flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

	// set client port and ip for broadcast
	bzero(&clie_addr, sizeof(clie_addr));
	clie_addr.sin_family = AF_INET;
	inet_pton(AF_INET, BROADCAST_IP, &clie_addr.sin_addr.s_addr);
	clie_addr.sin_port = htons(CLIENT_PORT);

	int i = 0;
	while(1)
	{
		sprintf(buf, "this is a broadcast news , this is the %d times\n", ++i);

		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clie_addr, sizeof(clie_addr));
		printf("I have broadcast all of you! do you hear me?\n");
		sleep(2);
	}
	close(sockfd);

	return 0;
}
