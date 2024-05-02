#include <stdint.h>
#include "../print/keyboard.h"
#include "pic.h"



typedef struct
{
    struct
    {
        uint64_t cr4;
        uint64_t cr3;
        uint64_t cr2;
        uint64_t cr0;
    } control_registers;

    struct
    {
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t r11;
        uint64_t r10;
        uint64_t r9;
        uint64_t r8;
        uint64_t rdi;
        uint64_t rsi;
        uint64_t rdx;
        uint64_t rcx;
        uint64_t rbx;
        uint64_t rax;
    } general_registers;

    struct
    {
        uint64_t rbp;
        uint64_t vector;
        uint64_t error_code;
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t dss;
    } base_frame;
} isr_frame_t;

typedef struct
{
    uint8_t scan_code;
    uint8_t key;
} scancode_t;

#define X86_DE_FAULT 0      // divide error fault
#define X86_DB_FAULT_TRAP 1 // debug fault/trap
#define X86_NMI_INT 2       // non-maskable interrupt

void halt(){
    while (true)
    {
        __asm__ volatile("cli; hlt");
    }
}


void do_divide_zero(isr_frame_t *frame);
void do_divide_zero(isr_frame_t *frame){
    print_exception_msg("Division Error (cannot divide by 0 using DIV or IDIV). Halting...");
    // kprintf("$rip: 0x%x", frame->base_frame.rip);
    halt();
}


void do_debug(isr_frame_t *frame);
void do_debug(isr_frame_t *frame){
    print_exception_msg("Debug. Halting...");
    halt();
    
}

void do_nmi(isr_frame_t *frame);
void do_nmi(isr_frame_t *frame){
    print_exception_msg("Non-Maskable Interrupt. Halting...");
    halt();
    
}


__attribute__((noreturn)) void exception_handler(isr_frame_t *frame);
void exception_handler(isr_frame_t *frame)
{
    uint64_t vector = frame->base_frame.vector;

    switch (vector)
    {
    case X86_DE_FAULT:
        do_divide_zero(frame);
        break;

    case X86_DB_FAULT_TRAP:
        do_debug(frame);
        break;

    case X86_NMI_INT:
        do_nmi(frame);
        break;
    
    default:
        break;
    }


    while (true)
    {
        __asm__ volatile("cli; hlt");
    }
}



// FAULTS: $rip will contain the value of the instruction that caused the exception
// TRAPS: $rip will contain the value of the instruction after the instruction that caused the exception

// // Debug (Trap)
// //      Instruction fetch breakpoint (Fault)
// //      General detect condition (Fault)
// //      Data read or write breakpoint (Trap)
// //      I/O read or write breakpoint (Trap)
// //      Single-step (Trap)
// //      Task-switch (Trap)
// // void exception_handler_1(isr_frame_t *frame);
// // void exception_handler_1(isr_frame_t *frame)
// // {
// //     print_exception_msg("Exception 1 caught (trap): debug interrupt, continuing...\n");
// // }

// // https://wiki.osdev.org/Non_Maskable_Interrupt
// void exception_handler_2(isr_frame_t *frame);
// void exception_handler_2(isr_frame_t *frame)
// {
//     print_exception_msg("Exception 2 caught (interrupt): non-maskable interrupt, continuing...\n");
// }

// Invalid Opcode
//      The Invalid Opcode exception occurs when the processor tries to execute an invalid or undefined opcode, or an instruction with invalid prefixes. It also occurs in other cases, such as:
//          The instruction length exceeds 15 bytes, but this only occurs with redundant prefixes.
//          The instruction tries to access a non-existent control register (for example, mov cr6, eax).
//          The UD instruction is executed.
//      The saved instruction pointer points to the instruction which caused the exception.
void exception_handler_6(isr_frame_t *frame);
void exception_handler_6(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}


// General Protection Fault
//      A General Protection Fault may occur for various reasons. The most common are:
//          Segment error (privilege, type, limit, read/write rights).
//          Executing a privileged instruction while CPL != 0.
//          Writing a 1 in a reserved register field or writing invalid value combinations (e.g. CR0 with PE=0 and PG=1).
//          Referencing or accessing a null-descriptor.
//          Accessing a memory address with bits 48-63 not matching bit 47 (e.g. 0x_0000_8000_0000_0000 instead of 0x_ffff_8000_0000_0000) in 64 bit mode.
//      The saved instruction pointer points to the instruction which caused the exception.
//      Error code: The General Protection Fault sets an error code, which is the segment selector index when the exception is segment related. Otherwise, 0.
void exception_handler_13(isr_frame_t *frame);
void exception_handler_13(isr_frame_t *frame)
{
    char str[20];
    char* hex = citoa(frame->base_frame.rip, str, 16);
    kputs("\nRIP:");
    kputs(hex);
    
    hex = citoa(frame->base_frame.error_code, str, 16);
    kputs("\nError code:");
    kputs(hex);

    print_exception_msg("Exception 13 caught (Fault): general protection fault interrupt, halting...\n");
    // frame->base_frame.rip += 1;
    while (true)
    {
        __asm__ volatile("cli; hlt");
    }

}

