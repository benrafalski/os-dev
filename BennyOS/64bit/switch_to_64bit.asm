[bits 32]
switch_to_64:
    call setup_paging

    lgdt [GDT64.Pointer]         ; Load the 64-bit global descriptor table.

    

[bits 64]
init_lm:
    cli                           ; Clear the interrupt flag.
    mov ax, GDT64.Data            ; Set the A-register to the data descriptor.
    mov ds, ax                    ; Set the data segment to the A-register.
    mov es, ax                    ; Set the extra segment to the A-register.
    mov fs, ax                    ; Set the F-segment to the A-register.
    mov gs, ax                    ; Set the G-segment to the A-register.
    mov ss, ax                    ; Set the stack segment to the A-register.
    ; mov edi, 0xB8000              ; Set the destination index to 0xB8000.
    mov rbp, 0x90000
    mov rsp, rbp
    call BEGIN_LM
