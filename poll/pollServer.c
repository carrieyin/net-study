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
    int index = 0;
    struct pollfd pollfds[OPEN_MAX];

    pollfds[0].fd = fd;
    pollfds[0].events = POLL_IN;

    
    int nready =0;
    while(1)
    {
        nready = poll(pollfds, max + 1, -1);

        if(nready  < 0)
        {
            continue;
        }

        if(pollfds[0].revents == POLL_IN)
        {
            int cfd = Accept(fd, &cli_addr, &cli_addr_len);
            index++;
            pollfds[index].fd = cfd;
            pollfds[index].events = POLL_IN;
            max++;

            if(--nready == 0)
            {
                continue;
            }
        }
        
        for(int i = 1; i < max; i++)
        {
            if(pollfds[i].revents == POLL_IN)
            {
                char buf[BUFSIZ];
                int nread = Read(pollfds[i].fd, buf, BUFSIZ);
                if(nread == 0)
                {
                    close(pollfds[i].fd);
                }else if(nread > 0)
                {
                    write(pollfds[i].fd, buf, nread);
                    write(STDOUT_FILENO, buf, nread);
                }
            }
        }

    }
    
}