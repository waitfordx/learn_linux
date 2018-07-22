#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <string.h>

#define SERV_PORT 6000
#define SERV_IP "127.0.0.1"

int main()
{	
	// init 
	char buf[BUFSIZ];
	struct sockaddr_in serv_addr;

	memset(&serv_addr, 0 , sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr) ;
	socklen_t clie_addr_len;
	
	// make a socket
	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd == -1)
	{
		perror("socket error!");
		exit(1);
	}
	int retc = connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (retc == -1)
	{
		perror("connect error!");
		exit(1);
	}

	// to do

	while(1)
{
	fgets(buf,sizeof(buf),stdin);
	write(cfd, buf, strlen(buf));
	int n =	read(cfd, buf, sizeof(buf));
	write(STDOUT_FILENO, buf, n);

}	
	close(cfd);
	return 0;
}
