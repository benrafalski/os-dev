HEADERS = $(wildcard ./print/*.h ./interrupts/*.h ./memory/*.h)

all: ../bin/os-image

../bin/os-image: ../bin/boot_sector.bin ../bin/kernel.bin
	cat $^ > $@
# ld -o $@ -Ttext 0xFFFFFFFF80000000 $^ --oformat binary
../bin/kernel.bin: ../bin/kernel_entry.o ../bin/kernel.o ../bin/isr.o
	ld -o $@ -T linker.ld $^ --oformat binary

../bin/kernel.o: kmain.c ${HEADERS}
	gcc -g -ffreestanding -c $< -o $@ 

../bin/kernel_entry.o: kernel_entry.asm
	nasm $< -f elf64 -o $@

../bin/isr.o: ./interrupts/isr.asm
	nasm $< -f elf64 -o $@

../bin/boot_sector.bin: ../boot/new_boot.asm
	nasm $< -f bin -o $@ 

clean:
	rm -fr ../bin/*.bin ../bin/*.o ../bin/os-image