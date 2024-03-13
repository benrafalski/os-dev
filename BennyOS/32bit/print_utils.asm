; 
; BIOS is 16-bit only so we need a new print string for 32-bit protected mode
; 
[bits 32]
; Constants
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

; background colors
BLACK_BGD equ 0x00
BLUE_BGD equ 0x10
GREEN_BGD equ 0x20
CYAN_BGD equ 0x30
RED_BGD equ 0x40
PURPLE_BGD equ 0x50
BROWN_BGD equ 0x60
GRAY_BGD equ 0x70
DARK_GRAY_BGD equ 0x80
LIGHT_BLUE_BGD equ 0x90
LIGHT_GREEN_BGD equ 0xA0
LIGHT_CYAN_BGD equ 0xB0
LIGHT_RED_BGD equ 0xC0
LIGHT_PURPLE_BGD equ 0xD0
YELLOW_BGD equ 0xE0
WHITE_BGD equ 0xF0

; foreground colors
BLACK_FGD equ 0x00
BLUE_FGD equ 0x01
GREEN_FGD equ 0x02
CYAN_FGD equ 0x03
RED_FGD equ 0x04
PURPLE_FGD equ 0x05
BROWN_FGD equ 0x06
GRAY_FGD equ 0x07
DARK_GRAY_FGD equ 0x08
LIGHT_BLUE_FGD equ 0x09
LIGHT_GREEN_FGD equ 0x0A
LIGHT_CYAN_FGD equ 0x0B
LIGHT_RED_FGD equ 0x0C
LIGHT_PURPLE_FGD equ 0x0D
YELLOW_FGD equ 0x0E
WHITE_FGD equ 0x0F

; prints string from ebx
print_string_32:
    pusha
    mov edx, VIDEO_MEMORY
    xor ch, ch
    mov ch, BLACK_BGD
    or ch, WHITE_FGD

print_string_pm_loop:
    mov al, [ebx]
    mov ah, ch
    ; mov ah, 0x0f

    cmp al, 0
    je print_string_pm_done

    mov [edx], ax

    add ebx, 1
    add edx, 2

    jmp print_string_pm_loop

print_string_pm_done:
    popa
    ret