#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>

#define SERV_PORT 6000
#define SERV_IP "127.0.0.1"

int main()
{
	// init serv_addr
	struct sockaddr_in serv_addr, client_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT); 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t client_addr_len;
	char buf[BUFSIZ];
	char client_ip[BUFSIZ];
	
		
	// make a new socket
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	// bind
	bind(lfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	
	listen(lfd, 128);
	
	client_addr_len= sizeof(client_addr);
	int cfd = accept(lfd,(struct sockaddr *) &client_addr, &client_addr_len);

	//print client message
	printf("congratulate!, ip: %s,	port: %d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)), ntohs(client_addr.sin_port));	

	// To do
	while(1)
	{	
		int n = read(cfd, buf, sizeof(buf)); 
		for (int i=0; i<n; i++)
			buf[i] = toupper(buf[i]);
		write(cfd, buf, n);
	}
	

	close(lfd);
	close(cfd);
	return 0;
}
