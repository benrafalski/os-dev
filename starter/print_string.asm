print_string:
    mov cl, byte [bx]
    test cl, cl
    jz end
    mov ah, 0x0e
    mov al , byte [bx]
    int 0x10
    add bx, 1
    jmp print_string

end:
    ret
