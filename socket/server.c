#include "wrapper.h"
#define SER_PORT 9527

int main()
{
    
    int ret;
    char buf[BUFSIZ];
    

    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SER_PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t len = sizeof(ser_addr);
    Bind(fd, (struct sockaddr*)&ser_addr, len);

    Listen(fd);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&cli_addr, 0, sizeof(cli_addr));
    int cfd = Accept(fd, (struct sockaddr*)&(cli_addr), &cli_addr_len);

    char cli_ip[30];
    const char *p = inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, cli_addr_len);
    printf("cli ip: %s, port:%d \n", p, ntohs(cli_addr.sin_port));
    printf("cli ip: %s", inet_ntoa(cli_addr.sin_addr));
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
        if(ret < 0)
        {
            sys_err("ser read error");
        }
        //printf("start read %d bytes\n", ret);
        write(STDOUT_FILENO, buf, ret);
        for(int i = 0; i < ret; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        printf("end read %d bytes\n", ret);
        sleep(1);
        
    }
    
    close(fd);
    close(cfd);
    
}
