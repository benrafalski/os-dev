HEADERS = $(wildcard *.h)

all: os-image

os-image: boot_sect.bin kernel.bin
	cat $^ > $@

kernel.bin: kernel_entry.o kernel.o
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.o: kernel.c ${HEADERS}
	gcc -fno-pie -m32 -ffreestanding -c $< -o $@ 

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

boot_sect.bin: boot_sect.asm
	nasm $< -f bin -o $@ 

clean:
	rm -fr *.bin *.o os-image

