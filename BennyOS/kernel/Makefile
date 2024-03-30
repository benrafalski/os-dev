HEADERS = $(wildcard ./print/*.h ./interrupts/*.h)

all: ../bin/os-image

../bin/os-image: ../bin/boot_sector.bin ../bin/kernel.bin
	cat $^ > $@

../bin/kernel.bin: ../bin/kernel_entry.o ../bin/kernel.o ../bin/isr.o
	ld -o $@ -Ttext 0x9000 $^ --oformat binary

../bin/kernel.o: kmain.c ${HEADERS}
	gcc -ffreestanding -c $< -o $@ 

../bin/kernel_entry.o: kernel_entry.asm
	nasm $< -f elf64 -o $@

../bin/isr.o: ./interrupts/isr.asm
	nasm $< -f elf64 -o $@

../bin/boot_sector.bin: ../boot/new_boot.asm
	nasm $< -f bin -o $@ 

clean:
	rm -fr ../bin/*.bin ../bin/*.o ../bin/os-image