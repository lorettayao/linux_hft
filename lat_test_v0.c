#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

// Precision TSC timing
static inline unsigned long long rdtsc_safe(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("mfence" ::: "memory"); 
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    int fd;
    unsigned long long kernel_ts, user_ts;
    int samples = 0;
    const int MAX_SAMPLES = 1000;

    FILE *csv = fopen("standard_lat.csv", "w");
    fprintf(csv, "sample,latency\n");

    // Initialize Subsystem (555: Init, 123: Start Timer)
    syscall(548, 555);
    syscall(548, 123);

    fd = open("/dev/hft", O_RDONLY);
    if (fd < 0) { perror("open /dev/hft failed"); return 1; }

    printf("Starting Standard I/O (Blocking Read) test: 1k samples...\n");

    while (samples < MAX_SAMPLES) {
        // This blocks! The OS puts this process to sleep.
        if (read(fd, &kernel_ts, sizeof(kernel_ts)) > 0) {
            user_ts = rdtsc_safe(); // Capture time immediately upon waking up
            
            fprintf(csv, "%d,%llu\n", samples, user_ts - kernel_ts);
            samples++;
            
            if (samples % 100 == 0) printf("Collected %d samples...\n", samples);
        }
    }

    fclose(csv);
    close(fd);
    printf("Standard results saved to latency_standard.csv\n");
    return 0;
}