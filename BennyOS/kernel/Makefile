HEADERS = $(wildcard ./print/*.h ./interrupts/*.h ./memory/*.h ./fs*.h)

all: ../bin/os-image

# gcc ./fs/makefs.c -o out
# 	./out
../bin/os-image: ../bin/boot_sector.bin ../bin/kernel.bin
	cat ../bin/boot_sector.bin ../bin/kernel.bin > $@

# ld -o $@ -Ttext 0xFFFFFFFF80000000 $^ --oformat binary
../bin/kernel.bin: ../bin/kernel_entry.o ../bin/kernel.o ../bin/isr.o ../bin/ata.o
	ld -o $@ -T linker.ld $^ --oformat binary

../bin/kernel.o: kmain.c ${HEADERS}
	gcc -g -ffreestanding -c $< -o $@ 

../bin/kernel_entry.o: kernel_entry.asm
	nasm $< -f elf64 -o $@

../bin/ata.o: ./fs/ata.asm
	nasm $< -f elf64 -o $@

../bin/isr.o: ./interrupts/isr.asm
	nasm $< -f elf64 -o $@

../bin/boot_sector.bin: ../boot/new_boot.asm
	nasm $< -f bin -o $@ 

clean:
	rm -fr ../bin/*.bin ../bin/*.o ../bin/os-image

run: 
	qemu-system-x86_64 -drive format=raw,file="../build/disk.img"

debug:
	qemu-system-x86_64 -drive format=raw,file="../build/disk.img",index=0,media=disk -s -S 

monitor:
	qemu-system-x86_64 -drive format=raw,file="../bin/os-image",index=0,media=disk -monitor telnet:127.0.0.1:55555,server,nowait

# make clean && make && cd ../build/; sudo ./mkdsk.sh && cd ../kernel; make run
