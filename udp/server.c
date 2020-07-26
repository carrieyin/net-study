#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#define SER_PORT 9527

int sys_err(const char* s)
{
    perror(s);
    exit(0);
}

int main()
{
    
    int ret;
    char buf[BUFSIZ];
    

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SER_PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd, (const struct socketaddr*)&ser_addr, sizeof(ser_addr));


    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t cli_addr_len = sizeof(cli_addr);
     
    while (1)
    {
        ret = recvfrom(fd, buf, BUFSIZ, 0, (struct sockaddr *)&cli_addr, &cli_addr_len);
        char cli_ip[30];
        const char *p = inet_ntop(AF_INET, &cli_addr, cli_ip, cli_addr_len);
        
        printf("cli ip: %s, port:%d \n", p, ntohs(cli_addr.sin_port));
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
        sendto(fd, buf, ret, 0, (struct sockaddr *)&cli_addr, cli_addr_len);
        
    }
    
    close(fd);
    
}
