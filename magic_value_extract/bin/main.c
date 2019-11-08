#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>

void
one_byte_cmp(char* input) {
    if ( input[3] == 'a' )
        if ( input[2] == 'b' )
            if ( input[1] == 'c' )
                if ( input[0] == 'a' )
                    return ;
    exit(1) ;
}
void two_byte_cmp(char* input) {
    uint16_t x = *((uint16_t *) input);
    uint16_t y = *((uint16_t *) (input +2));
    if ( x == 0x4141 && y == 0x4242 )
        return ;
    exit(1) ;
} 

void four_byte_cmp(char* input) {
    uint32_t x = (uint32_t) input ;
    if ( x == 0x68732f6e )
        return ;
    exit(1) ;
}


int
main( int argc, char** argv) {
    
    int r ; 
    char buf[50] ;
    memset(buf, 0 , sizeof(buf)) ;
    if ( (r = read(0, buf, sizeof(buf))) < 0 ) {
        fprintf(stderr, "read() error\n") ;
        exit(-1) ;
    }

    one_byte_cmp(&buf[3]) ;
    two_byte_cmp(&buf[9]) ;
    four_byte_cmp(&buf[20]);
    __builtin_trap();
    return 0;
}


