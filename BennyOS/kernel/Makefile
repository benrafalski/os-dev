all: ../bin/os-image

../bin/os-image: ../bin/boot_sector.bin ../bin/kernel.bin
	cat $^ > $@

../bin/kernel.bin: ../bin/kernel.o
	ld -o $@ -Ttext 0x9000 $^ --oformat binary

../bin/kernel.o: kmain.c ./print/colors.h ./print/print.h
	gcc -ffreestanding -c $< -o $@ 

# kernel_entry.o: kernel_entry.asm
# 	nasm $< -f elf -o $@

../bin/boot_sector.bin: ../boot/new_boot.asm
	nasm $< -f bin -o $@ 

clean:
	rm -fr ../bin/*.bin ../bin/*.o ../bin/os-image