#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#define SER_PORT 9527

int sys_err(const char* s)
{
    perror(s);
    exit(0);
}
int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_DGRAM,0);
    if(fd == -1)
    {
        sys_err("socket error");
    }
    struct sockaddr_in ser_addr;
    inet_pton(AF_INET,"127.0.0.1", &ser_addr.sin_addr.s_addr);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SER_PORT);
    
    char buf[BUFSIZ];

    while(1)    
    {
        fgets(buf, sizeof(buf), stdin);
        //printf("send %s \n", buf);
        //注意此处必须写实际发送的字节，否则会导致buf被填充，乱码
        sendto(fd, buf, strlen(buf), 0, (struct  socketaddr*)&ser_addr, sizeof(ser_addr));

        int ret = recvfrom(fd, buf, BUFSIZ, 0, NULL, 0);
        write(STDOUT_FILENO, buf, ret);
        //printf("write \n");
    }
    close(fd);
}