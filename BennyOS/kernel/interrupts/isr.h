#include <stdint.h>
#include "pic.h"

typedef struct {
    struct {
        uint64_t    cr4;
        uint64_t    cr3;
        uint64_t    cr2;
        uint64_t    cr0;
    } control_registers;

    struct {
        uint64_t    r15;
        uint64_t    r14;
        uint64_t    r13;
        uint64_t    r12;
        uint64_t    r11;
        uint64_t    r10;
        uint64_t    r9;
        uint64_t    r8;
        uint64_t    rdi;
        uint64_t    rsi;
        uint64_t    rdx;
        uint64_t    rcx;
        uint64_t    rbx;
        uint64_t    rax;
    } general_registers;
	
    struct {
        uint64_t    rbp;
        uint64_t    vector;
        uint64_t    error_code;
        uint64_t    rip;
        uint64_t    cs;
        uint64_t    rflags;
        uint64_t    rsp;
        uint64_t    dss;
    } base_frame;
} isr_frame_t;

typedef struct {
    uint8_t scan_code;
    uint8_t key;
} scancode_t;


__attribute__((noreturn)) void exception_handler(isr_frame_t* frame);
void exception_handler(isr_frame_t* frame)
{
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

void print_exception_msg(const char* msg){
    uint8_t color = get_color();
    set_color(RED_FGD, BLUE_BGD);
    kputs(msg);
    set_color(color & 0xf, color & 0xf0);
}

// FAULTS: $rip will contain the value of the instruction that caused the exception
// TRAPS: $rip will contain the value of the instruction after the instruction that caused the exception


// TODO: figure out how to make it safe
// Division Error (Fault)
//      Occurs when dividing any number by 0 using the DIV or IDIV instruction
void exception_handler_0(isr_frame_t* frame);
void exception_handler_0(isr_frame_t* frame){
    print_exception_msg("Exception 0 caught (fault): divide by zero, continuing...\n");
    frame->base_frame.rip += 1;
}

// Debug (Trap)
//      Instruction fetch breakpoint (Fault)
//      General detect condition (Fault)
//      Data read or write breakpoint (Trap)
//      I/O read or write breakpoint (Trap)
//      Single-step (Trap)
//      Task-switch (Trap)   
void exception_handler_1(isr_frame_t* frame);
void exception_handler_1(isr_frame_t* frame){
    print_exception_msg("Exception 1 caught (trap): debug interrupt, continuing...\n");
}

// https://wiki.osdev.org/Non_Maskable_Interrupt
void exception_handler_2(isr_frame_t* frame);
void exception_handler_2(isr_frame_t* frame){
    print_exception_msg("Exception 2 caught (interrupt): non-maskable interrupt, continuing...\n");
}

// Keyboard Interrupt
void exception_handler_33(isr_frame_t* frame);
void exception_handler_33(isr_frame_t* frame){
    // kputs("Keyboard interrupt!");
    // frame->base_frame.rip += 1;
    // key = 'l';

    // char* scancodes[4] = {
        
    // };

    // char* s1 = "1234567890-=";
    // char* s2 = "qwertyuiop[]";
    // char* s3 = "asdfghjkl;'`";
    // char* s4 = "zxcvbnm,./";


    // 0xe = backspace
    // 0xf = tab
    // 0x1c = enter
    // 0x1d = left ctrl
    // 0x2A	left shift pressed
    // 0x2B	\ pressed



    // unsigned char scan_code = inb(0x60);
    // if(scan_code >= 0x01 && scan_code <= 0x0C){

    // }

    
    pic_send_eoi(1);

}




// "1234567890-="
// // 0xe = backspace
// // 0xf = tab
// "qwertyuiop[]"
// // 0x1c = enter
// // 0x1d = left ctrl
// "asdfghjkl;'`"
// // 0x2A	left shift pressed
// // 0x2B	\ pressed
// "zxcvbnm,./"



