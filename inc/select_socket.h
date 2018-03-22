/********************************************************************
	created:	2018/03/22
	created:	22:3:2018   18:04
	filename: 	inc/select_socket.h
	file path:	inc
	file base:	select_socket
	file ext:	h
	author:		huanglb
	
	purpose:	
*********************************************************************/
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>

const int MAXLINE = 1024;

#define log(fmt,...) printf("[%s\%s:%d]: "##fmt, __FILE__,  __FUNCTION__, __LINE__, ##__VA_ARGS__)  