#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#define SERV_ADDR "serv.socket"
#define CLIE_ADDR "clie.socket"

int main()
{
	struct sockaddr_un servaddr, clieaddr;
	char buf[4096];

	int cfd = socket(AF_UNIX, SOCK_STREAM, 0);

	bzero (&clieaddr, sizeof(clieaddr));
	clieaddr.sun_family = AF_UNIX;
	strcpy(clieaddr.sun_path, CLIE_ADDR);

	int len = offsetof(struct sockaddr_un, sun_path ) + strlen(clieaddr.sun_path);

	unlink(CLIE_ADDR);
	bind(cfd, (struct sockaddr *)&clieaddr, len);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path, SERV_ADDR);

	len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path);

	connect(cfd,(struct sockaddr *)&servaddr, len);

	while(fgets(buf, sizeof(buf), stdin) != NULL)
	{
		write(cfd, buf, strlen(buf));
		len = read(cfd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, len);

	}
	close(cfd);
	return 0;
}
