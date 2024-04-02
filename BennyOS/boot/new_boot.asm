[org 0x7c00]
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
%include "../boot/32bit/print_utils.asm"
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
    mov dh, 15
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


    call KERNEL_OFFSET
    ; jmp 59000h

    jmp $

    hlt                           ; Halt the processor.

; Data
BOOT_DRIVE db 0x0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55