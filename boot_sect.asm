
[org 0x7c00] ; Tell the assembler where this code will be loaded


jmp $ ; Hang

%include "print_string.asm"
%include "print_hex.asm"
%include "disk_load.asm"

; Data

; Padding and magic number.
times 510-($-$$) db 0
dw 0xaa55
