all: os-image

os-image: boot_sector.bin kernel.bin
	cat $^ > $@

kernel.bin: kernel.o
	ld -o $@ -Ttext 0x1000 $< --oformat binary

kernel.o: kernel.c 
	gcc -ffreestanding -c $< -o $@

boot_sector.bin: new_boot.asm
	nasm $< -f bin -o $@ 

clean:
	rm -fr *.bin *.o os-image