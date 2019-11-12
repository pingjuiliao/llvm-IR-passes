#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(void) {
    int r;
    char buf[20] ;
    memset(buf, 0, sizeof(buf)) ;
    if ( (r = read(0, buf, sizeof(buf))) <= 0 ) {
        exit(-1) ;            
    }
    if ( strcmp("hello", buf) == 0 ) {
        __builtin_trap() ;
    }

    return 0 ;
}
