#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
uint32_t fibo(uint32_t n);

int
main( int argc, char** argv) {
    uint32_t result = 0;
    uint32_t i , n = 1 ;
    for ( i = 0 ; i < 10 ; ++i ) {
        printf("%d + %d == %d\n", n, i , n + i) ;
        n += i ;
    }
    return 0;

}

uint32_t 
fibo(uint32_t n) {
    if ( n <= 1 ) {
        return 1 ;
    }
    return fibo(n-1) + fibo(n-2) ;
}

