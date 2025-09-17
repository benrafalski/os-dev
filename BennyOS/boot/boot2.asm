[org 0x7e00]
[bits 16]

; Simple test - just print a message and hang
call clear_screen
mov si, stage2_msg
call print_string

jmp $

%include "../boot/16bit/print_utils.asm"

stage2_msg db 'Stage 2 loaded successfully!', 0x0D, 0x0A, 0