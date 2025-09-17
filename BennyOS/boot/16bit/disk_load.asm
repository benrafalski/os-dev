disk_load:
    pusha
    
    ; Uses LBA
    ; Parameters coming in:
    ; bx = memory address to load to
    ; dh = number of sectors to read  
    ; dl = drive number
    ; cl = starting sector number

    mov ah, 0x02        ; BIOS read sectors function
    mov al, dh          ; number of sectors to read
    mov ch, 0           ; cylinder 0
    mov dh, 0           ; head 0
    
    int 0x13            ; call BIOS
    
    jc disk_error       ; jump if error
    
    popa
    ret

disk_error:
    mov si, DISK_ERR_MSG
    call print_string
    cli 
    hlt

DISK_ERR_MSG: db "Disk read error!", 0x0D, 0x0A, 0