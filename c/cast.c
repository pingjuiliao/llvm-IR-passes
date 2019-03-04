#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
int
main(int argc, char** argv) {
    int i ;
    if ( argc < 2 ) {
        fprintf(stderr, "Usage: %s <signed integer>", argv[0]);
        exit(1);
    }
    i = atoi(argv[1]);
    printf("%u\n", (uint32_t) i);
    return 0;
}
