#include "shared/ipc_layout.h"
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

int main() {
    // 1. create shared memory
    int fd = shm_open("/tucu_test", O_CREAT | O_RDWR, 0600);
    ftruncate(fd, sizeof(SimSharedBlock));
    auto* shm = (SimSharedBlock*)mmap(nullptr, sizeof(SimSharedBlock),
                                       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    shm->sensor_ready.store(0);

    // 2. spin the robot child
    pid_t pid = fork();
    if (pid == 0) {
        execl("./robot", "./robot", nullptr);
        perror("execl failed");
        _exit(1);
    }

    // 3. become the simulator
    execl("./sim", "./sim", nullptr);
    perror("execl failed");
    _exit(1);
}