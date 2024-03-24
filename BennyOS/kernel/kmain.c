#include "print/print.h"


void main(){
    set_color(WHITE_FGD, BLUE_BGD);
    clear_screen();
    kputs("Hello from the kernel!");
}