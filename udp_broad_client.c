#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 5000
#define MAXLINE 4096
#define CLIENT_PORT 5566

int main()
{
	struct sockaddr_in locaddr;
	char buf[MAXLINE];

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&locaddr, sizeof(locaddr));
	locaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &locaddr.sin_addr.s_addr);
	locaddr.sin_port = htons(CLIENT_PORT);

 	int ret = bind(sockfd, (struct sockaddr *)&locaddr, sizeof(locaddr));
	if (ret == 0)
		printf("bind successeful!...\n");
	
	while(1)
	{
		ssize_t len =recvfrom(sockfd, buf, sizeof(buf), 0, NULL, 0);
		printf("recived %d bite\n", len);
		write(STDOUT_FILENO, buf, len);
	}

	close(sockfd);

	return 0;
}
