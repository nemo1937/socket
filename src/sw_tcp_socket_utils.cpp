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
				return -1;
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