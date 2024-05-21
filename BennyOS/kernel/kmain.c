#include "cpuid.h"
#include "print/print.h"
#include "interrupts/init_idt.h"
#include "memory/pmm.h"
#include "memory/mmu.h"


#define APIC_MASK 1 << 9

// int val = 0;

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
    set_color(LIGHT_GREEN_FGD, DARK_GRAY_BGD); // 0x2770
    clear_screen(); // 0x277a


    if(check_apic()){ // 0x277f
        kputs("+--------------------------------------+"); // 0x279b
        kputs("|Hello from the kernel! APIC IS allowed|");
        kputs("+--------------------------------------+\n");
    }else{
        kputs("+--------------------------------------+");
        kputs("Hello from the kernel! APIC NOT allowed");
        kputs("+--------------------------------------+\n");
    }

    // return;

    // for(;;); //0x268f:
    
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

    // return;
    

    // intialize the IDT
    // breaks here
    idt_init();

    // return;

    // enable keyboard interrupts
    pic_clear_mask(1);
    pic_clear_mask(2);
    idt_set_descriptor(0x21, isr_stub_table[0x21], PRESENT|DPL_0|INT_GATE);

    // map_block(0x300000, 0xFFFFC90000000000);

    // kmalloc(20);

    // map_page(0x7FFF000, 0xFFFFC90000000000);


    // char* test = (char*)0xFFFFC90000000000;
    // test[0] = 'w';

    // kputs(test);

    // init_physmem();


    // kprintf("%x\n", 0x5);
    
    for(;;) {
        asm("hlt");
    }
}

// Phase 0: Introduction
// DONE
// 1. Welcome to Operating Systems Development
// DONE
// 2. Building the latest GCC

// Phase I: Beginning
// In this phase we will set up a toolchain and create a basic kernel that will become the core of the new operating system.
// DONE
// 1. Setting up a Cross-Toolchain
// DONE
// 2. Creating a Hello World kernel
// DONE
// 3. Setting up a Project
// DONE
// 4. Calling Global Constructors
// DONE
// 5. Terminal Support
// TODO
// 6. Stack Smash Protector
// TODO
// 7. Multiboot
// DONE
// 8. Global Descriptor Table
// TODO
// 9. Memory Management
// DONE
// 10. Interrupts
// TODO
// 11. Multithreaded Kernel
// DONE
// 12. Keyboard
// TODO
// 13. Internal Kernel Debugger
// TODO
// 14. Filesystem Support

// Phase II: User-Space
// In this phase we'll expand your operating system into user-space and add support for programs, which is enough for your project to qualify as a small operating system. You need to work on system calls, program loading, memory management and rework parts of your system early in this phase.
// TODO
// 1. User-Space
// TODO
// 2. Program Loading
// TODO
// 3. System Calls
// TODO
// 4. OS Specific Toolchain
// TODO
// 5. Creating a C Library
// TODO
// 6. Fork and Execute
// TODO
// 7. Shell

// Phase III: Extending your Operating System
// With these basic features in place, you can now start writing your operating system and all its wonderful features. You'll add games, editors, test programs, command line utilties, drivers and whatever you can imagine. Your skill and imagination is the limit here. You can delay many of these features until later in your operating system and make then in almost any order.
// TODO
// 1. Time
// TODO
// 2. Threads
// TODO
// 3. Thread Local Storage
// TODO
// 4. Symmetric Multiprocessing
// TODO
// 5. Secondary Storage
// TODO
// 6. Real Filesystems
// TODO
// 7. Graphics
// TODO
// 8. User Interface
// TODO
// 9. Networking
// TODO
// 10. Sound
// TODO
// 11. Universal Serial Bus

// Phase IV: Bootstrapping
// You now have your basic operating system in place and you are ready to move onto the next level. In this phase, we will start porting software onto your operating system such that you can become self-hosting. You already begun your effort toward being self-hosting when you set up your OS-specific toolchain and it pays off now.
// TODO
// 1. Porting Software
// TODO
// 2. Porting GCC
// TODO
// 3. Compiling your OS under your OS
// TODO
// 4. Fully Self-hosting

// Phase V: Profit
// You have now successfully created a real operating system that is fully self-hosting and the envy of the entire operating systems development community. You have ported quake, have OpenGL programs, a working browser, a thriving community of contributors, and much success. You can now start over and develop the next operating system from your own operating system.