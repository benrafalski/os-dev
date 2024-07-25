#include <stdio.h>
#include <stdlib.h>


void main(void){

    char* ptr = (char*)malloc(sizeof(char));
    *ptr = 'c';
    char* ptr2 = ptr;
    ptr2 += 1;
    *ptr2 = 'a';
    printf("ptr: 0x%llx\n", ptr);
    printf("ptr*: %c\n", *ptr);

    printf("ptr2: 0x%llx\n", ptr2);
    printf("ptr2*: %c\n", *ptr2);

    while(1);
}