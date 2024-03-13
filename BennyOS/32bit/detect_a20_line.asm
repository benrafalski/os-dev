[bits 32]
 
; Check A20 line
; Returns to caller if A20 gate is cleared.
; Continues to A20_on if A20 line is set.
; Written by Elad Ashkcenazi 
 
detect_a20_line:   
    pushad
    mov edi,0x112345  ;odd megabyte address.
    mov esi,0x012345  ;even megabyte address.
    mov [esi],esi     ;making sure that both addresses contain diffrent values.
    mov [edi],edi     ;(if A20 line is cleared the two pointers would point to the address 0x012345 that would contain 0x112345 (edi)) 
    cmpsd             ;compare addresses to see if the're equivalent.
    popad
    jne A20_on        ;if not equivalent , A20 line is set.

    mov ebx, A20_OFF
    call print_string_32

    ret               ;if equivalent , the A20 line is cleared.
 
A20_on:
; *your code from here*
    mov ebx, A20_ON
    call print_string_32
    ret


A20_OFF: db "A20 is OFF !!!", 0
A20_ON: db "A20 is ON !!!", 0
