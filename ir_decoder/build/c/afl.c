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
    afl_maybe_log() ;
    return 0 ;
}
