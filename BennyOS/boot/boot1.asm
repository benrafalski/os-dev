[org 0x7c00]
[bits 16]

STAGE2_OFFSET equ 0x7e00  ; Load stage2 right after MBR
STAGE2_SECTORS equ 10     ; Adjust based on stage2 size
STAGE2_START equ 1   ; Starting sector (LBA)

mov [BOOT_DRIVE], dl

; Set up stack
mov bp, 0x7c00
mov sp, bp

; load stage2 from disk
mov bx, STAGE2_OFFSET           ; Load address
mov dh, STAGE2_SECTORS          ; Number of sectors
mov dl, [BOOT_DRIVE]            ; Drive number
xor ax, ax
mov ax, STAGE2_START

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