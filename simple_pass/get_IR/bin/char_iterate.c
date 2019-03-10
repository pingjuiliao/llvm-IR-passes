#include<stdio.h>
#include<string.h>

int 
main(void) {
    char* str = "hello" ;

    for ( int i = 0 ; i < strlen(str) ; ++i ) {
        printf(" %c\n", str[i]);
    }
    return 0 ;
}
