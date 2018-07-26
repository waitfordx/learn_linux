#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>

#define SERV_PORT 6006
#define SERV_IP "127.0.0.1"

int main ()
{
	//define the variables
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	char str[100];
	char buf[100];
	// recodrd the number of fds.
	int client[FD_SETSIZE];   
	int i;
	int n, sockfd,  nready;
	fd_set rset, allset;    /* rset note the Read sets, allset for temporary */

	// init the variables
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);
	printf("before socket!\n");	
	//  new a socket
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	// bind socket
	bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// listen socket
	listen(listenfd, 128);

	// use maxfd to record the max fd,  maxi to show the array subscript
	int maxfd = listenfd;
	int maxi = -1;
	
	for( i = 0; i<FD_SETSIZE; i++)
		client[i] = -1;								// init the array
	
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	// Reset the listening signal set for each loop
	while(1)
	{
		rset = allset;
		printf("test, before select!\n");
		 nready = select (maxfd+1, &rset, NULL, NULL, NULL);
		 printf("nready: %d\n", nready);
		if (nready < 0)
		{
			perror("select error");
			exit(1);
		}
		if (FD_ISSET(listenfd, &rset))
		{
			clie_addr_len = sizeof(clie_addr);

			printf("before accept!\n");
			// accept will not bloc to connect the client
			int connfd = accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
			printf ("complete ! IP : %s  PORT: %d\n", inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str))), ntohs(clie_addr.sin_port);
			// save the fd to the array of client		
			for (i = 0; i < FD_SETSIZE; i++ )
				if (client[i] < 0)
				{	
					client[i] = connfd;
					break;
				}
				
			if (i == FD_SETSIZE)
			{
				fputs("too many clients !", stderr);
				exit(1);
			}

			FD_SET(connfd, &allset);
			if (connfd > maxfd)
				maxfd = connfd;

			if (i > maxi)
				maxi = i;

			if (--nready == 0)
				continue;
			printf ("nready2 : %d\n", nready);
		}

		// solve the request form the connected client
		for (i = 0; i <= maxi; i++)
		{
			
			if ((sockfd = client[i]) < 0)
					continue;
				printf("sockfd!\n");
			if (FD_ISSET(sockfd, &rset))
			{
				n = read (sockfd, buf, sizeof(buf));
			 if ( n == 0)
				{
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
			 else if(n == -1)
				{
				while(errno == EINTR)
					n =	read(sockfd, buf, sizeof(buf));
				if(n > 0)
				{
					for (int j = 0; j < n; ++j)
						buf[j] = toupper(buf[j]);
					write(sockfd, buf, n);
				}
				else
				{
					perror("read error:");
					exit(1);
				}
			}
				else if(n > 0);
				{	
					printf ("read succeesfully! number: %d \n", n);
					for (int j =0; j < n; j++)
						buf[j] = toupper(buf[j]);
					printf("transform successfuly!\n");
				int p =	write(sockfd, buf, n);
					printf("write succefull! number : %d\n", p);
					write(STDOUT_FILENO, buf, n);
				}
				if (--nready == 0)
					break;
			}
		}
		
	}

	close(listenfd);
	return 0;
}
