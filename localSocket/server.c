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
#define SER_PATH "server.socket"

int main()
{
    
    int ret;
    char buf[BUFSIZ];
    
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un ser_addr;
    ser_addr.sun_family = AF_UNIX;
    strcpy(ser_addr.sun_path, SER_PATH);
    int len;
    len = offsetof(struct sockaddr_un, sun_path) + strlen(ser_addr.sun_path);
    unlink(SER_PATH);
    bind(fd, (const struct socketaddr*)&ser_addr, len);

    listen(fd, 20);

    while (1)
    {
        struct sockaddr_un cli_addr;
        int len = sizeof(cli_addr);
        int cfd = accept(fd, (struct sockaddr *)&cli_addr, (socklen_t *)&len);
        len -= offsetof(struct sockaddr_un, sun_path);
        cli_addr.sun_path[len] = '\0';

        printf("client bind filename %s \n", cli_addr.sun_path);
        
        int size;
        while((size = read(cfd, buf, sizeof(buf))) > 0)
        {
            for (int i = 0; i < sizeof(buf); i++)
            {
                buf[i] = toupper(buf[i]);
            }
            
             write(cfd, buf, size);
             write(STDOUT_FILENO, buf, size);
        }
           
    }
    
}
