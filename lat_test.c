#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

/**
 * RDTSC - Read Time Stamp Counter
 * Grabs the current CPU cycle count with nanosecond-level precision.
 */
static inline unsigned long long rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    int fd;
    void *mmap_ptr;
    // We use volatile to prevent the compiler from optimizing away the loop
    volatile unsigned long long *shared_data; 
    unsigned long long last_val = 0;
    int packet_count = 0;

    printf("--- HFT Latency Extension: Real-Time Packet Simulator ---\n");

    // 1. Initialize the Kernel Side (Mode 555 for device, Mode 123 for Timer)
    if (syscall(548, 555) != 0) {
        perror("Syscall 555 (Device Init) failed");
        return 1;
    }
    
    if (syscall(548, 123) != 0) {
        perror("Syscall 123 (Timer Start) failed");
        return 1;
    }
    printf("[System] Kernel Device and Timer initialized.\n");

    // 2. Open and Map the Shared Memory
    fd = open("/dev/hft", O_RDWR);
    if (fd < 0) {
        printf("Error: Cannot open /dev/hft (errno: %d). Did you mknod?\n", errno);
        return 1;
    }

    mmap_ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_ptr == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return 1;
    }

    shared_data = (unsigned long long *)mmap_ptr;
    printf("[System] Shared memory mapped at %p. Starting Polling...\n", mmap_ptr);
    printf("Press Ctrl+C to stop.\n\n");

    // 3. The "Hot Loop" (HFT Core Logic)
    // This loop consumes 100% CPU on one core to ensure 0ns wakeup latency.
    while (packet_count < 20) {
        unsigned long long current_kernel_time = *shared_data;

        // Check if the kernel has updated the timestamp
        if (current_kernel_time != last_val && current_kernel_time != 0) {
            unsigned long long user_receive_time = rdtsc();
            unsigned long long latency = user_receive_time - current_kernel_time;

            packet_count++;
            printf("[%d] Packet Arrived! Kernel Ticks: %llu | Latency: %llu cycles\n", 
                    packet_count, current_kernel_time, latency);

            last_val = current_kernel_time;
        }
    }

    printf("\nBenchmark Finished. Summary: Captured %d packets via Zero-Copy Polling.\n", packet_count);

    munmap(mmap_ptr, 4096);
    close(fd);
    return 0;
}