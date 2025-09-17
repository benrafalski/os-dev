[org 0x7c00]
[bits 16]

STAGE2_OFFSET equ 0x7e00  ; Load stage2 right after MBR
STAGE2_SECTORS equ 10     ; Adjust based on stage2 size

mov [BOOT_DRIVE], dl

; Set up stack
mov bp, 0x7c00
mov sp, bp

; Load stage2 from disk
mov bx, STAGE2_OFFSET     ; Where to load boot2
mov dh, STAGE2_SECTORS    ; How many sectors
mov dl, [BOOT_DRIVE]      ; Drive number
mov cl, 2                 ; Start from sector 2 (after MBR)

call disk_load

; Jump to stage2
jmp STAGE2_OFFSET

%include "../boot/16bit/print_utils.asm"
%include "../boot/16bit/disk_load.asm"

BOOT_DRIVE db 0
stage2_loading_msg db 'Loading stage 2...', 0x0D, 0x0A, 0

; padding and magic number
times 510-($-$$) db 0
dw 0xaa55