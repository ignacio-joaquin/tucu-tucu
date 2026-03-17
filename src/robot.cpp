#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdio>

#include "shared/ipc_layout.h"

int main() {
    int fd = shm_open("/tucu_test", O_RDWR, 0600);
    auto* shm = (SimSharedBlock*)mmap(nullptr, sizeof(SimSharedBlock),
                                      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int i = 0;
    while (true) {
        if (shm->motor_ready.load() != 1) {
            shm->motors.pwm_duty[0] = (i % 2 == 0) ? 100 : 0;  // Alternate left motor
            shm->motors.pwm_duty[1] = (i % 2 == 0) ? 100 : 0;  // Alternate right motor
            shm->motors.seq = i + 1;                           // Increment sequence number
            shm->motor_ready.store(1);                         // Mark as ready for sim to read
            i++;
        }
    }
}