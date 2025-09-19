#include <stddef.h>
#include <stdbool.h>
#include "cpuid.h"

#include "debug/regs.h"
#include "print/print.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "memory/mmu.h"
#include "memory/malloc.h"
#include "pci/pcie.h" 
#include "fs/vfs.h"
#include "interrupts/init_idt.h"
 

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
    // set_color(BLUE_FGD, WHITE_BGD);
    // set_color(LIGHT_GREEN_FGD, DARK_GRAY_BGD);
    set_color(LIGHT_GREEN_FGD, BLACK_BGD);
    clear_screen();

    memory_init();
    uintptr_t rip;
    GET_RIP(rip);

    if (rip >= KERNEL_VIRT_BASE) {
        kprintf("Kernel entry point: %p\n", rip);
    } else {
        asm volatile("jmp *%0" :: "r"(KERNEL_REMAP(rip)));
    }

    // remap PIC
    pic_disable();
    pic_remapping(0x20);

    // intialize the IDT
    idt_init();

    // enable keyboard interrupts
    pic_clear_mask(1);
    pic_clear_mask(2);
    idt_set_descriptor(IRQ_KEYBOARD, KERNEL_REMAP(isr_stub_table[IRQ_KEYBOARD]), PRESENT|DPL_0|INT_GATE);    

    pci_init();

    init_vfs();
    kprintf(">: ");
    
    for(;;) {
        asm("hlt");
    }
}