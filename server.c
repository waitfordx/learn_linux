#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define SERV_PORT 6000
#define SERV_IP "127.0.0.1"

void wait_child(int signo)
{
	while (waitpid(0 , NULL, WNOHANG) >0);
	return ;
}

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
	if (lfd == -1)
	{
		perror("socket error!");
		exit(1);
	}

	// bind
	int ret = bind(lfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret == -1)
	{
		perror("bind error!");
		exit(1);
	}
	
	int retl = listen(lfd, 128);
	if (retl == -1)
	{
		perror("listen error!");
		exit(1);
	}
	
	client_addr_len= sizeof(client_addr);
	while(1){
	int cfd = accept(lfd,(struct sockaddr *) &client_addr, &client_addr_len);
	if (cfd == -1)
	{
		perror("accept error!");
		exit(1);
	}
	
	// fork children program
	pid_t	pid = fork();
	if (pid < 0)
	{
		perror("fork error:");
		exit(1);
	}

	// child process
	else if(pid == 0)
	{
		close(lfd);
		while(1)
		{
			int n = read (cfd, buf, sizeof(buf));
			if(n == 0)			// client closed
			{
				close(cfd);
				return 0;
			}
			else if(n == -1)
			{
				while(errno == EINTR)
					n =	read(cfd, buf, sizeof(buf));
				if(n > 0)
				{
					for (int i = 0; i < n; ++i)
						buf[i] = toupper(buf[i]);
					write(cfd, buf, n);
				}
				else
				{
					perror("read error:");
					exit(1);
				}
			}
			else if (n > 0)
			{
				for (int i = 0; i < n; ++i)
					buf[i] = toupper(buf[i]);
				write(cfd, buf, n);
			}
		}		
 	}

	// parent process
	else 
	{
		close(cfd);
		// call a function to catch the signal to collect child process!
		signal(SIGCHLD, wait_child); 

	}

	

	//print client message
	printf("congratulate!, ip: %s,	port: %d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)), ntohs(client_addr.sin_port));	

	// To do
/*	while(1)
	{	
		int n = read(cfd, buf, sizeof(buf)); 
		for (int i=0; i<n; i++)
			buf[i] = toupper(buf[i]);
		write(cfd, buf, n);
	}
	

	close(lfd);
	close(cfd);*/
	}
	return 0;
}
