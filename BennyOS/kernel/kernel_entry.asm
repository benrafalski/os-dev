[bits 64]
[extern main]
global kernel_entry
kernel_entry:
    call main
    jmp $