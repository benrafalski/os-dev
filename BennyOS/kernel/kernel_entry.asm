[bits 64]
[extern main]
global kernel_entry
kernel_entry:
    ; hlt
    call main
    jmp $