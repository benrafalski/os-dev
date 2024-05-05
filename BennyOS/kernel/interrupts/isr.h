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


// FAULTS: $rip will contain the value of the instruction that caused the exception
// TRAPS: $rip will contain the value of the instruction after the instruction that caused the exception

#define X86_DE_FAULT 0      // divide error fault
#define X86_DB_FAULT_TRAP 1 // debug fault/trap
#define X86_NMI_INT 2       // non-maskable interrupt
#define X86_BP_TRAP 3       // breakpoint trap
#define X86_OF_TRAP 4       // overflow trap
#define X86_BR_FAULT 5      // bound range exceeded fault
#define X86_UD_FAULT 6      // invalid opcode fault
#define X86_NM_FAULT 7      // device not available fault
#define X86_DF_ABORT 8      // double fault abort
#define X86_TS_FAULT 10     // invalid TSS fault
#define X86_NP_FAULT 11     // segment not present fault
#define X86_SS_FAULT 12     // stack-segment fault fault
#define X86_GP_FAULT 13     // general protection fault 
#define X86_PF_FAULT 14     // page fault fault 
#define X86_MF_FAULT 16     // x87 floating-point exception fault 
#define X86_AC_FAULT 17     // alignment check fault
#define X86_MC_ABORT 18     // machine check abort
#define X86_XM_FAULT 19     // SMID floating-point exception fault
#define X86_VE_FAULT 20     // virtualization exception fault
#define X86_CP_FAULT 21     // control protection exception fault
#define X86_HV_FAULT 28     // hypervisor injection exception fault
#define X86_VC_FAULT 29     // VMM communication exception fault
#define X86_SX_FAULT 30     // security exception fault 

void halt(){
    while (true)
    {
        __asm__ volatile("cli; hlt");
    }
}

void do_divide_zero(isr_frame_t *frame);
void do_divide_zero(isr_frame_t *frame){
    print_exception_msg("Division Error (cannot divide by 0 using DIV or IDIV). Halting...");
}

void do_debug(isr_frame_t *frame);
void do_debug(isr_frame_t *frame){
    print_exception_msg("Debug. Halting...");
    
}

void do_nmi(isr_frame_t *frame);
void do_nmi(isr_frame_t *frame){
    print_exception_msg("Non-Maskable Interrupt. Halting...");
    
}

void do_breakpoint(isr_frame_t *frame);
void do_breakpoint(isr_frame_t *frame){
    print_exception_msg("Breakpoint (INT3 has been executed). Halting...");
    
}

void do_overflow(isr_frame_t *frame);
void do_overflow(isr_frame_t *frame){
    print_exception_msg("Overflow (INTO instruction with RFLAGS.OVERFLOW==1). Halting...");
    
}

void do_bound_range_exceeded(isr_frame_t *frame);
void do_bound_range_exceeded(isr_frame_t *frame){
    print_exception_msg("Bound Range Exceeded (Error during BOUND instruction). Halting...");
    
}

void do_invalid_opcode(isr_frame_t *frame);
void do_invalid_opcode(isr_frame_t *frame){
    print_exception_msg("Invalid Opcode (undefined opcode has been executed). Halting...");
    
}


void do_device_not_available(isr_frame_t *frame);
void do_device_not_available(isr_frame_t *frame){
    print_exception_msg("Device Not Available (FPU instruction but there is no FPU). Halting...");
    
}

void do_double_fault(isr_frame_t *frame);
void do_double_fault(isr_frame_t *frame){
    print_exception_msg("Double Fault (exception occured during another exception handler). Halting...");
    
}

void do_invalid_tss(isr_frame_t *frame);
void do_invalid_tss(isr_frame_t *frame){
    print_exception_msg("Invalid TSS (invalid segment during task switch). Halting...");
    
}

void do_segment_not_present(isr_frame_t *frame);
void do_segment_not_present(isr_frame_t *frame){
    print_exception_msg("Segment Not Present (segment/gate loaded with PRESENT==0). Halting...");
    
}

void do_stack_segment_fault(isr_frame_t *frame);
void do_stack_segment_fault(isr_frame_t *frame){
    print_exception_msg("Stack Segment Fault (multiple reasons for this). Halting...");
    
}

