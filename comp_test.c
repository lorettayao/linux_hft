#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

// 1. 定義 RDTSC 函數，用來抓取精確的 CPU Cycle
static inline unsigned long long rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    unsigned long long u_start, u_end;
    void *ptr;
    int fd;

    // --- A. 準備階段 ---
    // 呼叫 Syscall 548, 555 來確保核心設備已註冊
    syscall(548, 555);
    
    fd = open("/dev/hft", O_RDWR);
    if (fd < 0) {
        printf("Error: Could not open /dev/hft. Did you mknod?\n");
        return 1;
    }

    ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    printf("Starting Benchmark...\n\n");

    // --- B. 測試 Syscall 延遲 ---
    u_start = rdtsc();
    for(int i=0; i<1000; i++) {
        // 執行一個最簡單的 syscall 呼叫 (mode 1)
        syscall(548, 1);
    }
    u_end = rdtsc();
    printf("1. Average Syscall latency: %llu cycles\n", (u_end - u_start) / 1000);

    // --- C. 測試 Shared Memory 讀取延遲 (HFT 方案) ---
    volatile char *data_ptr = (char *)ptr; 
    u_start = rdtsc();
    for(int i=0; i<1000; i++) {
        // 直接從記憶體讀取一個位元組
        char val = data_ptr[0];
    }
    u_end = rdtsc();
    printf("2. Average Shared Memory latency: %llu cycles\n", (u_end - u_start) / 1000);

    printf("\nNote: The gap represents the HFT optimization benefit.\n");

    munmap(ptr, 4096);
    close(fd);
    return 0;
}
