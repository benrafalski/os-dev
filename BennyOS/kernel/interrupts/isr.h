#include <stdint.h>

__attribute__((noreturn)) void exception_handler(void);
void exception_handler()
{
    unsigned long long src = 1;
    unsigned long long dst;

    __asm__ volatile("movq %%rax, %0;"
                     : "=r"(dst)
                     :
                     : "rax"
    );

    char *str;
    str = citoa(dst, str, 10);
    kputs("Exception!! Error code: ");
    kputs(str);
    while (true)
    {
        __asm__ volatile("cli; hlt");
    }
}
