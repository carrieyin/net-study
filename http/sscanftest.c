#include <stdio.h>
#include <stdlib.h>

int main()
{
    char * s = "1.jpg";
    char name[10];
    char ex[10];
    char buf[10];
    //sscanf("1. jpg", "%*s%s", buf);
    sscanf("1.jpg", "%[^.].%s", buf,ex);
    printf("%s---- %s\n", buf, ex);
    
}