#include<stdio.h>
#include<stdint.h>
uint32_t fibo(uint32_t n);

int
main( int argc, char** argv) {
    
    uint32_t i = fibo(15);
    printf("fibbonacci(15) number is %u\n", i);
    return 0;
}

uint32_t 
fibo(uint32_t n) {
    if ( n <= 1 ) {
        return 1 ;
    }
    return fibo(n-1) + fibo(n-2) ;
}

