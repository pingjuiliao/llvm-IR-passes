#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<string.h>

char gbuf[32];
void strcmp_check(void);
void int32_check(void);
void biggerthan_check(void);
void vuln(void);

int
main( int argc, char** argv) {
    
    char cmd[5];
    
    strcmp_check();
    int32_check();
    biggerthan_check();
    vuln();



    return 0;
}
void
strcmp_check(void) {
    char buf[8];
    read(0, buf, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    if ( strcmp(buf, "pingjui") != 0 ) {
        fprintf(stderr, "strcmp_check failed\n");
        exit(1);
    } 
    return ;
}

void 
int32_check(void) {
    uint32_t n ;
    read(0, &n, sizeof(uint32_t));
    if ( n != 0x41414141 ) {
        fprintf(stderr, "Your n == %08x int32_check failed\n", n);
        exit(1);
    }
    return ;
}


void
biggerthan_check(void) {
    char int_buf[10] ;
    int i ;
    read(0, &int_buf, sizeof(int_buf));
    int_buf[sizeof(int_buf)-1] = '\0';
    i = atoi(int_buf);
    if ( i > 0 ) {
        fprintf(stderr, "biggerthan_check failed\n");
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
