/********************************************************************
	created:	2018/03/15
	created:	15:3:2018   15:52
	filename: 	sw_tcp_socket.cpp
	file path:	
	file base:	sw_tcp_socket
	file ext:	cpp
	author:		huanglb
	
	purpose:	1.a simple concurrent tcp server
				2. socket sun_family is unix
*********************************************************************/

#include "../inc/sw_tcp_socket_utils.h"

static void TCPServer()
{
	int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR ,&opt, sizeof(&opt));

	int flags = fcntl(server_sock, F_GETFL, 0);
	printf("flags: %d\n", flags & O_NONBLOCK);
	//fcntl(server_sock, F_SETFL, flags | O_NONBLOCK);

	struct sockaddr_un  server_addr;
	memset(&server_addr, 0x00, sizeof(server_addr));

	//删除原有server_socket对象
	unlink (SERVER_ADDR); 

	server_addr.sun_family = AF_UNIX;
	//server_addr.sin_addr.s_addr = inet_addr(ip);
	/*配置服务器信息(socket 对象)*/  
	strncpy(server_addr.sun_path, SERVER_ADDR, strlen(SERVER_ADDR));  
	
	//bind
	bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// set recv and send out time
	struct timeval timeout = {5, 0};
	int rt = setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
	if (rt < 0)
	{
		printf("setsockopt1 falied, %s", strerror(errno));
	}

	//struct timeval timeout = {5, 0};
	rt = setsockopt(server_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
	if (rt < 0)
	{
		printf("setsockopt2 falied, %s", strerror(errno));
	}

	//参数2：listen的连接总数 = 已通过的三次握手的连接 + 正在进行握手的连接
	listen(server_sock, 5);

	int client_sock;
	struct sockaddr_un client_addr;
	memset(&client_addr, 0x00, sizeof(client_addr));
	socklen_t len = sizeof(client_addr);

	pid_t child_pid = 0;
	
	while (1)
	{
		printf("server wait..\n");
		//accept
		client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len);
		if (client_sock < 0)
		{
			printf("accept error, %s. \n", strerror(errno));
			// if errno is system interupt signal or noblock.
			if (errno == EAGAIN || errno == EINTR)
			{
				continue;
			}
			
			printf("accept failed.\n");
			sleep(10);
			continue;
		}
		else // accept success
		{
			if (0 == (child_pid = fork())) // child proccess
			{
				close(server_sock);
				proccss_client(client_sock);
				exit(0);
			}
			else // parent close connected socket
			{
				close(client_sock);
			}
		}
		//sleep(10);
	}

	unlink(SERVER_ADDR);
	close(client_sock);
	
	return;
}

int main(int argc, char **argv)
{
	TCPServer();

	return 0;
}