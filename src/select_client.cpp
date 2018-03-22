/********************************************************************
	created:	2018/03/22
	created:	22:3:2018   18:03
	filename: 	src/select_client.cpp
	file path:	src
	file base:	select_client
	file ext:	cpp
	author:		huanglb
	
	purpose:	
*********************************************************************/
#include "select_socket.h"
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

void start(int fd);
void send_data(int fd);
void recv_data(int fd);

/*
 *	判断连接是否失效
 *  return : true 连接状态； false 失效状态
 */
bool IsConnect(int fd)
{
	struct tcp_info info;  
	int len=sizeof(info);  
	if (fd <= 0)
	{  
		return 0;  
	}  
	memset(&info, 0x00, sizeof(info));  
	getsockopt(fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);  
	if (info.tcpi_state == 1) 
	{  
		std::cout<< "socket connected" << std::endl;  
		return true; 
	} else 
	{  
		std::cout << "socket disconnected" << std::endl;  
		return false;  
	}  

	return true;
}

/*
 *	连接上服务器之后，开启发送数据，接收数据线程
 */
void start(int fd)
{
	std::cout << "connect server success." << std::endl;
	auto send_thread = std::thread(send_data, fd);
	auto recv_thread = std::thread(recv_data, fd);

	send_thread.join();
	recv_thread.join();

	return; 
}

void send_data(int fd)
{
	while(1)
	{
		if (!IsConnect(fd))
		{
			std::cout << "socket is unconnected. break." << std::endl;
			break;
		}
		std::string str_input("");
		std::cout <<"input data:" << std::endl;
		std::cin>>str_input;
		int rt = send(fd, str_input.c_str(), str_input.length(), false);
		if (rt < 0)
		{
			std::cout << "send failed.." << std::endl;
		}
		str_input.clear();
	}
	
	return;
}

void recv_data(int fd)
{
	
	while (1)
	{
		if (!IsConnect(fd))
		{
			std::cout << "socket is unconnected. break." << std::endl;
			break;
		}

		char buf[1024] = {0};
		std::cout << "recv from server.." << std::endl;
		
		if (recv(fd, buf, 1023, 0) <= 0)
		{
			std::cout << "recv data failed from server." << std::endl;
			break;
		}
		else
		{
			std::cout << "server said: " << buf << std::endl;
		}

	}

	return;
}

void client_run(std::string ip, int port)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(client_fd < 0)
	{
		std::cout << "create socket failed." << std::endl;
		std::cout << "error code: " << strerror(errno) << std::endl;
		
		exit(-1);
	}

	struct sockaddr_in server_addr;//即将连接的服务器地址信息
	memset((void *)&server_addr, 0 ,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);//指定端口
	inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);//指定IP

	int rt = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));//连接到服务端

	if (rt < 0)
	{
		std::cout << "connect server failed. error code: " << strerror(errno) << std::endl;
		exit(-1);
	}

	start(client_fd);

	return;
}

int main(int argc, char ** argv)
{
	std::cout <<"start client " << std::endl;
	client_run("127.0.0.1", 9090);

	return 0;
}