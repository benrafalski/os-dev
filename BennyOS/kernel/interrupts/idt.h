#include <stdint.h>
#include "errors.h"

#define IDT_MAX_DESCRIPTORS 256
// INT_GATE     -> P=0b1, DPL=0b00, TYPE=0x1110
// TRAP_GATE    -> P=0b1, DPL=0b00, TYPE=0x1111
#define INT_GATE        0xE
#define TRAP_GATE       0xF
#define PRESENT         (1 << 7)
#define NOT_PRESENT     (0 << 7)
#define DPL_0           (0 << 5)
#define DPL_1           (1 << 5)
#define DPL_2           (2 << 5)
#define DPL_3           (3 << 5)


// Interrupt Gate Descriptor
// |#################################################################|
// |127------------------------------------------------------------96|
// |-----------------------------------------------------------------|
// |Reserved                                                         |
// |-----------------------------------------------------------------|
// |96-------------------------------------------------------------64|
// |-----------------------------------------------------------------|
// |Offset                                                           |
// |63                                                             32|
// |-----------------------------------------------------------------|
// |63----------------------48|47|46-45|44|43-----40|39----35|34---32|
// |-----------------------------------------------------------------|
// |Offset                    |P |DPL  |0 |Gate Type|Reserved|IST    |
// |31                      16|  |1   0|  |3       0|        |2     0|
// |-----------------------------------------------------------------|
// |31----------------------16|15-----------------------------------0|
// |-----------------------------------------------------------------|
// |Segment Selector          |Offset                                |
// |15                       0|15                                   0|
// |#################################################################|
//      Offset: 64-bit address of the entry point of the ISR
//      Selector: segment selector that must point to a valid code segment in the GDT
//      IST: not used if 0, offset into the Interrupt Stack Table stored in the TSS
//      Gate Type: 0xE for interrupt gate and 0xF for trap gate
//      DPL: privilege level, can be 0 for kernel and 3 for userspace
//      P: present bit, must be 1 for the entry to be valid   
typedef struct
{
    uint16_t isr_low;   // The lower 16 bits of the ISR's offset
    uint16_t kernel_cs; // The GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t ist;        // The IST in the TSS that the CPU will load into RSP; set to zero for now
    uint8_t attributes; // Type and attributes; see the IDT page
    uint16_t isr_mid;   // The higher 16 bits of the lower 32 bits of the ISR's offset
    uint32_t isr_high;  // The higher 32 bits of the ISR's offset
    uint32_t reserved;  // Set to zero
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x1000))) static idt_entry_t idt[IDT_MAX_DESCRIPTORS]; // Create an array of IDT entries; aligned for performance

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

// define an IDTR
static idtr_t idtr;