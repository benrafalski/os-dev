[org 0x7c00]
[bits 16]
; init the stack
mov bp, 0x9000
mov sp, bp

call switch_to_32

jmp $ ; Hang

; Include files
; %include "16bit/print_utils.asm"
%include "32bit/gdt.asm"
%include "32bit/switch_to_32bit.asm"
%include "32bit/print_utils.asm"
%include "32bit/setup_paging.asm"
%include "64bit/switch_to_64bit.asm"
%include "64bit/gdt.asm"


[bits 32]
BEGIN_PM:
    ; mov ebx, MSG_PROT_MODE
    ; call print_string_32
    call switch_to_64
    ; mov ebx, MSG_LONG_MODE
    ; call print_string_32
    jmp $ ; Hang

[bits 64]
BEGIN_LM:

    ; mov rbx, MSG_LONG_MODE
    ; call print_string_32
    ; mov rdi, 0xb8000
clear_screen:
    mov edi, 0xb8000
    mov rax, 0x0F210F210F680F69   ; Set the A-register to 0x1F201F201F201F20.
    ; mov rax, 0x0F680F69
    mov ecx, 2                  ; Set the C-register to 500.
    rep stosq                     ; Clear the screen.
    hlt                           ; Halt the processor.
    jmp $ ; Hang


; Data
MSG_REAL_MODE: db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE: db "Successfully landed in 32-bit Protected Mode", 0
MSG_LONG_MODE: db "Successfully landed in 64-bit Long Mode", 0


; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55