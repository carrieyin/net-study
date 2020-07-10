#include "../common/wrapper.h"
//mac下无epoll
#include <sys/epoll.h>

#define PORT 8888 
#define OPEN_MAX 1024
#define MAXLINE 1024
int main()
{
    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    //ser_addr.sin_len = sizeof(ser_addr);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(fd, (const struct socketaddr*)&ser_addr, sizeof(ser_addr));

    Listen(fd);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&cli_addr, 0, sizeof(cli_addr));
   
    int efd;
    efd = epoll_create(OPEN_MAX);
    if(efd == -1)
    {
	sys_err("epoll error");
    }

   struct epoll_event tep, ep[OPEN_MAX];
   tep.data.fd = fd;
   tep.events = EPOLLIN;
   ssize_t res = epoll_ctl(efd, EPOLL_CTL_ADD,fd, &tep);
   if(res == -1)
   {
       sys_err("epoll ctl error");
   }
   
   int nready = 0;
   while(1)
   {
       nready = epoll_wait(efd, ep, OPEN_MAX, -1);
       if(nready == -1)
       {
           sys_err("epoll wait error");
       }
       int i = 0;
       for(; i < nready; i++)
       {
           if(!(ep[i].events & EPOLLIN))
           {
               continue;
           }
       
           if(ep[i].data.fd == fd)
           {
               int connectfd = Accept(fd, (struct sockaddr*)&cli_addr, &cli_addr_len);
               tep.events = EPOLLIN;
               tep.data.fd = connectfd;
               res = epoll_ctl(efd, EPOLL_CTL_ADD, connectfd, &tep);
               if(res == -1)
               {
                   sys_err("epoll ctl error");
               }
           }else
           {
              int sofd = ep[i].data.fd;
              char buf[MAXLINE];
              int n = Read(sofd, buf, MAXLINE);
              if(n == 0)
              {
                  epoll_ctl(efd, EPOLL_CTL_DEL, sofd, NULL);
              }
              write(STDOUT_FILENO, buf,n);
           }


       } 
   }
} 
