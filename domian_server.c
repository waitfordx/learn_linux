#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#define SERV_ADDR  "serv.socket"

int main()
{
	struct sockaddr_un servaddr, clieaddr;
	char buf[4096];
	int size;

	int lfd = socket(AF_UNIX, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path, SERV_ADDR);

	int len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path);

	unlink(SERV_ADDR);
	bind(lfd, (struct sockaddr *)&servaddr, len);

	listen(lfd, 20);

	printf("accept....!\n");
	while(1)
	{
		len = sizeof(clieaddr);
		int cfd = accept(lfd, (struct sockaddr *)& clieaddr,(socklen_t *)&len);
		
		len -= offsetof(struct sockaddr_un, sun_path);
		clieaddr.sun_path[len] = '\0';

		printf("client bind filename %s\n", clieaddr.sun_path);
		while(( size = read(cfd, buf, sizeof(buf))) > 0)
		{
			for(int i =0 ; i< size; i++)
				buf[i] = toupper(buf[i]);
			write(cfd, buf, size);

		}
		close(cfd);
	}

	close(lfd); 

	return 0;
}
