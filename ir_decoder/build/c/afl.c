#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t* afl_area_ptr ;
uint8_t** afl_global_area_ptr ;

void
afl_setup(void) {
    char* tmp = malloc(64) ;
    afl_area_ptr = tmp ;
    *afl_global_area_ptr = tmp ;
} 

void
afl_maybe_log(void) {
    if ( !afl_area_ptr ) {
        if ( afl_global_area_ptr ) {
            afl_area_ptr = *afl_global_area_ptr;
        } else {
            afl_setup() ;
        }
    }
}
int
main(void) {
    int r ;
    uint64_t x = 30;
    afl_maybe_log() ;
    if ((r = read(0, &x, sizeof(x))) <= 0 ) {
        fprintf(stderr, "failure\n") ;
        exit(-1) ;
    }
    if ( x == 0xffffffffffffffff ) {
        puts("correct");
    }

    return 0 ;
}
