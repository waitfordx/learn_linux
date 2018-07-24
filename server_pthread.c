#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#define SERV_PORT 6000
#define SERV_IP "127.0.0.1"

struct s_info{
	struct sockaddr_in cliaddr;
	int cfd;
};

void* do_work(void *arg)
{
	struct s_info *ts = (struct s_info*)arg;
	char client_ip[BUFSIZ];
	char buf[BUFSIZ];
	
	while(1)
	{
		int n = read(ts->cfd, buf, sizeof(buf));
		if (n == 0)
		{
			printf("the client %d closed\n !", ts->cfd);
			break;
		}
	//print client message
	printf("congratulate!, ip: %s,	port: %d\n", inet_ntop(AF_INET, &(*ts).cliaddr.sin_addr.s_addr, client_ip, sizeof(client_ip)), ntohs((*ts).cliaddr.sin_port));		
		for(int i = 0; i<n ; i++)
		{
			buf[i] = toupper(buf[i]);
		}
		write (ts->cfd, buf, n);

	}
	close(ts->cfd);

	return (void*)0;
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
	pthread_t tid; 

	struct s_info ts[256];            // max number of thread	
	int i;	
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

	ts[i].cliaddr = client_addr;
	ts[i].cfd = cfd;

	pthread_create(&tid, NULL, do_work, (void*) &ts[i]);
	pthread_detach(tid);
	i++;

	
 }
 return 0;
 }