// Page Fault
//      A Page Fault occurs when:
//          A page directory or table entry is not present in physical memory.
//          Attempting to load the instruction TLB with a translation for a non-executable page.
//          A protection check (privileges, read/write) failed.
//          A reserved bit in the page directory or table entries is set to 1.
//      The saved instruction pointer points to the instruction which caused the exception.
void exception_handler_14(isr_frame_t *frame);
void exception_handler_14(isr_frame_t *frame)
{
    char temp1[4];
    char* cr2_address = citoa(frame->control_registers.cr2, temp1, 16);
    char temp2[54 + strlen(cr2_address)];
    char* output_message = strcat("\nException 14 caught (Fault): page fault at address 0x", cr2_address, temp2);
    print_exception_msg(output_message);
    // The Page Fault sets an error code:
    //  31              15                             4               0
    // +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
    // |   Reserved   | SGX |   Reserved   | SS | PK | I | R | U | W | P |
    // +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
    uint64_t error = frame->base_frame.error_code;

    if(error & 1){
        // When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
        print_exception_msg("     Page present");
    }else{
        print_exception_msg("     Page not present");
    }
    if(error & (1 << 1)){
        // When set, the page fault was caused by a write access. When not set, it was caused by a read access.
        print_exception_msg("     Write access");
    }else{
        print_exception_msg("     Read access");
    }
    if(error & (1 << 2)){
        // When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
       print_exception_msg("\nUSER error. ");
    }
    if(error & (1 << 3)){
        // When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.
       print_exception_msg("\nRESERVED WRITE error. ");
    }
    if(error & (1 << 4)){
        // When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.
       print_exception_msg("\nINSTRUCTION FETCH error. ");
    }
    if(error & (1 << 5)){
        // When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights.
       print_exception_msg("\nPROTECTION KEY error. ");
    }
    if(error & (1 << 6)){
        // When set, the page fault was caused by a shadow stack access.
       print_exception_msg("\nSHADOW STACK error. ");
    }
    if(error & (1 << 15)){
        // When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging.
       print_exception_msg("\nSGX error. ");
    }
    // frame->base_frame.rip += 1;
    __asm__ volatile("cli; hlt");
}

void exception_handler_7(isr_frame_t *frame);
void exception_handler_7(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_8(isr_frame_t *frame);
void exception_handler_8(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_9(isr_frame_t *frame);
void exception_handler_9(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_10(isr_frame_t *frame);
void exception_handler_10(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_11(isr_frame_t *frame);
void exception_handler_11(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_12(isr_frame_t *frame);
void exception_handler_12(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_15(isr_frame_t *frame);
void exception_handler_15(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_16(isr_frame_t *frame);
void exception_handler_16(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_17(isr_frame_t *frame);
void exception_handler_17(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_18(isr_frame_t *frame);
void exception_handler_18(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_19(isr_frame_t *frame);
void exception_handler_19(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_20(isr_frame_t *frame);
void exception_handler_20(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_21(isr_frame_t *frame);
void exception_handler_21(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_22(isr_frame_t *frame);
void exception_handler_22(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_23(isr_frame_t *frame);
void exception_handler_23(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_24(isr_frame_t *frame);
void exception_handler_24(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_25(isr_frame_t *frame);
void exception_handler_25(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_26(isr_frame_t *frame);
void exception_handler_26(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_27(isr_frame_t *frame);
void exception_handler_27(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_28(isr_frame_t *frame);
void exception_handler_28(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_29(isr_frame_t *frame);
void exception_handler_29(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_30(isr_frame_t *frame);
void exception_handler_30(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

void exception_handler_31(isr_frame_t *frame);
void exception_handler_31(isr_frame_t *frame)
{
    print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
    frame->base_frame.rip += 1;
}

// Keyboard Interrupt
void exception_handler_33(isr_frame_t *frame);
void exception_handler_33(isr_frame_t *frame)
{
    read_key();
    pic_send_eoi(1);
}

