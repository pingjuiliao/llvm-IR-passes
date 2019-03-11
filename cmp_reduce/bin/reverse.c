#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
void
vuln(void) {
    char buf[10];
    read(0, buf, 32);
    return ;
}
void
strcmp_check(void) {
    char buf[8];
    read(0, buf, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    if ( strcmp(buf, "pingjui") != 0 ) {
        fprintf(stderr, "strcmp_check failed");
        exit(1);
    } 
    return ;
}
int
main(void) {
    uint32_t n ;
    
    strcmp_check() ;
    read(0, &n, sizeof(n));
    if ( n == 0x41424344 ) {
        vuln();
    }
    return 0 ;
}
