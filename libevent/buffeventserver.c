#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <arpa/inet.h>

void read_cb(struct bufferevent *bev, void *arg)
{
    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));
    printf("read client data: %s", buf);

    bufferevent_write(bev, "I have got data\n", 16);
}

void write_cb(struct bufferevent *bev, void *arg)
{
    printf("write ok \n");
}

void event_cb(struct bufferevent *bev, short events, void* arg)
{
    struct sockaddr_in * cliaddr = (struct sockaddr_in *) arg;
    if(events | BEV_EVENT_CONNECTED)
    {
        /*char ip[30];
        inet_ntop(AF_INET, &cliaddr->sin_addr.s_addr, ip, sizeof(*cliaddr));
        printf("client connected ip: %s, port: %d\n", ip, ntohs(cliaddr->sin_port));*/
    }
}

//listen回调
//typedef void (*evconnlistener_cb)(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);
//被回调说明客户端成功连接， cfd已经传入参数内部，创建bufferevent事件
//与客户端完成读写操作
void listener_cb(struct evconnlistener * liser, evutil_socket_t fd, struct sockaddr * caddr, int socklen, void * ptr)
{
    struct event_base *base = (struct event_base*)ptr;

    //创建bufevent事件
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    //设置回调函数
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, caddr);
    char ip[30];
    struct sockaddr_in * paddr = (struct sockaddr_in * )caddr;
    inet_ntop(AF_INET, paddr, ip, sizeof(*paddr));
    printf("client connected ip: %s, port: %d\n", ip, ntohs(paddr->sin_port));

    //启动read缓冲区 使能状态，默认禁用
    bufferevent_enable(bev, EV_READ);
}

int main()
{
    struct sockaddr_in seraddr;
    memset(&seraddr, 0, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9000);
    seraddr.sin_addr.s_addr =  htonl(INADDR_ANY);

    //创建base
    struct event_base *base = event_base_new();

    //创建服务器监听器,相当于socket bind listen accept
    //base:event base listener_cb:监听回调 base:传给回调一个参数 LEV|LEV 关闭时释放资源且监听允许端口复用 -1：监听数目不限制  seraddr：服务器地址  
    struct evconnlistener *listener = NULL;
    listener = evconnlistener_new_bind(base, listener_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (struct sockaddr *)&seraddr, sizeof(seraddr));

    //启动监听循环
    event_base_dispatch(base);

    //销毁event_base
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}