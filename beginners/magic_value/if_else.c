#include<stdio.h>

int
main( int argc, char** argv) {
    int i = 0;
    while( scanf("%d", &i) != EOF ) {
        if ( 0xcafebabe) {
            puts("you found the magic value");
        } else {
            puts("you did not found the magic value");
        }
    }

