/* epoll based on non_blocking i/o event driven*/
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BUFLEN 4096
#define MAX_EVENTS 1024
#define SERV_PORT 6060

// define a struct to describe ready fd related information

struct  myevent_s {
	int fd;										// file descriptor
	int events;									// monitored events				
	void *arg;									// argument
	void (* call_back) (int fd, int events, void *arg);				//  call back function
	int status;									// on the red and black tree			
	char buf[BUFLEN];							
	int len;
	long last_active;							// record time values
};

int g_efd;										// global variables ,save a fd returned by epoll_creat
struct myevent_s g_events[MAX_EVENTS+1];		// array , +1 --> listen fd

void recvdata(int fd, int events, void *arg);
void senddata(int, int, void *);
// init struct myevent_s member 

void eventset (struct myevent_s *ev, int fd, void (*call_back)(int , int ,void *), void *arg)
{
	ev->fd = fd;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	memset (ev->buf, 0, sizeof(ev->buf));
	ev->len = 0;
	ev->last_active = time(NULL);											// the time of call back evenset fuction 

	return ;
}

// addd a fd to the tree which monitored by epoll

void eventadd (int efd, int events, struct myevent_s *ev)
{
	struct epoll_event epv = {0, {0}};
	int op;
	epv.data.ptr = ev;
	epv.events = ev->events = events;		// EPOLLIN or EPOLLOUT

	if (ev->status == 1)
	{
		op = EPOLL_CTL_MOD;
	}
	else 					// add the ev to the tree 
	{
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}
	
	if(epoll_ctl(efd, op, ev->fd, &epv) < 0)	// actual operation
		printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
	else 
		printf("events add sucessfully! [fd=%d], op=%d, events[%OX]\n", ev->fd, op, events);

	return;
}


// delete a fd from the tree which monitored by epoll

void eventdel(int efd, struct myevent_s *ev)
{
	struct epoll_event epv = {0, {0}};      // define and initialize a struct variable
	
	if (ev->status != 1)
		return ;
	
	epv.data.ptr = ev;
	ev->status = 0;
	epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv );

	return;
}


// connect with the client when the fd is ready

void acceptconn(int lfd, int events, void *arg)
{
	struct sockaddr_in cliein;
	socklen_t len = sizeof(cliein);
	int cfd, i;

	if((cfd = accept (lfd, (struct sockaddr *)&cliein, &len)) == -1)
	{
		if(errno != EAGAIN && errno != EINTR)
		{
			perror("aceept error:");
			exit(1);
		}
		printf("%s: accept, %s\n", __func__, strerror(errno));
		return ;
	}
	
	do 
	{
		for (i = 0; i < MAX_EVENTS; i++)	// find an idle element from the global array
			if(g_events[i].status == 0)
				break;
		
		if (i == MAX_EVENTS)
		{
			printf("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
			break;          				// jump out of   do while   loop
		}

		int flag = 0;
		if((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0)    // set cfd noblock
		{
			printf("%s: fcntl noblocking failed, %s\n", __func__, strerror(errno));
			break;
		}
		
		// give cfd a myevent_s struct ,call back recvdata function
		eventset(&g_events[i], cfd, recvdata, &g_events[i]); 
		eventadd(g_efd, EPOLLIN, &g_events[i]);

	}while(0);
	
	printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cliein.sin_addr), ntohs(cliein.sin_port), g_events[i].last_active, i);
	return;
}

// recieve the data from client
void recvdata(int fd, int events, void *arg)
{
	struct myevent_s *ev = (struct myevent_s *)arg;
	int len;

	len = recv(fd, ev->buf, sizeof(ev->buf), 0);

	eventdel(g_efd, ev);

	if(len > 0)
	{
		ev->len = len;
		ev->buf[len] = '\0';
		printf("C[%d]:%s\n", fd, ev->buf);

		eventset(ev, fd, senddata, ev);
		eventadd(g_efd, EPOLLOUT, ev);
	}
	else if (len == 0)
	{
		close(ev->fd);
		printf("[fd=%d] pos[%ld], closed\n ", fd, ev-g_events);
	}
	else
	{
		close(ev->fd);
		printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
	}
	return ;
}

// send data to the client after working with the data
void senddata (int fd, int events, void *arg)
{
	struct myevent_s *ev = (struct myevent_s *)arg;
	int len;

	len = send(fd, ev->buf, ev->len, 0);

	if (len > 0)
	{
		printf("send[fd=%d], [%d]%s\n", fd, len, ev->buf);
		eventdel(g_efd, ev);
		eventset(ev, fd, recvdata, ev);
		eventadd(g_efd, EPOLLIN, ev);
	}
	else 
	{
		close(ev->fd);
		eventdel(g_efd, ev);
		printf("send[fd=%d] error %s\n", fd, strerror(errno));
		
	}
	return ;
}

// creat a new socket, initialized lfd

void initlistensocket(int efd, short port)
{
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(lfd, F_SETFL, O_NONBLOCK);

	// set the lfd's callback function and add lfd to the red and black tree
	eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
	eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

	struct sockaddr_in sin;
	memset(&sin, 0 , sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	bind(lfd, (struct sockaddr *)&sin, sizeof(sin));

	listen(lfd, 128);

	return ;
}

int main(int argc, char *argv[])
{
	unsigned short port = SERV_PORT;

	if(argc = 2)
		port = atoi(argv[1]);
	
	g_efd = epoll_create(MAX_EVENTS+1);
	if(g_efd <= 0)
		printf("create efd in %s error %s\n", __func__, strerror(errno));

	initlistensocket(g_efd, port);

	struct epoll_event events[MAX_EVENTS+1];
	printf("server runing: port[%d]\n", port);

	int checkpos = 0;
	int i;
	// time out verification 
	while(1)
	{
		long now = time(NULL);
		for(i = 0; i < 100; i++, checkpos++)
		{
			if(checkpos == MAX_EVENTS)
				checkpos = 0;
			if(g_events[checkpos].status != 1)
				continue;

			long duration = now - g_events[checkpos].last_active;
			
			if(duration >= 60)
			{
				close(g_events[checkpos].fd);
				printf("[fd=%d] timeout\n", g_events[checkpos].fd);
				eventdel(g_efd, &g_events[checkpos]);
			}
		}
		// listen the tree g_efd, add the fd which satisfies the event
		int nfd = epoll_wait(g_efd, events, MAX_EVENTS+1, 1000);
		if(nfd < 0)
		{
			printf("epoll_wait error , exit\n");
			break;
		}	

		for (i = 0; i < nfd; i++)
		{
			struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
			
			if((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
				ev->call_back(ev->fd, events[i].events, ev->arg);
			if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
				ev->call_back(ev->fd, events[i].events, ev->arg);
		}
	}

	// free resources


return 0;

}
