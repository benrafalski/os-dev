; Simple string printing utility for 16-bit real mode
; Uses BIOS interrupt 0x10, function 0x0E (teletype output)
; Input: DS:SI points to null-terminated string
print_string:
    mov ah, 0x0e
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

clear_screen:
    mov ax, 0x0003      ; 80x25 text mode
    int 0x10
    ret