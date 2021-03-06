#include "wrapper.h"
#define SER_PORT 9527

#include <thread>

void close(int cfd, int fd)
{
    close(fd);
    close(cfd);
}
int main()
{
    char buf[BUFSIZ];
    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SER_PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t len = sizeof(ser_addr);
    Bind(fd, (struct sockaddr*)&ser_addr, len);

    Listen(fd);
    int count = 0;
    while(true)
    {
        struct sockaddr_in cli_addr;
        socklen_t cli_addr_len = sizeof(cli_addr);
        memset(&cli_addr, 0, sizeof(cli_addr));
        
        int cfd = Accept(fd, (struct sockaddr*)&(cli_addr), &cli_addr_len);

        char cli_ip[30];
        const char *p = inet_ntop(AF_INET, &cli_addr, cli_ip, cli_addr_len);
        if(cfd == -1)
        {
            sys_err("accect error");
        }
        printf("ser accecped \n");
        printf("cfd is :%d \n", cfd);
        count++;
        std::thread thr([count, &buf, cfd](){
            int nr ;
            printf("in thr cfd is :%d \n", cfd);
            while ((nr = read(cfd, buf, sizeof(buf))) > 0)
            {
                printf("read bytes: %d\n", nr);
                //printf("start read %d bytes\n", ret);
                int nw = write(cfd, buf, nr);
                if(nw < nr)
                {
                    break;
                }
                printf("end read %d bytes\n", nr);               
            }
            
        });
    }
}
