#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char* s = "happy\n" ;
uint64_t h = 0x100000000 ;
int
main(void) {
    uint64_t v = h ;
    puts(s);
    printf("%lu\n", v) ;
    return 0;
}
