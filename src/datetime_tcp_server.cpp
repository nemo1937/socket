
/********************************************************************
	created:	2018/03/24
	created:	24:3:2018   18:49
	filename: 	src/daytime_tcp_server.cpp
	file path:	src
	file base:	daytime_tcp_server
	file ext:	cpp
	author:		huanglb
	
	purpose:	a simple datetime tcp server
*********************************************************************/
#include "../inc/sw_tcp_socket_utils.h"
#include <ctime>

void start_server(int port)
{
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
	{
		log();
		printf("socket failed. \n");
		return;
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind
	int rt = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	if (rt < 0)
	{
		log();
		printf("bind failed. \n");
		return ;
	}

	rt = listen(server_sock, 5);
	if (rt < 0)
	{
		log();
		printf("listen failed. \n");
		return;
	}

	

	for (;;)
	{
		int client_fd = -1;
		client_fd = accept(server_sock, (struct sockaddr *)NULL, NULL);
		if (client_fd < 0 && EINTR == errno)
		{
			log();
			printf("accept failed. strerror: %s \n", strerror(errno));
			continue;
		}
		else if (client_fd < 0)
		{
			log();
			printf("accept failed. errno: %s \n", strerror(errno));
			continue;
		}
		else
		{
			std::time_t datetime = std::time(nullptr);
			char send_buf[256] = {0};
			char *ptime = NULL;
			ptime = std::asctime(std::localtime(&datetime));
			memcpy(send_buf, ptime, 25);

			printf("datetime: %s \n", send_buf);
			// send to client
			rt = writen(client_fd, (void *)send_buf, 25);
			if ( rt < 0 )
			{
				log();
				printf("write failed. \n");
			}
			else
			{
				printf("write success. \n");
			}

			close(client_fd);
		}// else	
	}// for

	// clear socket resource.
	close(server_sock);

	return;
}


int main(int argc, char **argv)
{
	int port = 9090;

	start_server(port);

	return 0;
}