#include "wrapper.h"
#define PORT 6587

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
    fd_set rset,allset;
    int ret, maxfd;
    maxfd = fd;
    int maxindex = -1;
    FD_ZERO(&allset);
    FD_SET(fd, &allset);
    int cfds[1024];
    memset(cfds, -1, sizeof(cfds));

    while(1)
    {
        printf("start-------\n");
        rset = allset;
        ret = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(ret < 0)
        {
            sys_err("select error");
        }

        if(FD_ISSET(fd, &rset))
        {
            printf("select fd \n");
            int cfd = Accept(fd, &(cli_addr), &cli_addr_len);

            char cli_ip[30];
            bzero(cli_ip, sizeof(cli_ip));
            const char *p = inet_ntop(AF_INET, &cli_addr, cli_ip, cli_addr_len);
            printf("cli ip: %s, port:%d \n", p, ntohs(cli_addr.sin_port));
            FD_SET(cfd, &allset);
            
            for(int index = 0; index < 1024; index++)
            {
                if(cfds[index] < 0)
                {
                    cfds[index] = cfd;
                    break;
                }
            }
            if(index == 1024)
            {
                sys_err("out of max fd");
            }

            if(maxindex < index)
            {
                maxindex = index;
            }
            printf("maxindex %d \n", maxindex);
            if(--ret == 0) //只返回listenfd，后续无需执行
            {
                continue;
            }
        }
        
        for(int i = 0; i <= maxindex; i++)
        {
            printf("connet fd %d \n", cfds[i]);
            int socketfd = cfds[i];
            if(socketfd < 0)
            {
                continue;
            }
            
            if(FD_ISSET(socketfd, &rset))
            {
                char buf[BUFSIZ];
                int n = Read(socketfd, buf, sizeof(buf));
                if(n < 0)
                {
                    sys_err("read error");
                }
                else if(n == 0)
                {
                    close(socketfd);
                    cfds[i] = -1;
                    FD_CLR(socketfd, &allset);
                }
                else
                {
                    for(int j = 0; j < n; j++)
                    {
                        buf[j] = toupper(buf[j]);
                    }

                    write(i, buf, n);
                    write(STDOUT_FILENO, buf, n);
                }
                
            }

            if(--ret == 0)
            {
                break;
            }
        }

        
    }

    close(fd);   
    return 0;
}
