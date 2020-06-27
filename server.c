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
    int cfd = accept(fd, &cli_addr, &cli_addr_len);
    char cli_ip[30];
    const char *p = inet_ntop(AF_INET, &cli_addr, cli_ip, cli_addr_len);
    printf("cli ip: %s, port:%d, ip: %s", cli_ip, ntohs(cli_addr.sin_port), p);
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