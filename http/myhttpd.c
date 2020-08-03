//运用EPOLL ET写一个http 服务器

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define MAXSIZE 2048

int init_listen_fd(int port, int efd)
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket error");
        exit(1);
    }

    struct sockaddr_in seraddr;
    bzero(&seraddr, sizeof(bzero));
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(port);
    printf("listen port: %d \n", port);

    //端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int ret = bind(lfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if(ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen error");
        exit(1);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &ev);
    if(ret == -1)
    {
        perror("epoll ctl error");
        exit(1);
    }
    printf("add lfd over \n");
    return lfd;
}

//获取http中以\r\n结尾的数据
int get_line(int fd, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(fd, &c, 1, 0);
        if(n > 0)
        {
            if(c == '\r')
            {
                //拷贝读一次
                n = recv(fd, &c, 1, MSG_PEEK);
                if((n > 0) && (c == '\n'))
                {
                    recv(fd, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
                
            }
            buf[i] = c;
            i++;
        }
        else
        {
            c = '\n';
        }
    }
    buf[i] = '\0';
    if( -1 == n)
    {
        i = n;
    }

    return i;
    
}

void closefd(int cfd, int efd)
{
    //取消监听
    int ret = epoll_ctl(efd, EPOLL_CTL_DEL, cfd, NULL);
    if(ret == -1)
    {
        perror("epoll del error");
        exit(1);
    }
    close(cfd);
}

void sendResponse(int cfd, int no, char *disp, char *type,int len)
{
    char buf[1024] = {0};
    sprintf(buf, "HTTP/1.1 %d %s \r\n", no, disp);

    sprintf(buf, "Content-Type: %s \r\n", type);
    sprintf(buf + strlen(buf), "Content-Length:%d \r\n", len);
    sprintf(buf + strlen(buf), "\r\n");
    send(cfd, buf, strlen(buf), 0);
}
//发送文件内容给浏览
void sendfile(int cfd, char* file)
{
    //本地文件描述符
    int fd = open(file, O_RDONLY);
    if(fd == -1)
    {
        perror("open error");
        exit(1);
    }
    int n;
    char buf[1024];
    while ((n = read(fd, buf, sizeof(buf))) > 0)
    {
        /* code */
        int ret = send(cfd, buf, n, 0);
        //此处务必判定，否则会影响大文件的传输
        if (ret == -1) { 
			printf("errno = %d\n", errno);
			if (errno == EAGAIN) {
				printf("-----------------EAGAIN\n");
				continue;
			} else if(errno == EINTR) {
				printf("-----------------EINTR\n");
				continue;
			} else { 
				perror("send error");	
				exit(1);
			}	
		}
    }
    
    close(fd);
}

//判定文件类型
void getfileType(char * filename, char name[256], char extend[10], char type[30])
{
    printf("file name: %s \n", filename);
    sscanf(filename, "%[^.].%s", name, extend);
    printf("name:%s, extend:%s \n", name, extend);
    
    if(strncasecmp(extend, "jpg", 3) == 0)
    {
        strcpy(type, "image/jpeg");
    }
    else
    {
        strcpy(type, "text/plain; charset=iso-8859-1");
    }
    
}
//处理http请求，判定文件是否存在，回发文件内容
void http_request(int cfd, char * file)
{
    struct stat sbuf;
    //判断文件是否存在
    int ret = stat(file, &sbuf);
    if(ret == -1)
    {
        perror("stat error");
        //此处可以选择别的处理，比如回发浏览器404页面
        exit(1);
    }

    //这是一个普通文件
    if(S_ISREG(sbuf.st_mode))
    {
        //回发http协议应答
        /*
        HTTP/1.1 200 Ok
        Content-Type: text/plain; charset=iso-8859-1
        */
        int resstatus = 200;
        char* desp = "OK";
        char name[256];
        char extend[10];
        char type[30];
        getfileType(file, name, extend, type);
        //char* type = "Content-Type: text/plain; charset=iso-8859-1";  

        int len = sbuf.st_size;
        sendResponse(cfd, resstatus, desp, type, len);
        
        //回发数据
        sendfile(cfd, file);
    }
}

void do_read(int cfd, int efd)
{
    //读取一行http协议报文，拆分获取文件名
    char line[1024];
    int len = get_line(cfd, line, sizeof(line));
    if(len == 0)
    {
        printf("客户端关闭\n");
        closefd(cfd, efd);
    }else
    {
        //利用正则匹配 GET /hello.c HTTP/1.1
        char method[16], path[256], protocol[16];
        sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
        printf("method= %s, path：%s, protocol:%s \n", method, path, protocol);
        //读出缓冲区其他的数据，防止下次再有数据重复读
        //注意HTTP协议以\r\n换行，但是最后有一个\n
        while (1)
        {
            char buf[1024] ={0};
            len = get_line(cfd, buf, sizeof(buf));
            if(buf[0] == '\n'  || -1 == len)
            {
                printf("%s", buf);
                break;
            }
        }

        //忽略大小比较方法
        if(strncasecmp(method, "GET", 3) == 0)
        {
            char *file = path + 1;
            http_request(cfd, file);
        }
    }
}

void do_accept(int lfd, int efd)
{
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    int cfd = accept(lfd, (struct sockaddr *)&cli_addr, &len);
    if(cfd == -1)
    {
        perror("accept error");
        exit(1);
    }
    printf("connected \n");

    char client_ip[64] = {0};
    printf("new client ip: %s, port:%d \n", inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
    ntohs(cli_addr.sin_port));

    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;

    int ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev);
    if(ret == -1)
    {
        perror("epoll ctl error");
        exit(1);
    }

}

void epoll_run(int port)
{
    struct epoll_event allevents[MAXSIZE];

    //建立一个epoll监听树
    int efd = epoll_create(MAXSIZE);
    if(efd == -1)
    {
        perror("epoll create error");
        exit(1);
    }

    int lfd = init_listen_fd(port, efd);
    printf("listen ok, lfd: %d \n", lfd);
    while (1)
    {
        /* code */
        int ret = epoll_wait(efd, allevents, MAXSIZE, -1);
        if(ret == -1)
        {
            perror("epoll wait error");
            exit(1);
        }
        printf("event occured num:%d \n", ret);
        for(int i = 0; i < ret; i++)
        {
            struct epoll_event epollev = allevents[i];
            if(!(epollev.events & EPOLLIN))
            {
                continue;
            }
            
            if(epollev.data.fd == lfd)
            {
                printf("listen occured : %d \n", lfd);
                do_accept(lfd, efd);
            }
            else
            {
                printf("read occured %d \n", epollev.data.fd);
                do_read(epollev.data.fd, efd);
            }
        }
    }
    
}

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("please input ./server port path \n");
    }

    int port = atoi(argv[1]);

    //改变进程工作目录为请求中输入目录，方便查询
    int ret = chdir(argv[2]);
    if (ret == -1)
    {
        /* code */
        perror("chdir error");
        exit(1);
    }
    
    epoll_run(port);
}