void do_general_protection_fault(isr_frame_t *frame);
void do_general_protection_fault(isr_frame_t *frame){
    print_exception_msg("General Protection Fault (multiple reasons for this). Halting...");
    
}



void do_page_fault(isr_frame_t *frame);
void do_page_fault(isr_frame_t *frame){
    //  31              15                             4               0
    // +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
    // |   Reserved   | SGX |   Reserved   | SS | PK | I | R | U | W | P |
    // +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
    uint64_t error = frame->base_frame.error_code;
    // uint64_t cr2 = frame->control_registers.cr2;

    set_color(RED_FGD, DARK_GRAY_BGD);
    char temp1[4];
    char* cr2_address = citoa(frame->control_registers.cr2, temp1, 16);
    char temp2[24 + strlen(cr2_address)];
    char* output_message = strcat("Page Fault at address 0x", cr2_address, temp2);
    // print_exception_msg(output_message);

    // set_color(LIGHT_GREEN_FGD, DARK_GRAY_BGD);

    // return;


    if(error & 1){
        // When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
        char* e = " (page protection violation). Halting...";
        char temp3[strlen(output_message) + strlen(e)];
        char* error_message = strcat(output_message, e, temp3);
        print_exception_msg(error_message);
    }else{
        char* e = " (page not present). Halting...";
        char temp3[strlen(output_message) + strlen(e)];
        char* error_message = strcat(output_message, e, temp3);
        print_exception_msg(error_message);

        // print_exception_msg("Page Fault (page not present). Halting...");
    }
    // if(error & (1 << 1)){
    //     // When set, the page fault was caused by a write access. When not set, it was caused by a read access.
    //     print_exception_msg("Page Fault (write access violation). Halting...");
    // }else{
    //     print_exception_msg("Page Fault (read access violation). Halting...");
    // }
    // if(error & (1 << 2)){
    //     // When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
    //     print_exception_msg("Page Fault (CPL == 3). Halting...");
    // }
    // if(error & (1 << 3)){
    //     // When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.
    //     print_exception_msg("Page Fault (page directory entry contains reserved bits == 1). Halting...");
    // }
    // if(error & (1 << 4)){
    //     // When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.
    //     print_exception_msg("Page Fault (instruction fetch when NX enabled). Halting...");
    // }
    // if(error & (1 << 5)){
    //     // When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights.
    //     print_exception_msg("Page Fault (protection key violation). Halting...");
    // }
    // if(error & (1 << 6)){
    //     // When set, the page fault was caused by a shadow stack access.
    //     print_exception_msg("Page Fault (shadow stack access). Halting...");
    // }
    // if(error & (1 << 15)){
    //     // When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging.
    //     print_exception_msg("Page Fault (SGX violation). Halting...");
    // }
}



void do_x87_floating_point_exception(isr_frame_t *frame);
void do_x87_floating_point_exception(isr_frame_t *frame){
    print_exception_msg("x87 Floating Point Exception (FWAIT/WAIT executed with CR0.NE==1 && an ummasked #MF is pending). Halting...");    
}



void do_alignment_check(isr_frame_t *frame);
void do_alignment_check(isr_frame_t *frame){
    print_exception_msg("Alignment Check (do to unaligned memory data reference). Halting...");    
}

void do_machine_check(isr_frame_t *frame);
void do_machine_check(isr_frame_t *frame){
    print_exception_msg("Machine Check (processor detected internal error). Halting...");    
}

void do_smid_floating_point_exception(isr_frame_t *frame);
void do_smid_floating_point_exception(isr_frame_t *frame){
    print_exception_msg("SMID Floating-Point Exception (unmasked 128-bit media float error occured). Halting...");    
}


void do_virtualization_exception(isr_frame_t *frame);
void do_virtualization_exception(isr_frame_t *frame){
    print_exception_msg("Virtualization Exception. Halting...");    
}

void do_control_protection_exception(isr_frame_t *frame);
void do_control_protection_exception(isr_frame_t *frame){
    print_exception_msg("Control Protection Exception. Halting...");    
}

