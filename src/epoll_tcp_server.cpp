/********************************************************************
	created:	2018/03/26
	created:	26:3:2018   14:40
	filename: 	src/epoll_tcp_server.cpp
	file path:	src
	file base:	epoll_tcp_server
	file ext:	cpp
	author:		huanglb
	
	purpose:	a simple epoll server.
*********************************************************************/
#include "../inc/sw_tcp_socket_utils.h"
#include <sys/epoll.h>
#include <string>

#define IP "127.0.0.1"
#define	PORT 9090
#define  EPOLLEVENT 1000
#define  MAXLINE 1024
#define  FDSIZE 1000

void add_event(int epfd, int fd, int stat)
{
	struct epoll_event epevent;
	epevent.data.fd = fd;
	epevent.events = stat;

	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epevent);

	return;
}

void delete_event(int epfd, int fd, int stat)
{
	struct epoll_event epevent;
	epevent.data.fd = fd;
	epevent.events = stat;

	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epevent);

	return;
}

void modify_epoll(int epfd, int fd, int stat)
{
	struct epoll_event epevent;
	epevent.data.fd = fd;
	epevent.events = stat;

	epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epevent);

	return;
}

int eread(int epfd, int fd, char *buf)
{
again:
	int nbytes = read(fd, buf, MAXLINE);
	//
	if (nbytes < 0)
	{
		if (errno == EINTR)
		{
			log();
			goto again;
		}
		else
		{
			log();
			printf("read error. \n");
			shutdown(fd, SHUT_RDWR);
			delete_event(epfd, fd, EPOLLIN);
		}
	}
	else if (nbytes == 0) //socket disconnect.
	{
		log();
		printf("socket disconnect.. \n");
		delete_event(epfd, fd, EPOLLIN);
	}
	else
	{
		log();
		printf("recv msg: %s... \n", buf);
		modify_epoll(epfd, fd, EPOLLOUT);
	}

	return 0;
}

int ewrite(int epfd, int fd, char *buf)
{
again:
	int nbytes = write(fd, buf, strlen(buf));
	if (nbytes < 0)
	{
		if (errno == EINTR)
		{
			log();
			goto again;
		}
		else
		{
			log();
			printf("write failed...\n");
			shutdown(fd, SHUT_RDWR);
			delete_event(epfd, fd, EPOLLOUT);
		}
	}
	else if (nbytes == 0)
	{
		log();
		printf("write buf is full. again.");
		return 0;
	}
	else
	{
		log();
		printf("send msg: %s. \n", buf);

		modify_epoll(epfd, fd, EPOLLIN);
	}

	memset(buf, 0x00, MAXLINE);

	return 0;
}

void eaccpet(int epfd, int fd)
{
	struct sockaddr_in clientaddr;
	bzero(&clientaddr, sizeof(clientaddr));

	int clientfd = -1;
	size_t clientlen = sizeof(clientaddr);

	clientfd = accept(fd, (struct sockaddr *)&clientaddr, &clientlen);

	// add to epoll_tree
	if (clientfd < 0)
	{
		log();
		printf("accept failed. \n");
		return;
	}
	else
	{
		printf("accept a new client: %s, %d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		add_event(epfd, clientfd, EPOLLIN);
	}

	return;
}

void process_events(int epfd, struct epoll_event *pevents, int num, int listenfd, char *buf)
{
	int fd = -1;
	for (int i = 0; i < num; i++)
	{
		fd = pevents[i].data.fd;
		
		if ((fd == listenfd) && (pevents[i].events & EPOLLIN))
		{
			eaccpet(epfd, fd);
		}
		else if (pevents[i].events & EPOLLIN)
		{
			eread(epfd, fd, buf);
		}
		else if (pevents[i].events & EPOLLOUT)
		{
			char str[16] = {"hello"};
			strncpy(buf, str, strlen(str));
			ewrite(epfd, fd, buf);
		}
	}

	return;
}

void start_epoll(int listenfd)
{
	// epoll
	int epfd = epoll_create(FDSIZE);
	if (epfd < 0)
	{
		log();
		printf("epoll_create failed. \n");
		return;
	}

	struct epoll_event event[EPOLLEVENT];
	char buf[MAXLINE];
	memset(buf,0,MAXLINE);
	add_event(epfd,listenfd,EPOLLIN);
	
	for(; ;)
	{
		int rt = epoll_wait(epfd, event, EPOLLEVENT, -1);
		// process epoll_wait
		process_events(epfd, event, rt, listenfd, buf);
	}

	return;
}

int start_server(std::string ip, int port)
{
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
	{
		log();
		printf("socket failed. \n");
		return -1;
	}

	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	inet_pton(AF_INET, ip.c_str(), &serveraddr.sin_addr);

	//bind
	int rt = bind(server_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (rt < 0)
	{
		log();
		printf("bind failed \n");
		return -1;
	}
	// listen
	listen(server_sock, 5);

	start_epoll(server_sock);

	return 0;
}

int main(int argc, char **argv)
{
	start_server(IP, PORT);

	return 0;
}