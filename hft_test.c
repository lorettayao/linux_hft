#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

// 封裝一個抓取 CPU Ticks 的函數
static inline unsigned long long rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    unsigned long long u_start, u_end;
    
    printf("Measuring Syscall overhead...\n");
    
    u_start = rdtsc();
    // 呼叫你的 548 號 syscall，mode 傳 1 (正常模式)
    long ret = syscall(548, 1); 
    u_end = rdtsc();
    
    printf("User-side total cycles: %llu\n", u_end - u_start);
    return 0;
}
