#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

int main() {
    syscall(548, 555); 
    int fd = open("/dev/hft", O_RDWR);
    void *ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    printf("Result from shared memory: %s\n", (char *)ptr);
    return 0;
}
