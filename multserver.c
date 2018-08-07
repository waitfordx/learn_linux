#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000
#define MAXLINE 1500

#define GROUP "239.0.0.2"

int main()
{
	struct sockaddr_in serv_addr, clie_addr;
	char buf[MAXLINE];
	struct ip_mreqn group;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	bind (sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	
	inet_pton(AF_INET, GROUP, &group.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", &group.imr_address);
	group.imr_ifindex = if_nametoindex("ens33");

	setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &group, sizeof(group));
	
	bzero(&clie_addr, sizeof(clie_addr));
	clie_addr.sin_family = AF_INET;
	inet_pton(AF_INET, GROUP, &clie_addr.sin_addr.s_addr);
	clie_addr.sin_port = htons(CLIENT_PORT);

	int i = 0;
	while(1)
	{
		sprintf(buf, "this is a multicast program %d\n", ++i);
		ssize_t len = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clie_addr, sizeof(clie_addr));
		printf("i have send %d bit\n", len);
		sleep(2);
	}
	close(sockfd);

	return 0;
}
