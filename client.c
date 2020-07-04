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
    fd = socket(AF_INET, SOCK_STREAM,0);
    if(fd == -1)
    {
        sys_err("socket error");
    }
    struct sockaddr_in ser_addr;
    inet_pton(AF_INET,"192.168.199.217", &ser_addr.sin_addr.s_addr);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SER_PORT);
    int ret = connect(fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    
    if(ret != 0)
    {
        sys_err("conn error");
    }
    printf("cli conneted\n");
    char buf[BUFSIZ];

    int counter = 10;

    while(--counter)    
    {
        printf("client write \n");
        if(write(fd, "Hello\n", 5)< 0)
        {
            sys_err("write error");
        }
        //int num = read(fd, buf, sizeof(buf));
        //注意buf长度为获取的真正的长度，否则会乱码
        //write(STDOUT_FILENO, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, 5);
        //printf("write \n");
        sleep(1);
    }
    printf("close client\n");
    close(fd);
}