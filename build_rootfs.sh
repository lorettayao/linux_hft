#!/bin/bash
# 編譯測試程式
gcc -static hft_test.c -o ./rootfs/bin/hft_test
gcc -static comp_test.c -o ./rootfs/bin/comp_test
# gcc -static lat_test.c -o ./rootfs/bin/lat_test
gcc -static -march=x86-64 lat_test_v2.c -o ./rootfs/bin/lat_test_v2
gcc -static -march=x86-64 lat_test_v3.c -o ./rootfs/bin/lat_test_v3

# 打包
cd rootfs
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz
echo "Initramfs updated!"
