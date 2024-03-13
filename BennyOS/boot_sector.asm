[org 0x7c00]
[bits 16]
; init the stack
mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string

call switch_to_32

jmp $ ; Hang

; Include files
%include "16bit/print/print_utils.asm"
%include "32bit/gdt.asm"
%include "32bit/switch_to_32bit.asm"
%include "32bit/print_utils.asm"

[bits 32]

BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print_string_32

    jmp $ ; Hang

; Data
MSG_REAL_MODE: db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE: db "Successfully landed in 32-bit Protected Mode", 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55