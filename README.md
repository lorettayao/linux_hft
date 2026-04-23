My apologies! I understand—you want the raw Markdown code in a **copy-pasteable code block** so the formatting remains intact when you save it as `README.md`.

Here is the complete project documentation in `.md` format:

```markdown
# HFT-Kernel-Sandbox: Low-Latency Zero-Copy Infrastructure

A high-performance Linux Kernel experiment focused on minimizing User-Kernel communication overhead. This project implements a custom system call and a shared-memory character device to achieve "Zero-Copy" data transfer—a critical requirement in High-Frequency Trading (HFT) systems where every nanosecond counts.

## The Goal
In HFT, standard Linux I/O operations (like `read`, `write`, or `recv`) introduce significant latency due to context switching and data copying between Kernel and User space. The objective of this project is to:
1. **Bypass the standard I/O path** using Memory Mapping.
2. **Implement a Zero-Copy architecture** via a custom Character Device.
3. **Quantify performance gains** using hardware-level CPU cycle measurements (`rdtsc`).

## Environment Settings
The project was developed and tested in a strictly controlled sandbox environment:
* **Host OS:** Ubuntu 22.04 (Build Server)
* **Target Kernel:** Linux 5.4.0 (Custom Build)
* **Architecture:** x86_64
* **Emulation:** QEMU (No-graphic, Serial Console)
* **Toolchain:** GCC (Static Linking), BusyBox (Initramfs)
* **Performance Tracking:** Hardware-level `rdtsc` (Time Stamp Counter)

## Implementation Details

### 1. Custom System Call (Entry 548)
Implemented a custom entry point in `kernel/sys.c` to serve as the control plane for the HFT system.
* **Mode 555:** Dynamically registers a `miscdevice` for shared memory access.
* **Mode 999:** Triggers a CPU "Tight Loop" (Busy Polling) to simulate the high-frequency polling used in trading engines to wait for incoming network packets without sleeping.

### 2. Zero-Copy Shared Memory (The "Fast Path")
Developed a Character Device (`/dev/hft`) that implements a custom `.mmap` operation.
* **Kernel Side:** Allocates a physical page using `__get_free_page`.
* **Mapping:** Uses `remap_pfn_range` to map the kernel-space physical buffer directly into the User-space virtual address space.
* **Efficiency:** Once mapped, User-space accesses data via a direct pointer, incurring **zero** system call overhead for data retrieval.

### 3. Precision Benchmarking
A specialized comparison tool (`comp_test.c`) was developed to measure the "Round-trip" cost of a system call versus the direct memory access provided by this infrastructure.

##  Benchmark Results
Actual measurements taken within the QEMU sandbox (Cycles represent CPU clock ticks).

| Communication Method | Average Latency (Cycles) | Mechanism |
| :--- | :--- | :--- |
| **Standard Syscall** | **2,198** | Context Switch + Trap + Logic |
| **Shared Memory** | **128** | Direct Pointer Access |
| **Performance Gain** | **~17x Faster** | **94% Latency Reduction** |

> **Conclusion:** The shared memory implementation successfully bypassed kernel overhead. This proves that for latency-critical applications, a specialized memory-mapped interface is far superior to traditional system calls.

---

##  Execution Guide

### 1. Compilation
**Kernel:**
```bash
cd ~/linux
make -j$(nproc) bzImage
```

**User-Space:**
```bash
gcc -static comp_test.c -o ./rootfs/bin/comp_test
```

### 2. Packaging & Launch
**Build Initramfs:**
```bash
cd ~/hft-sandbox/rootfs
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz
```

**Run QEMU:**
```bash
qemu-system-x86_64 \
    -kernel ~/linux/arch/x86/boot/bzImage \
    -initrd ~/hft-sandbox/initramfs.cpio.gz \
    -nographic \
    -append "console=ttyS0 nokaslr iomem=relaxed"
```

### 3. Testing (Inside QEMU)
```bash
# Identify Minor Number (usually 60)
cat /proc/misc | grep hft

# Create Node & Run
mknod /dev/hft c 10 60
comp_test
```
