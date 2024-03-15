[org 0x7c00]
; [bits 16]
KERNEL_OFFSET equ 0x1000

mov [BOOT_DRIVE], dl

; init the stack
mov bp, 0x9000
mov sp, bp

; load the kernel
; mov bx, KERNEL_OFFSET
; mov dh, 15
; mov dl, [BOOT_DRIVE]
; call disk_load

; mov ax, KERNEL_OFFSET
; mov bx, 0x0
; mov [ax], bx
call load_kernel

jmp $

; call switch_to_32

jmp $ ; Hang

; Include files
%include "16bit/print_utils.asm"
%include "16bit/disk_load.asm"
; %include "../starter/disk_load.asm"

[bits 16]
load_kernel:
    ; mov bx, MSG_LOAD_KERNEL
    ; call print_string

    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret
; %include "32bit/gdt.asm"
; %include "32bit/switch_to_32bit.asm"
; %include "32bit/print_utils.asm"
; %include "32bit/setup_paging.asm"
; %include "64bit/switch_to_64bit.asm"
; %include "64bit/gdt.asm"
; %include "64bit/print_utils.asm"

; [bits 32]
; BEGIN_PM:


;     call KERNEL_OFFSET

;     jmp $

;     ; call switch_to_64

; ; Use 64-bit.
; [BITS 64]
; BEGIN_LM:
;     call clear_screen
;     ; print "Hello World!"
;     mov edi, 0xb8000 
;     mov rax, 0x1F6C1F6C1F651F48    
;     mov [edi],rax
 
;     mov rax, 0x1F6F1F571F201F6F
;     mov [edi + 8], rax
 
;     mov rax, 0x1F211F641F6C1F72
;     mov [edi + 16], rax


;     ; call KERNEL_OFFSET

;     jmp $

;     hlt                           ; Halt the processor.

; Data
BOOT_DRIVE db 0x0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55