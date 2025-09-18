disk_load:
    pusha
    
    ; Save the original parameters before we overwrite them
    push dx
    push ax
    
    ; First, get the actual disk geometry
    call get_disk_geometry
    
    ; Convert LBA to CHS using detected geometry
    pop ax
    call lba_to_chs
    
    ; Set up CHS values for BIOS call
    pop dx
    mov ah, 0x02                    ; BIOS read sectors function
    mov al, dh          ; number of sectors (from saved value)
    mov ch, [cylinder]              ; cylinder
    mov cl, [sector]                ; sector
    mov dh, [head]                  ; head
    ; jmp $
    ; mov dl, [temp_drive]            ; drive number (from saved value)
    
    int 0x13                        ; call BIOS
    
    jc disk_error                   ; jump if error
    
    popa
    ret

get_disk_geometry:
    pusha
    
    ; Get drive parameters
    mov ah, 0x08                    ; Get drive parameters
    mov dl, [BOOT_DRIVE]            ; Drive number
    xor di, di                      ; ES:DI = 0000:0000 to guard against BIOS bugs
    int 0x13
    
    jc .geometry_error
    
    ; Extract geometry info
    mov [max_head], dh              ; Store max head (heads = max_head + 1)
    mov al, cl
    and al, 0x3F                    ; Mask out cylinder bits to get sectors
    mov [sectors_per_track], al     ; Store sectors per track
    
    popa
    ret
    
.geometry_error:
    ; Fall back to your known working values (16 heads, 63 sectors/track)
    mov byte [max_head], 15         ; 16 heads (max_head = heads - 1)
    mov byte [sectors_per_track], 63
    popa
    ret

lba_to_chs:
    pusha                           ; Save all registers
    
    ; Convert LBA 820 to CHS using 16-bit arithmetic
    ; AX contains 820
    
    ; Debug: Check if we have valid geometry
    cmp byte [sectors_per_track], 0
    je .error
    cmp byte [max_head], 0
    je .error
    
    ; Get sectors per track into BX
    movzx bx, byte [sectors_per_track]  ; Should be 63
    
    ; Safety check: make sure BX is not zero
    cmp bx, 0
    je .error
    
    ; Calculate sector: S = (LBA % sectors_per_track) + 1
    xor dx, dx              ; Clear DX for division
    div bx                  ; AX = quotient, DX = remainder
    
    ; Sector = remainder + 1
    inc dx                  
    cmp dx, 63              ; Sanity check: sector should be <= 63
    ja .error
    mov [sector], dl
    
    ; Now AX contains (LBA / sectors_per_track) = quotient
    ; Calculate head: H = quotient % heads
    movzx bx, byte [max_head]
    inc bx                  ; heads = max_head + 1
    
    ; Safety check: make sure BX is not zero
    cmp bx, 0
    je .error
    
    xor dx, dx              ; Clear DX for division
    div bx                  ; AX = cylinder, DX = head
    
    ; Sanity checks
    cmp dx, 16              ; Head should be < 16
    jae .error
    cmp ax, 1024            ; Cylinder should be reasonable
    jae .error
    
    mov [head], dl
    mov [cylinder], al
    
    popa                    ; Restore all registers
    ret

.error:
    ; Set fallback values that we know work
    mov si, FALLBACK_MSG
    call print_string
    mov byte [cylinder], 0
    mov byte [head], 13
    mov byte [sector], 2
    popa                    ; Restore all registers
    ret

; Variables for geometry
; Add these variables
temp_sectors db 0
temp_drive db 0
max_head db 0
sectors_per_track db 0
head db 0
sector db 0
cylinder db 0

disk_error:
    mov si, DISK_ERR_MSG
    call print_string
    cli 
    hlt

DISK_ERR_MSG: db "Disk read error!", 0x0D, 0x0A, 0
FALLBACK_MSG: db "Using fallback values.", 0x0D, 0x0A, 0