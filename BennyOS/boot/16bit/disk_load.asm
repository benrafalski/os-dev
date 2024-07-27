disk_load:
    push dx
; starting head = 0x20
; starting sector = 33
; starting cylinder = 0

; addr 0x00411000 on disk -> LBA=0x2088 -> CHS=(8,4,13)
    ; BIOS read sector function

    ; kerenel = 0x00084c00 = LBA->1062
    mov ah, 0x02
    ; read dh sectors
    mov al, dh
    ; cylinder 0
    mov ch, 0x8
    ; head 0
    mov dh, 0x4
    ; start reading after 3nd sector (after the boot sector [0] and ext2 superblock [1])
    mov cl, 13
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




DISK_ERR_MSG: db "Disk read error!", 0


; disk_load:
;     mov ah, 0x41
;     mov bx, 0x55AA
;     mov dl, 0x80
;     int 0x13

;     jc disk_error ;

disk_error:
    ; mov bx, DISK_ERR_MSG
    ; call print_string
    ; INT 0x10, AH = 0xE -- display char
    mov ah, 0xE
    mov al, 0x41
    int 0x10
    cli 
    hlt
    jmp $