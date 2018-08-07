#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000

#define GROUP "239.0.0.2"

int main ()
{
	struct sockaddr_in localaddr;
	ssize_t len;
	char buf[BUFSIZ];
	struct ip_mreqn group;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	bzero(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &localaddr.sin_addr.s_addr);
	localaddr.sin_port = htons(CLIENT_PORT);
	
	bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));

	inet_pton(AF_INET, GROUP, &group.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", &group.imr_address);
	group.imr_ifindex = if_nametoindex("ens33");

	setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));

	while(1)
	{
		len = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, 0);
		write(STDOUT_FILENO, buf, len);
	}
	close(sockfd);
	return 0;

}
