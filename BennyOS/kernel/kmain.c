#include "print/print.h"


void main(){
    set_color(WHITE_FGD, BLUE_BGD);
    clear_screen();
    puts("Hello from the kernel!");
}