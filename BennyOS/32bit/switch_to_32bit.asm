[bits 16]
; switch to protected mode
switch_to_32:
    cli                     ; disable interrupts
    lgdt [gdt_descriptor]   ; load GDT with the start address of the GDT

    mov eax, cr0            ; set Protection Enable bit in CR0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:init_32        ; perform far jump to selector 0x08

[bits 32]
init_32:
    ; load DS, ES, FS, GS, SS, ESP ...
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call BEGIN_PM