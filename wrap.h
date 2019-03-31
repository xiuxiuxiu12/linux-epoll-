#ifndef WRAP_H_INCLUDED
#define WRAP_H_INCLUDED

#ifdef __cplusplus
extern "C"{

#endif


#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void perr_exit(const char* s);
int Socket(int domain,int type,int protocol);
int Bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
int Close(int fd);
ssize_t Readn(int fd, void *vptr, size_t n);
ssize_t Writen(int fd, void *vptr, size_t n);
ssize_t Readline(int fd, void *vptr, size_t maxlen);




#ifdef __cplusplus

};

#endif

#endif // WRAP_H_INCLUDED
