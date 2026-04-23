#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_hft_init 548 // 剛才你在 tbl 裡面寫的編號

int main() {
    printf("Starting HFT test...\n");
    long ret = syscall(SYS_hft_init, 123);
    printf("System call returned: %ld\n", ret);
    return 0;
}
