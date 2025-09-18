#include <stddef.h>
#include "cpuid.h"

#include "debug/regs.h"
#include "print/print.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "memory/mmu.h"
#include "memory/malloc.h"
#include "fs/vfs.h"
#include "interrupts/init_idt.h"

#define APIC_MASK 1 << 9
#define MEMORY_MAP_ADDR 0x9500

// Memory map entry structure (matches what bootloader creates)
typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t extended_attrs;
} __attribute__((packed)) memory_map_entry_t;

// Memory map types
#define MEMORY_TYPE_AVAILABLE 1
#define MEMORY_TYPE_RESERVED 2
#define MEMORY_TYPE_ACPI_RECLAIMABLE 3
#define MEMORY_TYPE_ACPI_NVS 4
#define MEMORY_TYPE_BAD 5

static const char* get_memory_type_string(uint32_t type) {
    switch(type) {
        case MEMORY_TYPE_AVAILABLE: return "Available";
        case MEMORY_TYPE_RESERVED: return "Reserved";
        case MEMORY_TYPE_ACPI_RECLAIMABLE: return "ACPI Reclaimable";
        case MEMORY_TYPE_ACPI_NVS: return "ACPI NVS";
        case MEMORY_TYPE_BAD: return "Bad Memory";
        default: return "Unknown";
    }
}

static void print_memory_map(void) {
    // Read the number of entries (first 2 bytes at MEMORY_MAP_ADDR)
    uint16_t entry_count = *(uint16_t*)MEMORY_MAP_ADDR;
    
    kprintf("\n=== Memory Map ===\n");
    kprintf("Found %d memory regions:\n", entry_count);
    
    // Get pointer to first entry (starts at offset 4)
    memory_map_entry_t* entries = (memory_map_entry_t*)(MEMORY_MAP_ADDR + 4);
    
    for (int i = 0; i < entry_count; i++) {
        kprintf("Region %d: \n    Base: 0x%x; Length: 0x%x; Type: %s; Attrs: 0x%x\n", i,
                entries[i].base_addr,
                entries[i].length,
                get_memory_type_string(entries[i].type),
                entries[i].extended_attrs);
    }
    kprintf("==================\n\n");
}

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

    if(check_apic()){
        kputs("+--------------------------------------+");
        kputs("|Hello from the kernel! APIC IS allowed|");
        kputs("+--------------------------------------+\n");
    }else{
        kputs("+--------------------------------------+");
        kputs("Hello from the kernel! APIC NOT allowed");
        kputs("+--------------------------------------+\n");
    }  

    // Print the memory map
    
    // for(;;) {
    //     asm("hlt");
    // }


    // init memory and jump to higher half kernel
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

    print_memory_map();

    init_vfs();
    kprintf(">: ");
    
    for(;;) {
        asm("hlt");
    }
}