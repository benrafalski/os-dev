[bits 64]
VIDEO_MEMORY equ 0xb8000

clear_screen_64:
    push rax
    push rdx
    push rcx
    mov edi, VIDEO_MEMORY         ; Set the destination index to 0xB8000.
    mov rax, 0x1F201F201F201F20   ; Set the A-register to 0x1F201F201F201F20.
    mov ecx, 500                  ; Set the C-register to 500.
    rep stosq                     ; Clear the screen.
    pop rcx
    pop rdx
    pop rax
    ret


