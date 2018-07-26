#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/time.h>
#include <netinet/in.h>

#define SERV_PORT 6008
#define SERV_IP "127.0.0.1"
#define OPEN_MAX 1024
#define MAX_LINE 80
int main ()
{
	//define the variables
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	char str[INET_ADDRSTRLEN];
	char buf[MAX_LINE];
	// recodrd the number of fds.
	struct pollfd client[OPEN_MAX];   
	int i, maxi;
	int sockfd,  nready;
	ssize_t n;


	// init the variables
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	//  new a socket
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// bind socket
	bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// listen socket
	listen(listenfd, 128);

	client[0].fd = listenfd ;
	client[0].events = POLLIN;

	// use maxfd to record the max fd,  maxi to show the array subscript
	
	for (i =1; i  < OPEN_MAX; i++)
		client[i].fd = -1;							// init the array
	
	maxi = 0;



	// Reset the listening signal set for each loop
	while(1)
	{


		 nready = poll(client, maxi+1, -1);
		printf("nready: %d\n", nready);
		
		if (nready < 0)
		{
			perror("select error");
			exit(1);
		}
		if (client[0].revents & POLLIN)
		{
			clie_addr_len = sizeof(clie_addr);

	
			// accept will not bloc to connect the client
			int connfd = accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
			printf ("complete ! IP : %s  PORT: %d\n", inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str))), ntohs(clie_addr.sin_port);
			// save the fd to the array of client		
			for (i = 0; i < OPEN_MAX; i++ )
				if (client[i].fd < 0)
				{	
					client[i].fd = connfd;
					break;
				}
				
			if (i == OPEN_MAX)
			{
				fputs("too many clients !", stderr);
				exit(1);
			}

			client[i].events = POLLIN;



			if (i > maxi)
				maxi = i;

			if (--nready == 0)
				continue;
	
		}

		// solve the request form the connected client
		for (i = 1; i <= maxi; i++)
		{
			
			if ((sockfd = client[i].fd) < 0)
					continue;
	
			if (client[i].revents & POLLIN )
			{
				n = read (sockfd, buf, sizeof(buf));
		    	if ( n < 0)
				{
					if(errno == ECONNRESET)
					{
						printf("client[%d] aborted connection \n", i);
						close(sockfd);					
						client[i].fd = -1;
					}
					else
					{
						perror("read error");
						exit(1);
					}
				}
			    else if(n == 0)
				{
					printf ("client[%d] closed connection\n", i);
					close(sockfd);
					client[i].fd = -1;
				}
				else 
				{	
				
					for (int j =0; j < n; j++)
						buf[j] = toupper(buf[j]);				
					int p =	write(sockfd, buf, n);
			
				}
				if (--nready <= 0)
					break;
			}
		}
		
	}

	close(listenfd);
	return 0;
}
