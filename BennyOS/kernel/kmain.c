// #include "print/print.h"

void print(const char *string, int color){
    // const char *string = "Hello from the kernel!";
    volatile char *video = (volatile char*)0xb8000;
    *video++ = *string++;
    *video++ = color;

    *video++ = *string++;
    *video++ = color;
    // while( *string != 0 )
    // {
    //     *video++ = *string++;
    //     *video++ = 0x1f;
    // }
}


void main(){
    const char *string = "Hello from the kernel!";
    print("Hello from the kernel!", 0x1f);
    // print();
}