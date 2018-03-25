/********************************************************************
	created:	2018/03/25
	created:	25:3:2018   12:02
	filename: 	src/echo_tcp_client.cpp
	file path:	src
	file base:	echo_tcp_client
	file ext:	cpp
	author:		huanglb
	
	purpose:	
*********************************************************************/

#include "../inc/sw_tcp_socket_utils.h"
#include <string>

static int start_echo_client(std::string ip, int port)
{
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET; 
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), (void *)&serveraddr.sin_addr);

	int rt = connect(client_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (rt < 0)
	{
		log();
		printf("connect failed \n");
		return -1;
	}

	str_cli_echo(stdin, client_sock);

	close(client_sock);

	return 0;
}

int main(int argc, char **argv)
{
	int port = 8090;
	std::string ip("127.0.0.1");

	start_echo_client(ip, port);

	return 0;
}
