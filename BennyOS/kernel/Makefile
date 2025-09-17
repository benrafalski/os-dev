HEADERS = $(wildcard ./print/*.h ./interrupts/*.h ./memory/*.h ./fs*.h)

all: ../build/disk.img


# Create disk image
../build/disk.img: ../bin/boot1.bin ../bin/boot2.bin ../bin/kernel.bin
    # Create empty disk image
	dd if=/dev/zero of=$@ bs=1M count=10
    
    # Write MBR (stage1)
	dd if=../bin/boot1.bin of=$@ bs=512 count=1 conv=notrunc
    
    # Write stage2 starting at sector 2
	dd if=../bin/boot2.bin of=$@ bs=512 seek=1 conv=notrunc
    
    # Set up ext2 filesystem starting at a safe offset sector 100
    # Then copy kernel to the filesystem
	cd ../build && ./mkdsk.sh

../bin/os-image: ../bin/boot1.bin ../bin/boot2.bin ../bin/kernel.bin
	cat ../bin/boot1.bin ../bin/boot1.bin ../bin/kernel.bin > $@

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

# ../bin/boot_sector.bin: ../boot/new_boot.asm
# 	nasm $< -f bin -o $@ 

../bin/boot1.bin: ../boot/boot1.asm
	nasm $< -f bin -o $@ 

../bin/boot2.bin: ../boot/boot2.asm
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
