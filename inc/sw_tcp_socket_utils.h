/********************************************************************
	created:	2018/03/23
	created:	23:3:2018   11:58
	filename: 	inc/sw_tcp_socket.h
	file path:	
	file base:	sw_tcp_socket
	file ext:	h
	author:		huanglb
	
	purpose: socket utils 
*********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>  
#include <fcntl.h>
#include <errno.h>
#include <time.h>
//#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDR "/tmp/server.sock"
#define MAXLINE 1024


#define log(fmt,...) printf("[%s:%s:%d]: "##fmt, __FILE__,  __FUNCTION__, __LINE__, ##__VA_ARGS__)

int readn(int fd, void *pstr, int len);

int writen(int fd, const void *pstr, int len);


//************************************
// Method:    setnoblock
// FullName:  setnoblock
// Access:    public 
// Returns:   void
// Qualifier: set socket noblock.
// Parameter: int fd
//************************************
void setnoblock(int fd);


//************************************
// Method:    proccss_client
// FullName:  proccss_client
// Access:    public static 
// Returns:   void
// Qualifier: process client's msg.
// Parameter: int fd
//************************************
void proccss_client(int fd);


void str_echo(int fd);


void str_cli_echo(FILE *fp, int fd);

void str_cli_echo_select(FILE *fp, int sockfd);