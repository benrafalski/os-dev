disk_load:
    push dx

    ; BIOS read sector function
    mov ah, 0x02
    ; read dh sectors
    mov al, dh
    ; cylinder 0
    mov ch, 0x00
    ; head 0
    mov dh, 0x00
    ; start reading after 2nd sector (after the boot sector)
    mov cl, 0x02
    ; call BIOS disk read
    int 0x13
    ; if carry flag set print error message

    ; pop dx
    ; mov bx, dx
    ; call print_hex
    jc disk_error ; 0x7c1e

    ; check if dh (num sectors expected) == al (num sectors read)
    pop dx
    cmp dh, al

    jne disk_error
    ret


disk_error:
    ; mov bx, DISK_ERR_MSG
    ; call print_string

    jmp $

DISK_ERR_MSG: db "Disk read error!", 0