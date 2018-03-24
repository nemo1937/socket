/********************************************************************
	created:	2018/03/24
	created:	24:3:2018   17:32
	filename: 	src\daytime_tcp_client.cpp
	file path:	src
	file base:	daytime_tcp_client
	file ext:	cpp
	author:		huanglb
	
	purpose:	a simple datetime tcp client
*********************************************************************/
#include "../inc/sw_tcp_socket_utils.h"
#include <string>
#define  PORT 9090

void start_client(std::string serverip, int serverport)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		log();
		printf("socket failed. \n");
		exit(0);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverport);
	int rt = inet_pton(AF_INET, serverip.c_str(), &server_addr.sin_addr);
	if (rt != 1)
	{
		log();
		close(client_fd);
		printf("inet_pton failed. errno:  %s\n", strerror(errno));
		exit(0);
	}

	rt = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	if (rt < 0)
	{
		log();
		close(client_fd);
		printf("connect failed. \n");
		exit(0);
	}
	
	// get time from server.
	int nbytes = 0;
	char buf[MAXLINE] = {0};
	char *pbuf = buf;
	int ibuf_len = 0;
/*
	for (;;)
	{
		char readbuf[64] = {0};
		memset(readbuf, 0x00, sizeof(readbuf));
		nbytes = read(client_fd, readbuf, 64);
		
		if (nbytes < 0 && EINTR == errno)
		{
			log();
			printf("recv a system interupt, read again. \n");
			continue;
		}
		else if (nbytes < 0)
		{
			log();
			printf("read failed strerrno: %s.", strerror(errno));
			break;
		}
		else if (nbytes == 0)
		{
			log();
			printf("end of data \n");
			break;
		}
		else // nbytes > 0
		{
			strncpy(pbuf, readbuf, nbytes);
			pbuf += nbytes;
			ibuf_len +=nbytes;
			if (ibuf_len >= MAXLINE)
			{
				log();
				break;
			}
		}
	}
*/
	rt = readn(client_fd, buf, MAXLINE);
	if (rt > 0)
	{
		log();
		printf("datetime: %s \n", buf);
	}

	// print datetime
	//printf("datetime: %s \n", buf);
	// clear socket source.
	close(client_fd);

	return;
}


int main(int argc, char **argv)
{
	std::string server_ip("127.0.0.1");
	int port = 9090;

	start_client(server_ip, port);

	return 0;
}