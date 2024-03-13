; prints the byte stored in BX
print_string:
    mov cl, byte [bx]
    test cl, cl
    jz end_print_string
    mov ah, 0x0e
    mov al , byte [bx]
    int 0x10
    add bx, 1
    jmp print_string

end_print_string:
    ret

; prints the number stored in BX in hex
print_hex:
    ; store the hex value in bx
    ; print '0x'
    mov ah, 0x0e    
    mov al , '0'
    int 0x10
    mov ah, 0x0e    
    mov al , 'x'
    int 0x10

print_byte_1:
    mov cx, bx
    ; print lower byte
    mov bl, bh
    and bh, 0xf0     ; bh = 0x10
    shr bh, 0x4     ; bh = 0x01
    and bl, 0x0f    ; bl = 0x09
    add bh, 0x30
    add bl, 0x30

    cmp bh, 0x39
    jle checkbl_1
    add bh, 0x7
    
checkbl_1:
    cmp bl, 0x39
    jle clean_1
    add bl, 0x7

clean_1:
    mov ah, 0x0e
    mov al, bh 
    int 0x10
    mov ah, 0x0e
    mov al, bl 
    int 0x10

print_byte_2:
    ; print higher byte
    mov bx, cx
    mov bh, bl
    and bh, 0xf0     ; bh = 0x10
    shr bh, 0x4     ; bh = 0x01
    and bl, 0x0f    ; bl = 0x09
    add bh, 0x30
    add bl, 0x30

    cmp bh, 0x39
    jle checkbl_2
    add bh, 0x7
    
checkbl_2:
    cmp bl, 0x39
    jle clean_2
    add bl, 0x7

clean_2:
    mov ah, 0x0e
    mov al, bh 
    int 0x10
    mov ah, 0x0e
    mov al, bl 
    int 0x10
    ret