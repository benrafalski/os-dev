[org 0x7c00]
[bits 16]
; init the stack
mov bp, 0x9000
mov sp, bp

call switch_to_32

jmp $ ; Hang

; Include files
%include "32bit/gdt.asm"
%include "32bit/switch_to_32bit.asm"
%include "32bit/print_utils.asm"
%include "32bit/setup_paging.asm"
%include "64bit/switch_to_64bit.asm"
%include "64bit/gdt.asm"
%include "64bit/print_utils.asm"

[bits 32]
BEGIN_PM:
    call switch_to_64

; Use 64-bit.
[BITS 64]
BEGIN_LM:
    call clear_screen
    ; print "Hello World!"
    mov edi, 0xb8000 
    mov rax, 0x1F6C1F6C1F651F48    
    mov [edi],rax
 
    mov rax, 0x1F6F1F571F201F6F
    mov [edi + 8], rax
 
    mov rax, 0x1F211F641F6C1F72
    mov [edi + 16], rax
    hlt                           ; Halt the processor.

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55