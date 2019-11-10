#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#define BUF_SIZE 128

char glob_buf[BUF_SIZE] ;
char* hello_str = "hello world" ;

void 
ERROR(char* format, ...) {
    va_list args;
    printf("[ERROR] ");
    va_start(args, format) ;
    vprintf(format, args) ;
    va_end(args) ;
    puts("");
    exit(-1);

}

void
arbitrary_write_local_stack(void) {
    char tmp[20] ;
    char buf[BUF_SIZE] ;
    int r, x;
    
    memset(tmp, 0 , sizeof(tmp)) ;
    memset(buf, '.', sizeof(buf)) ;
    if ((r = read(0, tmp, sizeof(tmp))) <= 0 ) {
        ERROR("Failed on read() r == %d", r) ;        
    }
    x = atoi(tmp) ;
    printf("x == %d\n", x) ;
    
    buf[x] = '$' ;
    return ;
}

void
arbitrary_write_heap(void) {
    char tmp[20] ;
    char* buf = malloc(BUF_SIZE) ;
    int r, x;
    
    memset(tmp, 0 , sizeof(tmp)) ;
    memset(buf, '.', BUF_SIZE) ;
    if ((r = read(0, tmp, sizeof(tmp))) <= 0 ) {
        ERROR("Failed on read() r == %d", r) ;        
    }
    x = atoi(tmp) ;
    printf("x == %d\n", x) ;
    
    buf[x] = '$' ;
    return ;
}

void
simple_false_positive(void) {
    char* s = hello_str ;
    puts(s) ;
}


int
main( int argc, char** argv) {
    arbitrary_write_local_stack() ;
    arbitrary_write_heap() ;
    simple_false_positive();
    return 0;
}
