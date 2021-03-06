#include "../inc/sw_tcp_socket_utils.h"

//************************************
// Method:    readn
// FullName:  readn
// Access:    public static 
// Returns:   ssize_t
// Qualifier:
// Parameter: int fd
// Parameter: void * pstr
// Parameter: ssize_t len
//************************************
int readn(int fd, void *pstr, int len)
{
	int nleft = len;
	int nread = 0;

	char *pread = (char *)pstr;
	
	for (; ;)
	{
		nread = read(fd, pread, nleft);
		if (nread < 0)
		{
			if (errno == EAGAIN)
			{
				log();
				printf("errno == again..\n");
				continue;
			}
			else //disconnect
			{
				//return -1;
				break;
			}
		}
		else if (0 == nread) // EOF
		{
			break;
		}
		else // success.
		{
			nleft = nleft - nread;
			pread += nread;
			nread = 0;
		}
	}// for
	
	return (len-nleft);
}

//************************************
// Method:    writen
// FullName:  writen
// Access:    public static 
// Returns:   int
// Qualifier:
// Parameter: int fd
// Parameter: const void * pstr
// Parameter: int len
//************************************
int writen(int fd, const void *pstr, int len)
{
	int nleft = len;
	int nwrite = 0;
	const char *pwrite = (const char *)pstr;

	for (; ;)
	{
		nwrite = write(fd, pwrite, nleft);
		if (nwrite < 0 && errno == EINTR)
		{
			log();
			printf("strerrno: %s \n", strerror(errno));
			continue;
		}
		else if (nwrite <= 0) // failed.
		{
			log();
			printf("write error..%s \n", strerror(errno));
			return -1;
		}
		else
		{
			nleft -= nwrite;
			pwrite += nwrite;
			if (nleft == 0)
			{
				log();
				printf("finish send msg..\n");
				break;
			}
		}
	}

	return len;
}


//************************************
// Method:    setnoblock
// FullName:  setnoblock
// Access:    public 
// Returns:   void
// Qualifier: set socket noblock.
// Parameter: int fd
//************************************
void setnoblock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	//printf("flags: %d\n", flags & O_NONBLOCK);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	return;
}

//************************************
// Method:    proccss_client
// FullName:  proccss_client
// Access:    public static 
// Returns:   void
// Qualifier: process client's msg.
// Parameter: int fd
//************************************
void proccss_client(int fd)
{
	char buf[MAXLINE] = {0};
	int nbyte = 0;

again:
	for (; ;)
	{
		nbyte = read(fd, buf, MAXLINE);
		if (nbyte <0 && errno == EINTR)
		{
			printf("a system interupt. errno: %s.\n", strerror(errno));
			goto again;
		}
		else if (nbyte <= 0)
		{
			log();
			printf("read failed. socket disconnect..\n");
			close(fd);
			break;
		}
		else
		{
			printf("client: %s. \n", buf);
		}
	}

	return;
}

void str_echo(int fd)
{
	// readn
	char buf[MAXLINE] = {0};
	int nbytes = 0;
again:
	while ((nbytes = read(fd, buf, MAXLINE)) > 0)
	{
		log();
		printf("buf: %s \n", buf);
		writen(fd, buf, strlen(buf));
		memset(buf, 0x00, sizeof(buf));
	}
	//int nbytes = read(fd, buf, MAXLINE);
	if (nbytes < 0 && errno == EINTR)
	{
		goto again;
	}
	else if (nbytes < 0)
	{
		log();
		printf("read failed...");
	}

	return;
}

void str_cli_echo(FILE *fp, int fd)
{
	char sendbuf[MAXLINE] = {0};
	char readbuf[MAXLINE] = {0};

	while(fgets(sendbuf, MAXLINE, fp) != NULL)
	{
		// writen
		writen(fd, sendbuf, strlen(sendbuf));

		// recv from server.
		if (read(fd, readbuf, MAXLINE) > 0)
		{
			fputs(readbuf, stdout);
		}
		else
		{
			log();
			printf("readn failed. \n");
			break;
		}
		memset(sendbuf, 0x00, sizeof(sendbuf));
		memset(readbuf, 0x00, sizeof(readbuf));
	}

	return;
}

void str_cli_echo_select(FILE *fp, int sockfd)
{
//	int 


	return;
}