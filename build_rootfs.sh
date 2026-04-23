#!/bin/bash
# 編譯測試程式
gcc -static hft_test.c -o ./rootfs/bin/hft_test
gcc -static comp_test.c -o ./rootfs/bin/comp_tes

# 打包
cd rootfs
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz
echo "Initramfs updated!"
