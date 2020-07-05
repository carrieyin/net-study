#include "wrapper.h"
#include <signal.h>

#define PORT 8888

void sig_child(int sig)
{
    while(waitpid(0, NULL, WNOHANG));
}

int main()
{ 
    int fd;
    fd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(PORT);
    seraddr.sin_len = sizeof(seraddr);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(fd, (const struct socketaddr *)(&seraddr), seraddr.sin_len);

    Listen(fd);

    while(1)
    {
        struct sockaddr_in cliaddr;
        int cfd = Accept(fd, &cliaddr, &cliaddr.sin_len);
        pid_t pid;
        pid = fork();
        if(pid > 0) //父进程
        {
            struct sigaction sig;
            sig.sa_handler = sig_child;
            sigemptyset(&sig.sa_mask);
            sig.sa_flags = 0;
            sigaction(SIGCHLD, &sig, NULL);
        }
        else if(pid == 0)
        {
            char buf[BUFSIZ];
            while(1)
            {
                int size = read(cfd, buf, sizeof(buf));
                if(size == 0)
                {
                    sys_err("read end");
                }

                write(cfd, buf, size);
                write(STDOUT_FILENO, buf, size);
            }
        }

    }

    return 0;
}