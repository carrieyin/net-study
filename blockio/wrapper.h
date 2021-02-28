#ifndef _WRAPPER_H
#define _WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

extern void sys_err(const char* str);
extern int Socket(int domain, int type, int protocol);
extern int Bind(int socket, struct sockaddr *address, socklen_t address_len);
extern int Listen(int socket);
extern int Accept(int socket, struct sockaddr * address, socklen_t * address_len);

#endif