void do_hypervisor_injection_exception(isr_frame_t *frame);
void do_hypervisor_injection_exception(isr_frame_t *frame){
    print_exception_msg("Hypervisor Injection Exception. Halting...");    
}

void do_vmm_communication_exception(isr_frame_t *frame);
void do_vmm_communication_exception(isr_frame_t *frame){
    print_exception_msg("VMM Communication Exception. Halting...");    
}

void do_security_exception(isr_frame_t *frame);
void do_security_exception(isr_frame_t *frame){
    print_exception_msg("Security Exception. Halting...");    
}

// Keyboard Interrupt
void exception_handler_33(isr_frame_t *frame);
void exception_handler_33(isr_frame_t *frame)
{
    read_key();
    pic_send_eoi(1);
}

void print_base_frame(isr_frame_t *frame){
    //     struct
    // {
    //     uint64_t rbp;
    //     uint64_t vector;
    //     uint64_t error_code;
    //     uint64_t rip;
    //     uint64_t cs;
    //     uint64_t rflags;
    //     uint64_t rsp;
    //     uint64_t dss;
    // } base_frame;
    // uint8_t color = get_color();
    set_color(YELLOW_FGD, DARK_GRAY_BGD);
    kprintf("INT%d ", frame->base_frame.vector);
    kprintf(" (error code 0x%x)\n", frame->base_frame.error_code);
    kprintf("RIP: 0x%x\n", frame->base_frame.rip);
    kprintf("RBP: 0x%x ", frame->base_frame.rbp);
    kprintf("; RSP: 0x%x\n", frame->base_frame.rsp);
    kprintf("CS: 0x%x ", frame->base_frame.cs);
    kprintf("; DSS: 0x%x ", frame->base_frame.dss);
    kprintf("; RFLAGS: 0x%x\n", frame->base_frame.rflags);
    set_color(LIGHT_GREEN_FGD, DARK_GRAY_BGD);
    // print_exception_msg("here");
    
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

    case X86_BP_TRAP:
        do_breakpoint(frame);
        break;

    case X86_OF_TRAP:
        do_overflow(frame);
        break;

    case X86_BR_FAULT:
        do_bound_range_exceeded(frame);
        break;

    case X86_UD_FAULT:
        do_invalid_opcode(frame);
        break;

    case X86_NM_FAULT:
        do_device_not_available(frame);
        break;

    case X86_DF_ABORT:
        do_double_fault(frame);
        break;

    case X86_TS_FAULT:
        do_invalid_tss(frame);
        break;

    case X86_NP_FAULT:
        do_segment_not_present(frame);
        break;

    case X86_SS_FAULT:
        do_stack_segment_fault(frame);
        break;

    case X86_GP_FAULT:
        do_general_protection_fault(frame);
        break;

    case X86_PF_FAULT:
        do_page_fault(frame);
        break;

    case X86_MF_FAULT:
        do_x87_floating_point_exception(frame);
        break;

    case X86_AC_FAULT:
        do_alignment_check(frame);
        break;

    case X86_MC_ABORT:
        do_machine_check(frame);
        break;

    case X86_XM_FAULT:
        do_smid_floating_point_exception(frame);
        break;

    case X86_VE_FAULT:
        do_virtualization_exception(frame);
        break;

    case X86_CP_FAULT:
        do_control_protection_exception(frame);
        break;

    case X86_HV_FAULT:
        do_hypervisor_injection_exception(frame);
        break;

    case X86_VC_FAULT:
        do_vmm_communication_exception(frame);
        break;

    case X86_SX_FAULT:
        do_security_exception(frame);
        break;

    default:
        print_exception_msg("Undefined interrupt has been executed. Halting...");
        break;
    }

    print_base_frame(frame);

    halt();
    for(;;) {
        asm("hlt");
    }
}


