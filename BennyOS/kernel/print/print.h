// #include "colors.h"

// #define VIDEO_MEMORY 0xb8000

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