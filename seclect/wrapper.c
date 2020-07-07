#include "wrapper.h"

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

int Bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    
    int ret = bind(socket, address, address_len); 
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

int Accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len)
{
    int cfd = accept(socket, address, address_len);
    if(cfd < 0)
    {
        sys_err("accept error");
    }

    return cfd;
}


int Read(int socket, char* buf, int size)
{
    int ret = 0;
    
    ret = read(socket, buf, sizeof(buf));
    if(ret < 0)
    {
        sys_err("ser read error");
    }
     
    return ret;
}