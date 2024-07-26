#!/bin/bash
# make disk image, 128MB is size, copy 1048576 bytes at a time
dd if=/dev/zero of=disk.img bs=1048576 count=128
# fdisk disk.img <<EOF 
# n
# p
# 1


# a
# p
# w
# EOF

# losetup -o $[2048*512] --sizelimit $[8*1024*1024] /dev/loop3 disk.img
mke2fs disk.img
losetup /dev/loop3 disk.img
losetup -a
# mkfs.ext2 /dev/loop3
file -sL disk.img
file -sL /dev/loop3
fdisk -l /dev/loop3
mkdir mdir
mount /dev/loop3 mdir
# mount disk.img mdir
cp ../bin/kernel.bin mdir/
umount mdir
rm -r mdir
dd if=../bin/boot_sector.bin of=disk.img conv=notrunc bs=446 count=1
dd if=../bin/boot_sector.bin of=disk.img conv=notrunc bs=1 count=2 skip=510 seek=510
losetup -d /dev/loop3
# xxd disk.img | grep 8816
# 00411000: 8816 657d bd00 7e89 ece8 3d01 e835 00eb  ..e}..~...=..5..
# 00411010: feb4 41bb aa55 b280 cd13 7200 b40e b041  ..A..U....r....A
# 00411050: 010f 22c0 ea59 7c08 0066 b810 008e d88e  .."..Y|..f......
# 00411080: 2000 000f 22df 31c0 b900 1000 00f3 ab0f   ...".1.........
# 00411100: 0008 00fa 66b8 1000 8ed8 8ec0 8ee0 8ee8  ....f...........
# 00411150: 657d e8bc fec3 e895 ffff ffeb fee8 9e02  e}..............
# 00411200: e826 2300 00eb fef3 0f1e fa55 4889 e589  .&#........UH...
# 00411210: f866 8945 ec0f b745 ec89 c2ec 8845 ff0f  .f.E...E.....E..
# 00411220: b645 ff5d c3f3 0f1e fa55 4889 e589 f889  .E.].....UH.....
# 00411230: f266 8945 fc89 d088 45f8 0fb6 45f8 0fb7  .f.E....E...E...
# 00411240: 55fc ee90 5dc3 f30f 1efa 5548 89e5 89f8  U...].....UH....
# 00411250: 6689 45ec 0fb7 45ec 89c2 66ed 6689 45fe  f.E...E...f.f.E.




