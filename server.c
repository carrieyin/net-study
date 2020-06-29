#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include  <arpa/inet.h>

#define SER_PORT 9527
void sys_err(const char* str)
{
    perror(str);
    exit(1);
}


const char * getclientip(int sockconn , struct sockaddr_in sa, socklen_t len);

int main()
{
    int fd = 0;
    int ret;
    char buf[BUFSIZ];
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        sys_err("socket error");
    }

    struct sockaddr_in ser_addr, cli_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SER_PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd, (const struct sockaddr *)&ser_addr, sizeof(ser_addr));
    listen(fd, 128);
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&cli_addr, 0, sizeof(cli_addr));
    int cfd = accept(fd, &cli_addr, &cli_addr_len);
    char cli_ip[30];
    const char *p = inet_ntop(AF_INET, &cli_addr, cli_ip, cli_addr_len);
    printf("cli ip: %s, port:%d \n", p, ntohs(cli_addr.sin_port));
    //printf("cli ip:", inet_ntoa(&(cli_addr.sin_addr)));
    //下面的函数报错
    //getclientip(cfd, cli_addr, cli_addr_len);
    if(cfd == -1)
    {
        sys_err("accect error");
    }
    printf("ser accecped \n");
    while (1)
    {
       ret = read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);
        for(int i = 0; i < ret; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        //write(STDOUT_FILENO, buf, ret);
        
    }
    
    close(fd);
    close(cfd);
    
}

const char * getclientip(int sockconn , struct sockaddr_in sa, socklen_t len)
{
    if(!getpeername(sockconn, (struct sockaddr *)&sa, &len))
    {
        char sql[1024];
        memset(sql,0,1024);
        char  mechine_ip[30];
        char *p  = mechine_ip;
        memset(mechine_ip, 0, 30);
        snprintf(sql,1024,"client login. ip: %s, port :%d",inet_ntoa(sa.sin_addr),ntohs(sa.sin_port));
        snprintf(p,17,"%s",inet_ntoa(sa.sin_addr));

        printf("ip:--- %s", p);
    }
}
