#include<stdio.h>
#include<stdlib.h>

void vuln();
int
main(int argc, char** argv) {
    int n;
    scanf("%d", &n);    
    if ( n == 1337 ) {
        vuln();
    }
}

void
vuln(void) {
    char buf[50] ;
    read(0, buf, 0x100);
    puts(buf);
    return ;
}
