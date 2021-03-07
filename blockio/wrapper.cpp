#include  <unistd.h>
#include  <arpa/inet.h>       /* inet(3) functions */
 
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <string.h>
    #include <unistd.h>
    #include <netinet/in.h>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}


int Socket(int domain, int type, int protocol)
{
    int fd = 0;
    fd = socket(domain, type, 0);
    if(fd == -1)
    {
        sys_err("socket error");
    }

    return fd;
}

int Bind(int socket, struct sockaddr *address, socklen_t address_len)
{
    int ret = bind(socket, (struct sockaddr *)address, address_len); 
    if(ret != 0)
    {
        sys_err("bind error");
    }
    return ret;
}

int Listen(int socket)
{
    int ret = listen(socket, 128);
    if(ret != 0)
    {
        sys_err("error listen");
    }

    return ret;
}

int Accept(int socket, struct sockaddr * address, socklen_t * address_len)
{
    int cfd = accept(socket, (struct sockaddr*)address, address_len);
    if(cfd < 0)
    {
        sys_err("accept error");
    }

    return cfd;
}