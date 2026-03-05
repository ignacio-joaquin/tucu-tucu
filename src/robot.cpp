#include "shared/ipc_layout.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

int main() {
    int fd = shm_open("/tucu_test", O_RDWR, 0600);
    auto* shm = (SimSharedBlock*)mmap(nullptr, sizeof(SimSharedBlock),
                                   PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    while (true) {
        int val = shm->sensor_ready.load();
        printf("robot read: %d\n", val);
        if (val == 5) break;
        usleep(100000);
    }
}