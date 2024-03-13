detect_lm:
    mov eax, 0x80000000    ; Set the A-register to 0x80000000.
    cpuid                  ; CPU identification.
    cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
    jb .NoLongMode         ; It is less, there is no long mode.

    mov eax, 0x80000001    ; Set the A-register to 0x80000001.
    xor edx, edx
    cpuid                  ; CPU identification.
    test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
    jz .NoLongMode         ; They aren't, there is no long mode.

    mov ebx, YES_LONG
    call print_string_32
    ret

.NoLongMode:
    mov ebx, NO_LONG
    call print_string_32
    ret

NO_LONG: db "NO LONG MODE!!", 0
YES_LONG: db "YES LONG MODE!!", 0