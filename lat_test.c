#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>

// static inline unsigned long long rdtscp(void) {
//     unsigned int lo, hi, aux;
//     __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (aux));
//     return ((unsigned long long)hi << 32) | lo;
// }

static inline unsigned long long rdtsc_safe(void) {
    unsigned int lo, hi;
    // 使用 mfence 確保前面的指令都執行完了，達到類似 rdtscp 的效果
    __asm__ __volatile__ ("mfence" ::: "memory"); 
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    int fd;
    void *mmap_ptr;
    volatile unsigned long long *shared_data;
    unsigned long long last_val = 0;
    int samples = 0;
    const int MAX_SAMPLES = 1000; // Collect 1k data points

    FILE *csv = fopen("latency_data.csv", "w");
    fprintf(csv, "sample,latency\n");

    // Init Syscalls
    syscall(548, 555);
    syscall(548, 123);

    fd = open("/dev/hft", O_RDWR);
    mmap_ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data = (unsigned long long *)mmap_ptr;

    printf("Logging 1k samples to latency_data.csv...\n");

    while (samples < MAX_SAMPLES) {
        unsigned long long current_kernel_time = *shared_data;
        if (current_kernel_time != last_val && current_kernel_time != 0) {
            unsigned long long user_receive_time = rdtsc_safe();
            if (last_val != 0) { // Skip the first "cold" sample
                fprintf(csv, "%d,%llu\n", samples, user_receive_time - current_kernel_time);
                samples++;
            }
            last_val = current_kernel_time;
        }
    }

    fclose(csv);
    printf("Done. Transfer this file to your host to plot.\n");
    return 0;
}