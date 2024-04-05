[org 0x7c00]
; global kernel_entry
; [bits 16]
KERNEL_OFFSET equ 0x1000

mov [BOOT_DRIVE], dl

; init the stack
mov bp, 0x8000
mov sp, bp

call load_kernel

; jmp $

call switch_to_32

jmp $ ; Hang

; Include files
; %include "16bit/print_utils.asm"
%include "../boot/16bit/disk_load.asm"
%include "../boot/32bit/gdt.asm"
%include "../boot/32bit/switch_to_32bit.asm"
; %include "../boot/32bit/print_utils.asm"
%include "../boot/32bit/setup_paging.asm"
%include "../boot/64bit/switch_to_64bit.asm"
%include "../boot/64bit/gdt.asm"
%include "../boot/64bit/print_utils.asm"

[bits 16]
load_kernel:
; load the kernel at offset 77000
    ; mov ax, 0x5000
    ; mov es, ax
    mov bx, KERNEL_OFFSET
    mov dh, 20  ; issue... it wasn't loading the rest of the kernel...
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret

[bits 32]
BEGIN_PM:

    call switch_to_64
    jmp $

; Use 64-bit.
[BITS 64]
BEGIN_LM:
    call clear_screen

    ; mov rax, 0xffffffff00000000
    ; mov rbx, [rax]

    ; hlt

    mov rax, 0x0
    mov rbx, 0x1000
.fill_table:
    mov rcx, rbx
    or rcx, 3
    mov rdx, 0xA000
    mov rsi, 510
    mov rdi, 0xB003
    mov [rdx+rsi*8], rdi
    mov rdx, 0xB000
    mov rsi, 0
    mov rdi, 0xC003
    mov [rdx+rsi*8], rdi
    mov rdx, 0xC000
    mov rsi, 0
    mov [rdx+rsi*8], rcx
    add rbx, 0x1000
    inc rax
    cmp rax, 1024
    jne .fill_table

; [extern map_page]

;     mov rdi, 0x1000
;     mov rsi, 0xFFFFFFFF80000000
;     call map_page

    ; call 0xFFFFFFFF80000000
    call KERNEL_OFFSET
    ; jmp 59000h

    jmp $

    hlt                           ; Halt the processor.




;  mov eax, 0x0
;  mov ebx, 0x100000
;  .fill_table:
;       mov ecx, ebx
;       or ecx, 3
;       mov [table_768+eax*4], ecx
;       add ebx, 4096
;       inc eax
;       cmp eax, 1024
;       jne .fill_table
; mov rax, 0xFFFFFFFF80000000
; 0xFFFFFFFF80001000












; Data
BOOT_DRIVE db 0x0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55