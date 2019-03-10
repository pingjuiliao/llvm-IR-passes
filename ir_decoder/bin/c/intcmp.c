#include<stdio.h>
#include<stdint.h>

int
main(void) {
    uint64_t v = 0x4142434445464748 ;
    int i ;
    char c ;
    uint8_t *i0, *i1, *i2, *i3 ;
    
    c = ((char *) &v)[0];
    printf("%c -", c) ;
    c = ((char *) &v)[1];
    printf("%c -", c) ;
    c = ((char *) &v)[2];
    printf("%c -", c) ;
    c = ((char *) &v)[3];
    printf("%c -", c) ;
    puts(" is the integer presented byte by byte");
    
    i0 = (uint8_t *) &v + 0 ;
    i1 = (uint8_t *) &v + 1 ;
    i2 = (uint8_t *) &v + 2 ;
    i3 = (uint8_t *) &v + 3 ;
    
    if ( ((char *) &v)[0] == '\x48' && ((char *) &v)[1] == '\x47' ) {

        puts("Yes !") ;
    } 


    //if ( *i0 == 0x44 && *i1 == 0x43 && *i2 == 0x42 && *i3 == 0x41 )
    //    puts("Yes! You guess correctly") ;

    return 0 ;
}
