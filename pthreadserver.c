#include "wrapper.h"
#include <pthread.h>

#define PORT 6666

void * thread_work(void * arg)
{
    char buf[BUFSIZ];
    int cfd = *((int*) arg);
    while(1)
    {
        int size = read(cfd, buf, sizeof(buf));
        if(size == 0)
        {
            close(cfd); //注意此处需要及时close,否则客户端退出后,accept的时候报错
        }

        write(cfd, buf, size);
        write(STDOUT_FILENO, buf, size);
    }
}

int main()
{
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    fd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR ,&opt, sizeof(opt));
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

        pthread_t tid;
        pthread_create(&tid, NULL, thread_work, &cfd);
        pthread_detach(tid);
    }

    return 0;
}