// Invalid Opcode
//      The Invalid Opcode exception occurs when the processor tries to execute an invalid or undefined opcode, or an instruction with invalid prefixes. It also occurs in other cases, such as:
//          The instruction length exceeds 15 bytes, but this only occurs with redundant prefixes.
//          The instruction tries to access a non-existent control register (for example, mov cr6, eax).
//          The UD instruction is executed.
//      The saved instruction pointer points to the instruction which caused the exception.
// void exception_handler_6(isr_frame_t *frame);
// void exception_handler_6(isr_frame_t *frame)
// {
//     print_exception_msg("Exception 6 caught (Fault): Invalid Opcode interrupt, continuing...\n");
//     frame->base_frame.rip += 1;
// }


// // General Protection Fault
// //      A General Protection Fault may occur for various reasons. The most common are:
// //          Segment error (privilege, type, limit, read/write rights).
// //          Executing a privileged instruction while CPL != 0.
// //          Writing a 1 in a reserved register field or writing invalid value combinations (e.g. CR0 with PE=0 and PG=1).
// //          Referencing or accessing a null-descriptor.
// //          Accessing a memory address with bits 48-63 not matching bit 47 (e.g. 0x_0000_8000_0000_0000 instead of 0x_ffff_8000_0000_0000) in 64 bit mode.
// //      The saved instruction pointer points to the instruction which caused the exception.
// //      Error code: The General Protection Fault sets an error code, which is the segment selector index when the exception is segment related. Otherwise, 0.
// void exception_handler_13(isr_frame_t *frame);
// void exception_handler_13(isr_frame_t *frame)
// {
//     char str[20];
//     char* hex = citoa(frame->base_frame.rip, str, 16);
//     kputs("\nRIP:");
//     kputs(hex);
    
//     hex = citoa(frame->base_frame.error_code, str, 16);
//     kputs("\nError code:");
//     kputs(hex);

//     print_exception_msg("Exception 13 caught (Fault): general protection fault interrupt, halting...\n");
//     // frame->base_frame.rip += 1;
//     while (true)
//     {
//         __asm__ volatile("cli; hlt");
//     }

// }

// // Page Fault
// //      A Page Fault occurs when:
// //          A page directory or table entry is not present in physical memory.
// //          Attempting to load the instruction TLB with a translation for a non-executable page.
// //          A protection check (privileges, read/write) failed.
// //          A reserved bit in the page directory or table entries is set to 1.
// //      The saved instruction pointer points to the instruction which caused the exception.
// void exception_handler_14(isr_frame_t *frame);
// void exception_handler_14(isr_frame_t *frame)
// {
//     char temp1[4];
//     char* cr2_address = citoa(frame->control_registers.cr2, temp1, 16);
//     char temp2[54 + strlen(cr2_address)];
//     char* output_message = strcat("\nException 14 caught (Fault): page fault at address 0x", cr2_address, temp2);
//     print_exception_msg(output_message);
//     // The Page Fault sets an error code:
//     //  31              15                             4               0
//     // +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
//     // |   Reserved   | SGX |   Reserved   | SS | PK | I | R | U | W | P |
//     // +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
//     uint64_t error = frame->base_frame.error_code;

//     if(error & 1){
//         // When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
//         print_exception_msg("     Page present");
//     }else{
//         print_exception_msg("     Page not present");
//     }
//     if(error & (1 << 1)){
//         // When set, the page fault was caused by a write access. When not set, it was caused by a read access.
//         print_exception_msg("     Write access");
//     }else{
//         print_exception_msg("     Read access");
//     }
//     if(error & (1 << 2)){
//         // When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
//        print_exception_msg("\nUSER error. ");
//     }
//     if(error & (1 << 3)){
//         // When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.
//        print_exception_msg("\nRESERVED WRITE error. ");
//     }
//     if(error & (1 << 4)){
//         // When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.
//        print_exception_msg("\nINSTRUCTION FETCH error. ");
//     }
//     if(error & (1 << 5)){
//         // When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights.
//        print_exception_msg("\nPROTECTION KEY error. ");
//     }
//     if(error & (1 << 6)){
//         // When set, the page fault was caused by a shadow stack access.
//        print_exception_msg("\nSHADOW STACK error. ");
//     }
//     if(error & (1 << 15)){
//         // When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging.
//        print_exception_msg("\nSGX error. ");
//     }
//     // frame->base_frame.rip += 1;
//     __asm__ volatile("cli; hlt");
// }



