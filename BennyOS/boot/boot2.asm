[org 0x7e00]
[bits 16]

KERNEL_OFFSET equ 0xa000
KERNEL_SECTORS equ 125  ; Number of sectors to load (adjust as needed)
KERNEL_SECTOR_START equ 820 ; Starting sector (LBA)
MEMORY_MAP_ADDR equ 0x9500  ; Store memory map here (after stack, before kernel)

mov [BOOT_DRIVE], dl

; init the stack
mov bp, 0x9000
mov sp, bp

call clear_screen
mov si, stage2_msg
call print_string

; Get memory map first
call get_memory_map

; Load kernel from disk
call load_kernel

mov si, kernel_loaded_msg
call print_string

mov si, switching_to_32_msg
call print_string

call switch_to_32

; Include files BEFORE 32-bit and 64-bit sections
%include "../boot/16bit/print_utils.asm"
%include "../boot/16bit/disk_load.asm"
%include "../boot/32bit/print_utils.asm"
%include "../boot/32bit/gdt.asm"
%include "../boot/32bit/switch_to_32bit.asm"
%include "../boot/32bit/setup_paging.asm"
%include "../boot/64bit/print_utils.asm"
%include "../boot/64bit/switch_to_64bit.asm"
%include "../boot/64bit/gdt.asm"

[bits 16]
get_memory_map:
    mov si, memory_map_msg
    call print_string
    
    mov di, MEMORY_MAP_ADDR + 4     ; Start storing entries after the count
    xor ebx, ebx                    ; EBX = 0 for first call
    mov edx, 0x534D4150             ; 'SMAP' signature
    xor bp, bp                      ; Entry count
    
.loop:
    mov eax, 0xe820                 ; Function code
    mov ecx, 24                     ; Entry size (24 bytes)
    int 0x15                        ; Call BIOS
    
    jc .done                        ; Carry flag = error or done
    
    ; Check if we got a valid entry
    cmp eax, 0x534D4150             ; Should return 'SMAP'
    jne .done
    
    ; Check if entry size is valid
    test ecx, ecx
    jz .skip_entry
    
    ; We got a valid entry
    add di, 24                      ; Move to next entry position
    inc bp                          ; Increment entry count
    
    ; Check if this is the last entry
    test ebx, ebx
    jz .done
    
    ; Continue if we have more entries
    jmp .loop
    
.skip_entry:
    ; Skip invalid entries but continue
    test ebx, ebx
    jnz .loop
    
.done:
    ; Store the entry count at the beginning
    mov [MEMORY_MAP_ADDR], bp
    
    ; Print how many entries we found
    mov si, found_entries_msg
    call print_string
    mov ax, bp
    call print_hex_word
    mov si, newline_msg
    call print_string
    
    ret

load_kernel:
    mov si, loading_kernel_msg
    call print_string
    
    mov bx, KERNEL_OFFSET           ; Load address
    mov dh, KERNEL_SECTORS          ; Number of sectors
    mov dl, [BOOT_DRIVE]            ; Drive number
    mov ax, KERNEL_SECTOR_START

    call disk_load         
    ret

; Helper function to print a word in hex
print_hex_word:
    push ax
    push cx
    
    mov cl, 8
    shr ax, cl          ; Get high byte
    call print_hex_byte
    
    pop cx
    pop ax
    and ax, 0x00FF      ; Get low byte
    call print_hex_byte
    ret

; Helper function to print a byte in hex
print_hex_byte:
    push ax
    push cx
    
    mov cl, 4
    shr al, cl          ; Get high nibble
    call print_hex_nibble
    
    pop cx
    pop ax
    and al, 0x0F        ; Get low nibble
    call print_hex_nibble
    ret

print_hex_nibble:
    cmp al, 9
    jle .digit
    add al, 'A' - 10
    jmp .print
.digit:
    add al, '0'
.print:
    mov ah, 0x0e
    int 0x10
    ret

[bits 32]
BEGIN_PM:
    call clear_screen_32
    mov ebx, my_string
    call print_string_32
    call switch_to_64

[bits 64]
BEGIN_LM:
    call clear_screen_64
    call KERNEL_OFFSET
    jmp $

; Data
BOOT_DRIVE db 0
stage2_msg db 'Stage 2 loaded successfully!', 0x0D, 0x0A, 0
memory_map_msg db 'Getting memory map...', 0x0D, 0x0A, 0
loading_kernel_msg db 'Loading kernel...', 0x0D, 0x0A, 0
kernel_loaded_msg db 'Kernel loaded!', 0x0D, 0x0A, 0
switching_to_32_msg db 'Switching to 32-bit mode...', 0x0D, 0x0A, 0
found_entries_msg db 'Found ', 0
newline_msg db ' memory map entries', 0x0D, 0x0A, 0
my_string db "Now in 32-bit Protected Mode!", 0