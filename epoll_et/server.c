#include "../common/wrapper.h"

//mac下无epoll，仅演示epoll可以用于监听普通文件描述符，实际读写pipe不需要
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
        tep.events = EPOLLIN | EPOLLET;
        //tep.events = EPOLLIN;
        ssize_t res = epoll_ctl(efd, EPOLL_CTL_ADD, pfd[0], &tep);

        if(res == -1)
        {
            sys_err("epoll ctl error");
        }

       struct epoll_event ep[OPEN_MAX];
       while (1)
       { 
            int nready = epoll_wait(efd, ep, OPEN_MAX, -1);
            //printf("ready %d \n", nready);
            if(nready > 0)
            {
                char buf[BUFSIZ];
                int nread = Read(pfd[0], buf, 5);
                //printf("read %d \n", nread);
                write(STDOUT_FILENO, buf, nread);
            }
       }        
   }
   else //父进程
   {

       close(pfd[0]);
       while (1)
       {
           int n = write(pfd[1], "aaaa\nbbbb\n", 10);
           //printf("write %d \n",n);
           sleep(3);
       }
   }
   
} 
