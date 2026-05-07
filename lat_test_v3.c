#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "hft_queue.h"

static inline unsigned long long rdtsc_safe(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("mfence" ::: "memory"); 
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    int fd;
    void *mmap_ptr;
    struct hft_ring *ring;
    int samples = 0;
    const int MAX_SAMPLES = 1000;

    FILE *csv = fopen("latency_data.csv", "w");
    if (!csv) { perror("csv open failed"); return 1; }
    fprintf(csv, "sample,latency\n");

    // 1. Initialize Kernel Subsystem
    syscall(548, 555);
    syscall(548, 123);

    fd = open("/dev/hft", O_RDWR);
    if (fd < 0) { perror("open /dev/hft failed"); return 1; }

    // 2. Map the full Ring Buffer size (16KB for 1024 slots)
    mmap_ptr = mmap(NULL, sizeof(struct hft_ring), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_ptr == MAP_FAILED) { perror("mmap failed"); return 1; }
    
    ring = (struct hft_ring *)mmap_ptr;

    // 3. THE FIX: Sync Head to Tail
    // This ignores any packets the kernel generated while the app was starting up
    printf("Syncing with kernel... skipping stale packets.\n");
    ring->head = ring->tail;

    printf("Logging %d samples to latency_data.csv...\n", MAX_SAMPLES);

    while (samples < MAX_SAMPLES) {
        // 4. Busy-poll until the kernel moves the tail
        while (ring->head == ring->tail) {
            __asm__ __volatile__ ("pause" ::: "memory"); 
        }

        // 5. Packet arrived! Capture time immediately
        unsigned long long user_receive_time = rdtsc_safe();
        
        // Get the message at the current head
        struct hft_msg *msg = &ring->buffer[ring->head];
        unsigned long long kernel_time = msg->kernel_ts;

        // 6. Record Latency
        if (kernel_time != 0) {
            unsigned long long latency = user_receive_time - kernel_time;
            fprintf(csv, "%d,%llu\n", samples, latency);
            samples++;
        }

        // 7. Move the head forward (Consumer moves Head, Producer moves Tail)
        ring->head = (ring->head + 1) & (RING_SIZE - 1);
    }

    fclose(csv);
    printf("Done. Everything is captured correctly.\n");
    return 0;
}