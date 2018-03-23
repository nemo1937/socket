/********************************************************************
	created:	2018/03/23
	created:	23:3:2018   12:00
	filename: 	src/sw_tcp_socket_client.cpp
	file path:	
	file base:	sw_tcp_socket_client
	file ext:	cpp
	author:		huanglb
	
	purpose:	a simple sun_family is unix protcol socket client.
*********************************************************************/
#include "../inc/sw_tcp_socket_utils.h"


static void un_tcp_client()
{
	int client_fd =  socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		log();
		printf("socket failed. errno: %s", strerror(errno));
		exit(0);
	}

	struct sockaddr_un server_addr;
	memset((void *)&server_addr, 0x00 ,sizeof(struct sockaddr_un));
	server_addr.sun_family = AF_UNIX;
	//配置服务器信息(socket 对象)
	strncpy(server_addr.sun_path, SERVER_ADDR, strlen(SERVER_ADDR));  

	int rt = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
	if (rt < 0)
	{
		log();
		printf("connect failed. \n");
	}

	char msg[MAXLINE] = {"hello, world"};
	
	int nbytes = send(client_fd, msg, strlen(msg), 0);
	if (nbytes < 0)
	{
		log();
		printf("send failed. \n");
	}
	else
	{
		printf("send success. \n");
	}
	close(client_fd);

	return;
}

int main(int argc, char **argv)
{
	un_tcp_client();

	return 0;
}