#!/bin/bash

# Create 10MB disk image
dd if=/dev/zero of=disk.img bs=1M count=10

# Write stage1 bootloader (MBR) to sector 0
dd if=../bin/boot1.bin of=disk.img bs=512 count=1 conv=notrunc

# Write stage2 bootloader starting at sector 1
dd if=../bin/boot2.bin of=disk.img bs=512 seek=1 conv=notrunc

# Reserve sectors 0-99 for bootloaders and other boot data
# Start ext2 filesystem at sector 100 (51.2KB offset)
FILESYSTEM_START_SECTOR=100
FILESYSTEM_OFFSET=$((FILESYSTEM_START_SECTOR * 512))

# Create ext2 filesystem starting at the offset
# Use loopback device with offset
LOOP_DEVICE=$(sudo losetup -f --show -o $FILESYSTEM_OFFSET disk.img)

# Format as ext2 (remaining space after offset)
sudo mkfs.ext2 $LOOP_DEVICE

# Mount the filesystem
mkdir -p /tmp/bennyos_mount
sudo mount $LOOP_DEVICE /tmp/bennyos_mount

# Copy kernel to the filesystem
sudo cp ../bin/kernel.bin /tmp/bennyos_mount/

# Create any other directories/files needed
sudo mkdir -p /tmp/bennyos_mount/boot
sudo mkdir -p /tmp/bennyos_mount/etc

# Unmount and cleanup
sudo umount /tmp/bennyos_mount
sudo losetup -d $LOOP_DEVICE
rmdir /tmp/bennyos_mount

echo "Disk image created successfully!"
echo "Stage1 bootloader: sectors 0"
echo "Stage2 bootloader: sectors 1-$((($(stat -c%s ../bin/boot2.bin) + 511) / 512))"
echo "Ext2 filesystem starts at sector $FILESYSTEM_START_SECTOR"
































# # old
# # make disk image, 128MB is size, copy 1048576 bytes at a time
# dd if=/dev/zero of=disk.img bs=1048576 count=128
# # fdisk disk.img <<EOF 
# # n
# # p
# # 1


# # a
# # p
# # w
# # EOF

# # losetup -o $[2048*512] --sizelimit $[8*1024*1024] /dev/loop3 disk.img
# # mke2fs -b 1024 disk.img
# mke2fs disk.img
# losetup /dev/loop3 disk.img
# losetup -a
# file -sL disk.img
# file -sL /dev/loop3
# fdisk -l /dev/loop3
# mkdir mdir
# mount /dev/loop3 mdir
# cp ../bin/kernel.bin mdir/
# # cp test.txt mdir/
# mkdir mdir/first
# mkdir mdir/first/second
# # echo -n "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." > test.txt
# echo -n "Lorem ipsum dolor sit amet, consectetur adipiscing elit." > test.txt
# cp test.txt mdir/first/second
# ls -la mdir
# umount mdir
# rm test.txt
# rm -r mdir
# dd if=../bin/boot_sector.bin of=disk.img conv=notrunc bs=446 count=1
# dd if=../bin/boot_sector.bin of=disk.img conv=notrunc bs=1 count=2 skip=510 seek=510
# losetup -d /dev/loop3




