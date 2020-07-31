#include <event2/event.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sys_error(const char *s)
{
    perror(s);
    exit(0);
}

void read_cb(int fd, short what, void *arg)
{
    char buf[1024];
    int len = read(fd, buf, sizeof(buf));

    printf("read : %s \n", buf);

    sleep(1);

}

int main()
{
    unlink("fifoevent");
    mkfifo("fifoevent", 0664);

    int fd = open("fifoevent", O_RDONLY | O_NONBLOCK);
    if(fd == -1)
    {
        sys_error("open error");
    }

    struct event_base *base;
    base = event_base_new();
    struct event * event = event_new(base, fd, EV_READ|EV_PERSIST, read_cb, NULL);
    event_add(event, NULL);

    event_base_dispatch(base);

    event_base_free(base);
}