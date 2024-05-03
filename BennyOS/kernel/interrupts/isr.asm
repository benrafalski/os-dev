%macro pushagrd 0
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%endmacro

%macro popagrd 0
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
%endmacro

%macro pushacrd 0
mov rax, cr0
push rax
mov rax, cr2
push rax
mov rax, cr3
push rax
mov rax, cr4
push rax
%endmacro

%macro popacrd 0
pop rax
mov cr4, rax
pop rax
mov cr3, rax
pop rax
mov cr2, rax
pop rax
mov cr0, rax
%endmacro

%macro stub_prework 0
push rbp
mov rbp, rsp
pushagrd
pushacrd
mov ax, ds
push rax
push qword 0
lea rdi, [rsp + 0x10]
%endmacro

%macro stub_postwork 0
pop rax
pop rax
mov ds, ax
mov es, ax
popacrd
popagrd
pop rbp
add rsp, 0x10
iretq
%endmacro


%macro isr_stub_no_err 1
isr_stub_%+%1:
    push 0
    push %1
    stub_prework
    call exception_handler
    stub_postwork 
%endmacro


%macro isr_stub_err 1
isr_stub_%+%1:
    push %1
    stub_prework
    call exception_handler
    stub_postwork 
%endmacro

isr_stub_no_err 0 ; 0x26f0
isr_stub_no_err 1
isr_stub_no_err 2
isr_stub_no_err 3
isr_stub_no_err 4
isr_stub_no_err 5
isr_stub_no_err 6
isr_stub_no_err 7
isr_stub_err 8
isr_stub_no_err 9
isr_stub_err 10
isr_stub_err 11
isr_stub_err 12
isr_stub_err 13
isr_stub_err 14
isr_stub_no_err 15
isr_stub_no_err 16
isr_stub_err 17
isr_stub_no_err 18
isr_stub_no_err 19
isr_stub_no_err 20
isr_stub_err 21
isr_stub_no_err 22
isr_stub_no_err 23
isr_stub_no_err 24
isr_stub_no_err 25
isr_stub_no_err 26
isr_stub_no_err 27
isr_stub_no_err 28
isr_stub_err 29
isr_stub_err 30
isr_stub_no_err 31

isr_stub_32:
    mov rax, 32
    call exception_handler
    iretq

isr_stub_33:
    push 0
    push 33
    stub_prework
    call exception_handler_33
    stub_postwork

extern exception_handler_33
extern exception_handler

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    34 
    dq isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1
%endrep