/********************************************************************
	created:	2018/03/25
	created:	25:3:2018   11:21
	filename: 	src/echo_tcp_server.cpp
	file path:	src
	file base:	echo_tcp_server
	file ext:	cpp
	author:		huanglb
	
	purpose:	a simple echo tcp server.
*********************************************************************/
#include "../inc/sw_tcp_socket_utils.h"
#include <signal.h>
#include <sys/wait.h>

void sig_chld(int signal)
{
	pid_t pid;
	int stat = 0;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("child %d terminated. \n", pid);
	}

	return;
}


static void start_echo_tcp_server(int port)
{
	pid_t child_pid = 0;

	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
	{
		log();
		printf("socket failed. \n");
		return;
	}

	//server socket ref
	struct sockaddr_in serveraddr, clientaddr;
	bzero(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int rt = bind(server_sock, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in));
	if (rt < 0)
	{
		log();
		printf("bind failed. \n");
		return;
	}

	rt = listen(server_sock, 5);

	signal(SIGCHLD, sig_chld);

	for (; ;)
	{
		int client_sock = -1;
		int client_len = sizeof(clientaddr);
		client_sock = accept(server_sock, (struct sockaddr *)&clientaddr, (socklen_t *)&client_len);
		if (client_sock < 0 && errno == EINTR)
		{
			log();
			printf("recv eintr \n");
			continue;
		}
		else if (client_sock < 0)
		{
			log();
			printf("accept failed. strerror: %s\n", strerror(errno));
			break;
		}
		
		if ((child_pid = fork()) == 0) // child
		{
			close(server_sock);
			//printf("")
			log();
			str_echo(client_sock);
			exit(0);
		}
		else // parent
		{
			close(client_sock);
		}
	}

	close(server_sock);

	return;
}

int main(int argc, char **argv)
{
	int port = 8090;

	start_echo_tcp_server(port);


	return 0;
}