#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stddef.h>

#define SER_PORT 9527
#define CLI_PATH "cli.socket"
#define SER_PATH "server.socket"

int sys_err(const char* s)
{
    perror(s);
    exit(0);
}
int main()
{
    char buf[BUFSIZ];
    
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un cli_addr;
    cli_addr.sun_family = AF_UNIX;
    strcpy(cli_addr.sun_path, CLI_PATH);
    int len;
    len = offsetof(struct sockaddr_un, sun_path) + strlen(cli_addr.sun_path);
    unlink(CLI_PATH);
    bind(fd, (const struct socketaddr*)&cli_addr, len);

   
    struct sockaddr_un ser_addr;
    ser_addr.sun_family = AF_UNIX;
    strcpy(ser_addr.sun_path, SER_PATH);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(ser_addr.sun_path);
    int ret = connect(fd, (struct sockaddr *)&ser_addr, len);
    
    if(ret != 0)
    {
        sys_err("conn error");
    }

    int size = 0;
    while((size = fgets(buf, sizeof(buf), stdin)) > 0)    
    {
        if(write(fd, buf, size)< 0)
        {
            sys_err("write error");
        }
        write(STDOUT_FILENO, buf, size);
        //printf("write \n");
    }
    close(fd);
}