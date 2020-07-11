#include "../common/wrapper.h"
//mac下无epoll
#include <sys/epoll.h>

#define PORT 8888 
#define OPEN_MAX 1024
#define MAXLINE 1024

int main()
{
    int efd;
    
   
  int nready = 0;

   int pfd[2];
   pipe(pfd);
   int pid = fork();
   if(pid == 0) //子进程
   {
       close(pfd[1]); //关闭写断
       int efd = epoll_create(OPEN_MAX);
        if(efd == -1)
        {
        sys_err("epoll error");
        }

        struct epoll_event tep;
        tep.data.fd = pfd[0];
        tep.events = EPOLLIN;
        ssize_t res = epoll_ctl(efd, EPOLL_CTL_ADD, pfd[0], &tep);
        if(res == -1)
        {
            sys_err("epoll ctl error");
        }

       struct epoll_event ep[OPEN_MAX];
       int nready = epoll_wait(efd, ep, OPEN_MAX, -1);
       if(nready > 0)
       {
           char buf[BUFSIZ];
           int nread = Read(pfd[0], buf, BUFSIZ);
           printf("read %d \n", nread);
           write(STDOUT_FILENO, buf, nread);

       }
   }
   else //父进程
   {
       close(pfd[0]);
       write(pfd[0], "aaa\n", 5);
   }
   
} 
