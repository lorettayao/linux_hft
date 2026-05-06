#!/bin/bash
# 1. Pack
cd ~/hft-sandbox/rootfs
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz
cd ..

# 2. Run
# this qemu command helps isolating cpu
# but inside when u run u still have to use testcase 0x2 to make sure u use the isolated CPU
qemu-system-x86_64 \
    -m 1G -smp 2 \
    -kernel ~/linux/arch/x86/boot/bzImage \
    -initrd initramfs.cpio.gz \
    -nographic \
    -append "console=ttyS0 nokaslr iomem=relaxed isolcpus=1 nohz_full=1 rcu_nocbs=1"