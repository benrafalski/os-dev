; Interrupts like INT 13h can't be used in long mode

;=============================================================================
; ATA read sectors (LBA mode) 
;
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
;
; @return None
;=============================================================================

global ata_lba_read
ata_lba_read:
               pushfq
               and rax, 0x0FFFFFFF
               push rax
               push rbx
               push rcx
               push rdx
               push rdi

                mov rax, rdi
                mov rcx, rsi
                mov rdi, rdx


               mov rbx, rax         ; Save LBA in RBX
               
               mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
               shr eax, 24          ; Get bit 24 - 27 in al
               or al, 11100000b     ; Set bit 6 in al for LBA mode
               out dx, al

               mov edx, 0x01F2      ; Port to send number of sectors
               mov al, cl           ; Get number of sectors from CL
               out dx, al
               
               mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
               mov eax, ebx         ; Get LBA from EBX
               out dx, al

               mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
               mov eax, ebx         ; Get LBA from EBX
               shr eax, 8           ; Get bit 8 - 15 in AL
               out dx, al


               mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
               mov eax, ebx         ; Get LBA from EBX
               shr eax, 16          ; Get bit 16 - 23 in AL
               out dx, al

               mov edx, 0x1F7       ; Command port
               mov al, 0x20         ; Read with retry.
               out dx, al

.still_going_lba_read:  in al, dx
               test al, 8           ; the sector buffer requires servicing.
               jz .still_going_lba_read      ; until the sector buffer is ready.

               mov rax, 256         ; to read 256 words = 1 sector
               xor bx, bx
               mov bl, cl           ; read CL sectors
               mul bx
               mov rcx, rax         ; RCX is counter for INSW
               mov rdx, 0x1F0       ; Data port, in and out
               rep insw             ; in to [RDI]

               pop rdi
               pop rdx
               pop rcx
               pop rbx
               pop rax
               popfq
               ret


;=============================================================================
; ATA read sectors (CHS mode) 
; Max head index is 15, giving 16 possible heads
; Max cylinder index can be a very large number (up to 65535)
; Sector is usually always 1-63, sector 0 reserved, max 255 sectors/track
; If using 63 sectors/track, max disk size = 31.5GB
; If using 255 sectors/track, max disk size = 127.5GB
; See OSDev forum links in bottom of [http://wiki.osdev.org/ATA]
;
; @param EBX The CHS values; 2 bytes, 1 byte (BH), 1 byte (BL) accordingly
; @param CH The number of sectors to read
; @param RDI The address of buffer to put data obtained from disk               
;
; @return None
;=============================================================================
global ata_chs_read
ata_chs_read:   pushfq
                push rax
                push rbx
                push rcx
                push rdx
                push rdi
                mov rax, rdi
                mov rcx, rsi
                mov rdi, rdx
                
                mov rdx,1f6h            ;port to send drive & head numbers
                mov al,bh               ;head index in BH
                and al,00001111b        ;head is only 4 bits long
                or  al,10100000b        ;default 1010b in high nibble
                out dx,al

                mov rdx,1f2h            ;Sector count port
                mov al,ch               ;Read CH sectors
                out dx,al

                mov rdx,1f3h            ;Sector number port
                mov al,bl               ;BL is sector index
                out dx,al

                mov rdx,1f4h            ;Cylinder low port
                mov eax,ebx             ;byte 2 in ebx, just above BH
                mov cl,16
                shr eax,cl              ;shift down to AL
                out dx,al

                mov rdx,1f5h            ;Cylinder high port
                mov eax,ebx             ;byte 3 in ebx, just above byte 2
                mov cl,24
                shr eax,cl              ;shift down to AL
                out dx,al

                mov rdx,1f7h            ;Command port
                mov al,20h              ;Read with retry.
                out dx,al
                
.still_going_chs_read:   in al,dx
                test al,8               ;the sector buffer requires servicing.
                jz .still_going_chs_read         ;until the sector buffer is ready.

                mov rax,512/2           ;to read 256 words = 1 sector
                xor bx,bx
                mov bl,ch               ;read CH sectors
                mul bx
                mov rcx,rax             ;RCX is counter for INSW
                mov rdx,1f0h            ;Data port, in and out
                rep insw                ;in to [RDI]

                pop rdi
                pop rdx
                pop rcx
                pop rbx
                pop rax
                popfq
                ret


;=============================================================================
; ATA write sectors (LBA mode) 
;
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to write
; @param RDI The address of data to write to the disk
;
; @return None
;=============================================================================
global ata_lba_write
ata_lba_write:
    pushfq
    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    
    mov rax, rdi    ; Move LBA to rax 
    mov rcx, rsi    ; Move sector count to rcx  
    mov rdi, rdx    ; Move buffer address to rdi

    mov rbx, rax    ; Save LBA in RBX

    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al

    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al

    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al


    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov edx, 0x1F7       ; Command port
    mov al, 0x30         ; Write with retry.
    out dx, al

.still_going_lba_write:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going_lba_write      ; until the sector buffer is ready.

    mov rax, 256         ; to write 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; write CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for OUTSW
    mov rdx, 0x1F0       ; Data port, in and out
    mov rsi, rdi
    rep outsw            ; out

    ; Wait for write completion
    mov edx, 0x1F7       ; Command/Status port
.wait_write_complete:
    in al, dx            ; Read status
    test al, 0x80        ; Check BSY (busy) bit
    jnz .wait_write_complete  ; Wait until not busy
    
    ; Optional: Flush cache to ensure data reaches disk
    mov al, 0xE7         ; Cache flush command
    out dx, al
.wait_flush:
    in al, dx            ; Read status
    test al, 0x80        ; Check BSY bit  
    jnz .wait_flush      ; Wait for flush to complete

    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret

; .still_going_lba_write:  in al, dx
;     test al, 8           ; the sector buffer requires servicing.
;     jz .still_going_lba_write      ; until the sector buffer is ready.

;     mov rax, 256         ; to read 256 words = 1 sector
;     xor bx, bx
;     mov bl, cl           ; write CL sectors
;     mul bx
;     mov rcx, rax         ; RCX is counter for OUTSW
;     mov rdx, 0x1F0       ; Data port, in and out
;     mov rsi, rdi
;     rep outsw            ; out

;     pop rdi
;     pop rdx
;     pop rcx
;     pop rbx
;     pop rax
;     popfq
;     ret