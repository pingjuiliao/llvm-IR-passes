#include<stdio.h>
#include<stdlib.h>

int
main(int argc, char** argv) {
    int i, n;
    scanf("%d", &n);
    if ( n > 100 || n < 0) {
        puts("invalid");
        exit(1);
    }
    for ( i= 2; i < 100; i = i + 2) {
        n = n + i ;
    }
    printf("n is %d\n", n);
    return 0;
}
