#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<string.h>

char buf[50];
void vuln(void);

int
main( int argc, char** argv) {
    vuln();
    return 0;
}

void
vuln(void) {
    char dst[10];
    read(0, buf, sizeof(buf));
    strcpy(dst,buf);
    return ;
}

