#include "shared/ipc_layout.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>

int main() {

    int fd = shm_open("/tucu_test", O_RDWR, 0600);
    auto* shm = (SimSharedBlock*)mmap(nullptr, sizeof(SimSharedBlock),
                                   PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for (int i = 1; i <= 5; i++) {
        sleep(1);
        shm->sensor_ready.store(i);
        printf("sim wrote: %d\n", i);
    }


    shm_unlink("/tucu_test");
}