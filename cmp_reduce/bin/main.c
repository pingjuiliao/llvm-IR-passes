#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<string.h>

char gbuf[32];
void strcmp_check(void);
void int32_check(void);
void vuln(void);

int
main( int argc, char** argv) {
    
    char cmd[5];
    
    strcmp_check();
    int32_check();
    vuln();



    return 0;
}

void 
int32_check(void) {
    uint32_t n ;
    read(0, &n, sizeof(uint32_t));
    if ( n != 0x41424344 ) {
        fprintf(stderr, "Your n == %08x int32_check failed\n", n);
        exit(1);
    }
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

void
vuln(void) {
    char b[5];
    memset(b, 0, sizeof(b));
    read(0, gbuf, sizeof(gbuf));
    strcpy(b, gbuf);
    return ; 
}
