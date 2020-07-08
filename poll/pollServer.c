#include "wrapper.h"
#include "poll.h"

#define PORT 9999
#define OPEN_MAX 1024
int main()
{
    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_len = sizeof(ser_addr);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(fd, (const struct socketaddr*)&ser_addr, ser_addr.sin_len);

    Listen(fd);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&cli_addr, 0, sizeof(cli_addr));

    int max = 0;
    struct pollfd pollfds[OPEN_MAX];
    struct pollfd listenpool;
    listenpool.fd = fd;
    listenpool.events = POLL_IN;
    


}