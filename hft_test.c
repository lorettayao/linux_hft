#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_hft_init 548 // 剛才你在 tbl 裡面寫的編號
int main() {
    printf("Requesting 5-second CPU Polling...\n");
    long ret = syscall(548, 999); 
    printf("Syscall returned: %ld\n", ret);
    return 0;
}
