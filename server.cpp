#include <iostream>
#include <vector>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include  "wrap.h"
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>


#define SERV_PORT 6666

typedef std::vector<struct pollfd> PollFdList;
int main()
{

    //创建服务器的套接字
    int listenfd;
    listenfd = Socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,0);

    sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    //设置服务器的套接字支持端口复用
    int opt = 1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    //设置监听的最大上限数
    Listen(listenfd,SOMAXCONN);

    //将服务器的套接字设置为poll监听
    struct pollfd pfd;
    pfd.fd = listenfd;
    pfd.revents = POLLIN;

    PollFdList pollfds;
    pollfds.push_back(pfd);

    //创建一个客户端的套接字
    int connfd;
    struct sockaddr_in clien_addr;

    //定义一个变量，用来接受poll的返回值
    int nready;

    socklen_t clien_addr_len;

    //进入死循环
    while(1){

        //如果poll监听有事件发生，则返回nready
        nready = poll(&*pollfds.begin(),pollfds.size(),-1);
        if(nready == -1){
            perr_exit("poll 监听失败");
        }
        if(nready == 0){
            continue;
        }

        //判断poll监听到的套接字是不是listenfd
        if(pollfds[0].revents & POLLIN){
            clien_addr_len = sizeof(clien_addr);

             //如果是，利用accept函数，接受该套接字

            connfd = Accept(listenfd,(struct sockaddr*)&clien_addr,&clien_addr_len);
            //打印与服务器建立链接的客户端的ip和端口号
            char client_ip[1024] = {0};
            printf("client ip:%s, client port:%d\n",
                   inet_ntop(AF_INET,&clien_addr.sin_addr.s_addr,client_ip,sizeof(client_ip)),
                   ntohs(clien_addr.sin_port));

            //将该客户单的套接字设置到poll监听中

            fcntl(connfd,O_NONBLOCK|O_CLOEXEC);

            pfd.fd = connfd;
            pfd.events = POLLIN;
            pollfds.push_back(pfd);
            nready--;
            if(nready == 0){
                continue;
            }

        }

        //如果监听到的套接字是客户端的

        for(PollFdList::iterator it = pollfds.begin()+1;
            it != pollfds.end() && nready > 0; it++)
        {
            if(it->revents & POLLIN){
                //读取该套接字中的内容
                connfd = it->fd;
                char buf[1024] = {0};
                int len = Read(connfd,buf,sizeof(buf));
                if(len == 0){
                    std::cout<<"client close"<<std::endl;
                    pollfds.erase(it);

                    --it;
                    close(connfd);
                    continue;
                }

             //并将该内容做一些处理之后，写回到客户端的套接字中

                for(int i = 0; i < len; i++){
                    buf[i] = toupper(buf[i]);
                }
                Write(connfd,buf,len);
            }

        }

    }



    return 0;
}


