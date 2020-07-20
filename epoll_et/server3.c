#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../common/wrapper.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

//mac下无epoll
//该实现细节存在问题待修复，比如send获取的buf，由于之前eventset 导致buf是空
#define MAX_EVENTS 1024
#define BUFLEN 4096
#define SERV_PROT 8080
void recvdate(int fd, int events, void* arg);
void senddata(int fd, int events, void* arg);
void initlistensocket(int efd, int port);

struct myevent_s
{
    int fd;
    int events;
    void* arg;
    void (*call_back)(int fd, int events, void * arg);    /* 回调函数 */            
    int status;
    char buf[BUFLEN]; 
    int len;
    long last_active;
};

struct myevent_s g_events[MAX_EVENTS + 1];

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
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
            if((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
            {
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }

             if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            {
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
        }

        
    }

    return 0;
}
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void * arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    memset(ev->buf, 0, sizeof(ev->buf));
    ev->len = 0;
    ev->last_active = time(NULL);
}

void eventadd(int efd, int events, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if(ev->status == 0)
    {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if(epoll_ctl(efd, op, ev->fd, &epv) < 0)
    {
        printf("event add failed fd:%d, events:%d \n", ev->fd, events);
    }
    else
    {
        /* code */
        printf("event add ok fd:%d, op:%d, events:%d \n", ev->fd, op, events);
    }

    return;
    
}
void eventdel(int efd, struct myevent_s *ev)
{
    struct epoll_event epv = {0,{0}};
    if(ev->status != 1)
    {
        return;
    }
    
    epv.data.ptr = NULL;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
    return;
    
}


void recvdata(int fd, int events, void* arg)
{
    struct myevent_s *ev = (struct myevent_s*)arg;
    int len;
    len = recv(fd, ev->buf, sizeof(ev->buf), 0);
    eventdel(g_efd, ev);
    if(len > 0)
    {
        ev->len = len;
        ev->buf[len] = '\0';
        printf("[%d] rev:%s \n", fd,ev->buf);

        eventset(ev, fd, senddata, ev);
        eventadd(g_efd, EPOLLOUT, ev);
    }else if(len == 0)
    {
        //
        close(ev->fd);
    }
    else
    {
        /* code */
        close(ev->fd);
    }
    
}

void senddata(int fd, int events, void* arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;
    //此处有问题
    len = send(fd, ev->buf, ev->len, 0);
    eventdel(g_efd, ev);
    if(len > 0)
    {
        printf("[%d] send, %d,%s \n", fd, len, ev->buf);
        eventset(ev, fd, recvdata, ev);
        eventadd(g_efd, EPOLLIN, ev);
    }
    else
    {
        close(ev->fd);
        
    }

    return;
    
    
}

void acceptconn(int lfd, int events, void *arg)
{
    struct sockaddr_in cin;
    socklen_t len ;
    len =  sizeof(cin);
    int cfd,i;

    if((cfd = accept(lfd, (struct sockaddr *)&cin, &len)) == -1)
    {
        if((errno != EAGAIN) && (errno != EINTR))
        {
            //暂时不做处理
        }

        printf("%s:accecpt:%s \n", __func__, strerror(errno));
    }

    do
    {
        /* code */
        for(i = 0; i < MAX_EVENTS; i++)
        {
            if(g_events[i].status == 0)   //从全局的g_events寻找一个开空闲元素
            {
                break;
            }
        }

        if(i == MAX_EVENTS)
        {
            printf("%s max connect limit[%d]\n", __func__, MAX_EVENTS);
            break;
        }

        int flag = 0;
        if((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0)
        {
            printf("%s: fcntl nonblocking failed , %s\n", __func__, strerror(errno));
            break;
        }

        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);

    } while (0);
    
}

void initlistensocket(int efd, int port)
{
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);                       //设置为非阻塞
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SERV_PROT);
    //ser_addr.sin_len = sizeof(ser_addr);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(fd, (const struct sockaddr*)&ser_addr, sizeof(ser_addr));

    Listen(fd);

    eventset(&g_events[MAX_EVENTS], fd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);
}
         
    


