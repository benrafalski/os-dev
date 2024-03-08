print_string:
    mov cl, byte [bx]
    test cl, cl
    jz end
    mov ah, 0x0e
    mov al , byte [bx]
    int 0x10
    add bx, 1
    jmp print_string

print_hex:
    ; store the hex value in bx
    ; print '0x'
    mov ah, 0x0e    
    mov al , '0'
    int 0x10
    mov ah, 0x0e    
    mov al , 'x'
    int 0x10

    mov cx, bx
    ; print lower byte
    mov bl, bh
    and bh, 0xf0     ; bh = 0x10
    shr bh, 0x4     ; bh = 0x01
    and bl, 0x0f    ; bl = 0x09
    add bh, 0x30
    add bl, 0x30
    mov ah, 0x0e
    mov al, bh 
    int 0x10
    mov ah, 0x0e
    mov al, bl 
    int 0x10
    ; print higher byte
    mov bx, cx
    mov bh, bl
    and bh, 0xf0     ; bh = 0x10
    shr bh, 0x4     ; bh = 0x01
    and bl, 0x0f    ; bl = 0x09
    add bh, 0x30
    add bl, 0x30
    mov ah, 0x0e
    mov al, bh 
    int 0x10
    mov ah, 0x0e
    mov al, bl 
    int 0x10

    ret

print_hex_byte:
    mov ah, 0x0e    
    mov al , '0'
    int 0x10
    mov ah, 0x0e    
    mov al , 'x'
    int 0x10
    mov bh, bl
    and bh, 0xf0     ; bh = 0x10
    shr bh, 0x4     ; bh = 0x01
    and bl, 0x0f    ; bl = 0x09
    add bh, 0x30
    add bl, 0x30


    ; if bh > 0x39 or bl > 0x39 :
    ; FIX ME LATER

;     cmp bh, 0x39
;     jle print_clean_hex_byte
;     add bh, 8

;     mov ah, 0x0e
;     mov al, "X" 
;     int 0x10
    
;     cmp bl, 0x39
;     jle print_clean_hex_byte
;     add bl, 8

; print_clean_hex_byte:

    mov ah, 0x0e
    mov al, bh 
    int 0x10
    mov ah, 0x0e
    mov al, bl 
    int 0x10
    ; pop bx
    ret



end:
    ret
