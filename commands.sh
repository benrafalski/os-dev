# qemu-system-x86_64 -drive format=raw,file="D:\Programs\OS Stuff\os-dev\boot_sect.bin"
# nasm boot_sect.asm -f bin -o boot_sect.bin
# gcc -ffreestanding -c kernel.c -o kernel.o
# ld -o kernel.bin -Ttext 0x1000 kernel.o --oformat binary
# cat boot_sect.bin kernel.bin > os-image

nasm boot_sect.asm -f bin -o boot_sect.bin
nasm kernel_entry.asm -f elf -o kernel_entry.o
gcc -ffreestanding -c kernel.c -o kernel.o
ld -o kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o --oformat binary
cat boot sect.bin kernel.bin > os-image