/********************************************************************
	created:	2018/03/21
	created:	21:3:2018   13:29
	filename: 	src/select_server.cpp
	file path:	src
	file base:	select_server
	file ext:	cpp
	author:		huanglb
	
	purpose:	select tcp server.
*********************************************************************/

#include "select_socket.h"
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <vector>
#include <errno.h>
#include <time.h>

void start_server(int port);

class client
{
public:
	client(int fd, struct sockaddr_in client_addr);

public:
	int client_fd;
	struct sockaddr_in client_addr;
};

client::client(int fd, struct sockaddr_in addr)
{
	client_fd = fd;
	memcpy((void *)&client_addr, (void *)&addr, sizeof(struct sockaddr_in));
}

// golobal vector
std::vector<client> vec_client;
int g_clients = 0;

/*
 *	start server
 */
void start_server(int port)
{
	// create socket
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
	{
		std::cout << "server socket failed. " << std::endl;
		exit(-1);
	}
	struct sockaddr_in server_addr;
	bzero((void *)&server_addr, sizeof(struct sockaddr_in));

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	
	// bind
	int rt = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	if (rt < 0)
	{
		std::cout << "server socket bind failed." << std::endl;
		exit(-1);
	}

	// set recv and send time
// 	struct timeval timeout = {5, 0};
// 	rt = setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
// 	if (rt < 0)
// 	{
// 		printf("setsockopt1 falied, %s", strerror(errno));
// 	}
// 
// 	//struct timeval timeout = {5, 0};
// 	rt = setsockopt(server_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
// 	if (rt < 0)
// 	{
// 		printf("setsockopt2 falied, %s", strerror(errno));
// 	}


	//listen
	rt = listen(server_sock, 5);
	if (rt < 0)
	{
		std::cout << "server socket listen failed. " << strerror(errno) << std::endl; 
	}


	fd_set rset;

	int connect_client = 0;
	char buff[1024] = {0};

	FD_ZERO(&rset);
	FD_SET(server_sock, &rset);

	while (1)
	{
		fd_set tmp_rset = rset;
		// select
		if (select(FD_SETSIZE, &tmp_rset, NULL, NULL, NULL) < 0)
		{
			std::cout <<"server select failed. " << strerror(errno) << std::endl;
		//	break;
		}
		else
		{
			std::cout << "select success.." << std::endl;
		}
		

		// 若有监听端口可读，说明有新的连接进来
		if (FD_ISSET(server_sock, &tmp_rset))
		{
			struct sockaddr_in sock_client;
			bzero((void *)&sock_client, sizeof(struct sockaddr_in));

			std::cout <<"new client connect." <<std::endl;

			socklen_t sock_client_len = sizeof(sock_client);

			connect_client = accept(server_sock, (struct sockaddr *)&sock_client, &sock_client_len);
			if (connect_client < 0)
			{
				std::cout << "server accept failed. " << strerror(errno) << std::endl;
			}
			g_clients++;

			// add to rset
			FD_SET(connect_client, &rset);
			vec_client.push_back(client(connect_client, sock_client));
		}

		// proccess clients
		for (auto iter = vec_client.begin(); iter != vec_client.end(); iter++)
		{
			// 若客户端的描述符可读
			FD_ISSET(iter->client_fd, &tmp_rset);

			char buf[256] = {0};
			int nbyts = 0;
			nbyts = recv(iter->client_fd, buff, 1024, 0);
			if (nbyts == 0)
			{
				log();
				std::cout << "clients: " << (char*)inet_ntop(AF_INET, (void *)&(iter->client_addr.sin_addr),\
					buf, sizeof(buf)) << " : " << ntohs(iter->client_addr.sin_port) << " disconnect." << std::endl;
				
				FD_CLR(iter->client_fd, &rset);
				vec_client.erase(iter);

				break;
			}
			else if (nbyts < 0)
			{
				// recving receve a system interupt
				if (errno == EINTR)
				{
					continue;
				}
				else
				{
					log();
					std::cout << "clients: " << (char*)inet_ntop(AF_INET, (void *)&(iter->client_addr.sin_addr),\
						buf, sizeof(buf)) << " : " << ntohs(iter->client_addr.sin_port) << " disconnect." << std::endl;

					FD_CLR(iter->client_fd, &rset);
					vec_client.erase(iter);
					
					continue;
				}
			}
			else
			{
				log();
				std::cout << "client: " << (char*)inet_ntop(AF_INET, (void *)&(iter->client_addr.sin_addr), buf, sizeof(buf)) \
					<< " : " << ntohs(iter->client_addr.sin_port) << " send msg: "<< buff <<std::endl;
			} // else
		} // for
	} // while

	return;
}

int main(int argc, char **argv)
{
	const int port = 9090;

	//start server
	start_server(port);

	return 0;
}