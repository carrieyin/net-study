#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../common/wrapper.h"
//mac下无epoll
#include <sys/epoll.h>
#include <errno.h>


#define MAX_EVENTS 1024
#define BUFLEN 4096
#define SERV_PROT 8080
void recvdate(int fd, int events, void* arg);
void senddata(int fd, int events, void* arg);

struct myevents_s
{
    int fd;
    int events;
    void* arg;
    void (*call_back)(int fd, int events, void * arg);               
    int status;
    char buf[BUFLEN]; 
    int len;
    long last_active;
};

struct myevevts_s g_events[MAX_EVENTS + 1];

int g_efd;

int main()
{
    g_efd = epoll_create(MAX_EVENTS + 1);
    if(g_efd < 0)
    {
        printf("create efd in %s err %s \n", __func__, strerror(errno));
    }

    initlistensocket(g_efd, SERV_PROT);
    struct epoll_event events[MAX_EVENTS + 1] ;

    while(1)
    {
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if(nfd < 0)
        {
            printf("epoll wait error exit\n");
        }

        int i;
        for(i = 0; i < nfd; i++)
        {
            struct myevents *ev = (struct myevent_s *)events[i].data.ptr;
            if((events[i].events & EPOLLIN)) && (ev->events & EPOLLIN))
            {

            }
        }

    }
    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SERV_PROT);
    //ser_addr.sin_len = sizeof(ser_addr);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(fd, (const struct socketaddr*)&ser_addr, sizeof(ser_addr));

    Listen(fd);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&cli_addr, 0, sizeof(cli_addr));



}