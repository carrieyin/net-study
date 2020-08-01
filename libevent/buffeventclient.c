#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

void read_cb(struct bufferevent *bev, void *arg)
{
    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));
    printf("read client data: %s", buf);

}

void write_cb(struct bufferevent *bev, void *arg)
{
    printf("write ok \n");
}

void event_cb(struct bufferevent *bev, short events, void* arg)
{
    if(events & BEV_EVENT_EOF)
    {
        printf("connection closed \n");
    }
    else if(events & BEV_EVENT_ERROR)
    {
        printf("som other error \n");
    }
    else if(events & BEV_EVENT_CONNECTED)
    {
        printf("connect server \n");
        return ;
    }

    bufferevent_free(bev);
    
}

void read_terminal(evutil_socket_t fd, short what, void *arg)
{
    char buf[1024] = {0};
    int  len = read(fd, buf, sizeof(buf));

    struct bufferevent * bev = (struct bufferevent*)arg;
    bufferevent_write(bev, buf, len + 1);
}


int main()
{
    //创建base
    struct event_base *base = event_base_new();

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //创建bufevent
    struct bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  
    struct sockaddr_in seraddr;
    memset(&seraddr, 0, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9000);
    seraddr.sin_addr.s_addr =  htonl(INADDR_ANY);

    //连接服务器
    bufferevent_socket_connect(bev, (struct sockaddr*)&seraddr, sizeof(seraddr));

    //设置回调
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

    //设置读缓冲使能
    bufferevent_enable(bev, EV_READ);

    //创建事件，监听标准输入，从标准输入写入缓冲区
    struct event *ev = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, read_terminal, bev);

    //添加事件
    event_add(ev, NULL);

    //启动监听循环
    event_base_dispatch(base);

    //销毁
    event_free(ev);

    event_base_free(base);

    return 0;
}