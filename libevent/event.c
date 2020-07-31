#include <stdio.h>
#include <event2/event.h>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

int main()
{
    struct event_base *base = event_base_new();
    char **buf;
    buf = event_get_supported_methods();
    
    char **p = buf;
    while (p != NULL && *p != NULL)
    {
        printf("support: %s \n", *p);
        p++;
    }
    

    return 0;
}