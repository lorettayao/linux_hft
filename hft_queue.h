#ifndef HFT_QUEUE_H
#define HFT_QUEUE_H

#define RING_SIZE 1024  // Must be power of 2 for fast wrapping
#define CACHE_LINE 64

struct hft_msg {
    unsigned long long kernel_ts;
    unsigned long long dummy_data; 
};

struct hft_ring {
    // Producer (Kernel) updates this. Aligned to avoid False Sharing.
    unsigned long tail __attribute__((aligned(CACHE_LINE)));
    
    // Consumer (User) updates this.
    unsigned long head __attribute__((aligned(CACHE_LINE)));
    
    // The buffer itself
    struct hft_msg buffer[RING_SIZE] __attribute__((aligned(CACHE_LINE)));
};

static inline unsigned long long rdtsc_safe(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("mfence" ::: "memory"); 
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

#endif