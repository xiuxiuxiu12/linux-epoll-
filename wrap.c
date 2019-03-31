#include "wrap.h"


void perr_exit(const char* s)
{
    perror(s);
    exit(-1);
}

int Socket(int domain,int type,int protocol)
{
    int n;
    n = socket(domain,type,protocol);
    if(n < 0)
        perr_exit("socket error");
    return n;
}

int Bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen)
{
    int n;
    n = bind(sockfd,addr,addrlen);
    if(n < 0)
        perr_exit("bind error");
    return n;
}

int Listen(int sockfd, int backlog)
{
    int n;
    n = listen(sockfd,backlog);
    if(n < 0)
        perr_exit("listen error");
    return n;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int n;
again:
    n = accept(sockfd,addr,addrlen);
    if(n < 0)
        if(errno == ECONNABORTED || errno == EINTR)
            goto again;
        else
            perr_exit("accept error");
    return n;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    int n;
again:
    n = read(fd,buf,count);
    if(n < 0)
	//以非阻塞方式读，并且没有数据
        if(errno == EINTR || errno == EAGAIN)
            goto again;
        else
            return -1;
    return n;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    int n;
again:
    n = write(fd,buf,count);
    if(n < 0){
        if(errno == EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}

int Close(int fd)
{
    int n;
    n = close(fd);
    if(n < 0)
        perr_exit("close error");
    return n;
}

ssize_t Readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	size_t nread;
	char *ptr;

	nleft = n;
	ptr = (char *)vptr;

	while(nleft > 0){
        nread = read(fd,ptr,nleft);
        if(nread < 0){
            if(errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if(nread == 0)
            break;

        nleft -= nread;
        ptr += nread;
	}

	//返回已经读到的字节数
	return n-nleft;
}

ssize_t Writen(int fd, void* vptr, size_t n)
{
    size_t nleft;
    size_t nwrite;
    const char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while(nleft > 0){
        nwrite = write(fd,ptr,nleft);
        if(nwrite < 0){
            if(errno == EINTR)
                nwrite = 0;
        }

        nleft -= nwrite;
        ptr += nwrite;
    }
    return n;

}

//在文件缓冲区中想读取一行函数
//因为fgets函数无法使用在文件描述符中读取一行
static ssize_t my_read(int fd, char *ptr)
{
    static int read_cnt;
    static char *read_ptr;
    static char read_buf[100];

    //如果说还没有读取数据，则读100字节大小的数据
    //如果时候已经读取了数据，则每次取读取到的数据中的第一个字母
    if(read_cnt <= 0){
again:
        read_cnt = read(fd,read_buf,sizeof(read_buf));
        if(read_cnt < 0)
        {
            if(errno == EINTR)
                goto again;
            return -1;
        }
        else if(read_cnt == 0)
            return 0;
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}


ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
    //利用my_read读取数据，
    char c,*ptr;
    int rc, n;
    ptr = (char *)vptr;
    for(n = 1; n < maxlen; n++){
        rc = my_read(fd,&c);
        if(rc == 1){
            *ptr++ = c;
            if(c == '\n')
                break;
        }
        else if(rc == 0){
            *ptr = 0;
            return n-1;
        }
        else
            return -1;
    }
    *ptr = 0;
    return n;


}


