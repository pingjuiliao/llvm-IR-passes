#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

void swap(int *a, int *b) {
    char buf[20];
    int tmp = *b ;
    *b = *a ;
    *a = tmp ;
    read(0, buf, 20);
    write(1, buf, 20);
}

void printA(int a[], size_t size) {
    int i;
    char buf[20];
    for (i = 0 ; i < size-1 ; ++ i ) {
        printf("%d -> ", a[i]);
    } 
    printf("%d\n", a[i]);
    read(0, buf, 20);
    write(0, buf, 20);
}

int
main(int argc, char** argv) {
    int array[20] = { 0 } ;
    int i , j;
    srand(time(0));
    for ( i = 0 ; i < 20 ; ++i ) {
        array[i] = rand() % 20 ;
    }
    for ( i = 0; i < 19 ; ++i ) {
        for (j = i + 1; j < 20 ; ++j) {
            if ( array[i] > array[j] ) {
                swap(&array[i], &array[j]);
            }
        }
    }
    printA(array, 20);
    return 0 ;
}
