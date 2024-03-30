#include <stdint.h>

__attribute__((noreturn)) void exception_handler(void);
void exception_handler()
{
    int src = 1;
    int dst;

    asm ("mov %1, %0\n\t"
        "add $1, %0"
        : "=r" (dst)
        : "r" (src));

    // printf("%d\n", dst);

    char* c = (char*)0x70;
    kputs(c);

    kputs("Exception!!");
    while (true) {
        __asm__ volatile ("cli; hlt");
    }
}

