#include "cpuid.h"
#include "print/print.h"
#include "interrupts/init_idt.h"
#include "memory/mmu.h"


#define APIC_MASK 1 << 9

static int check_apic(void){
    uint32_t unused, edx;
    __cpuid(1, unused, unused, unused, edx);
    return edx & APIC_MASK;
}

void interrupt(){
    __asm__ __volatile__("int3;");
}

void main()
{
    // set_color(WHITE_FGD, BLUE_BGD);
    set_color(LIGHT_GREEN_FGD, DARK_GRAY_BGD);
    clear_screen();
    // map_kernel();
    // for(;;);
    // good
    if(check_apic()){
        // these strings are at 0x3118 when clearscreen is called is asm these are nowhere to be found
        // on the good version they are located at 0x2118...hmmm
        kputs("+--------------------------------------+");
        kputs("|Hello from the kernel! APIC IS allowed|");
        kputs("+--------------------------------------+\n");
    }else{
        kputs("+--------------------------------------+");
        kputs("Hello from the kernel! APIC NOT allowed");
        kputs("+--------------------------------------+\n");
    }

    // for(;;);
    
    // for(int i = 0; i < 1; i++){
    //     uint64_t temp;
    //     __asm__(
    //         "mov (%1), %%rax;"
    //         "mov %%rax, %0;"
    //         : "=r"(temp)
    //         : "r"(0xffffffff00000000) 
    //         : "eax"
    //     );
    //     // kprintf("addr: 0x%x; ", 0xc000+i);
    //     // kprintf("val: 0x%x\n", temp);
    // }   
    
    // for(;;) {
    //     asm("hlt");
    // }

    kprintf(">:  ", 0);

    // remap PIC
    pic_disable();
    pic_remapping(0x20);

    // intialize the IDT
    idt_init();

    // enable keyboard interrupts
    pic_clear_mask(1);
    pic_clear_mask(2);
    idt_set_descriptor(0x21, isr_stub_table[0x21], PRESENT|DPL_0|INT_GATE);


    // uint64_t *kernel_addr = (uint64_t*)0xdeadb000;
    mem();
    // if(!(*kernel_addr)){
    //     ((void(*)(void))0xdeadb000)();
    // }

    
    // char* ptr = (char*)0xdeadbeef;
    // ptr[0] = 'h';

    // asm(
    //     "mov $0xdeadb000, %%rax;"
    //     "call %%rax;"
    
    //  : : :"rax");
    

    for(;;) {
        asm("hlt");
    }
}

// Phase 0: Introduction
// 1. Welcome to Operating Systems Development
// 2. Building the latest GCC

// Phase I: Beginning
// In this phase we will set up a toolchain and create a basic kernel that will become the core of the new operating system.
// 1. Setting up a Cross-Toolchain
// 2. Creating a Hello World kernel
// 3. Setting up a Project
// 4. Calling Global Constructors
// 5. Terminal Support
// 6. Stack Smash Protector
// 7. Multiboot
// 8. Global Descriptor Table
// 9. Memory Management
// 10. Interrupts
// 11. Multithreaded Kernel
// 12. Keyboard
// 13. Internal Kernel Debugger
// 14. Filesystem Support

// Phase II: User-Space
// In this phase we'll expand your operating system into user-space and add support for programs, which is enough for your project to qualify as a small operating system. You need to work on system calls, program loading, memory management and rework parts of your system early in this phase.
// 1. User-Space
// 2. Program Loading
// 3. System Calls
// 4. OS Specific Toolchain
// 5. Creating a C Library
// 6. Fork and Execute
// 7. Shell

// Phase III: Extending your Operating System
// With these basic features in place, you can now start writing your operating system and all its wonderful features. You'll add games, editors, test programs, command line utilties, drivers and whatever you can imagine. Your skill and imagination is the limit here. You can delay many of these features until later in your operating system and make then in almost any order.
// 1. Time
// 2. Threads
// 3. Thread Local Storage
// 4. Symmetric Multiprocessing
// 5. Secondary Storage
// 6. Real Filesystems
// 7. Graphics
// 8. User Interface
// 9. Networking
// 10. Sound
// 11. Universal Serial Bus

// Phase IV: Bootstrapping
// You now have your basic operating system in place and you are ready to move onto the next level. In this phase, we will start porting software onto your operating system such that you can become self-hosting. You already begun your effort toward being self-hosting when you set up your OS-specific toolchain and it pays off now.
// 1. Porting Software
// 2. Porting GCC
// 3. Compiling your OS under your OS
// 4. Fully Self-hosting

// Phase V: Profit
// You have now successfully created a real operating system that is fully self-hosting and the envy of the entire operating systems development community. You have ported quake, have OpenGL programs, a working browser, a thriving community of contributors, and much success. You can now start over and develop the next operating system from your own operating system.