#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<unistd.h>


void
func1(void) {
    puts("func1") ;
}
void
func2(void) {
    puts("func2") ;
}
void
func3(void) {
    puts("func3") ;
}
int
main( int argc, char** argv) {
    uint64_t i = 0 ;

    printf("Hello World !");
    func1();
    func2();
    func3();

    return 0;
}




