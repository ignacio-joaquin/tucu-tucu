#include "shared/ipc_layout.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <cmath>
#include <chrono> // For time measurement

struct position
{
    float x;
    float y;
    float heading_deg;
};

void calculate_position(const MotorCmd& cmd, position& pos, float dt) {
    constexpr float scale = 0.1f;
    constexpr float wheel_base = 0.5f; // distance between wheels (meters)

    float left_speed = cmd.pwm_duty[0] * scale;
    float right_speed = cmd.pwm_duty[1] * scale;

    float linear = 0.5f * (left_speed + right_speed);            // forward velocity (m/s)
    float angular = (right_speed - left_speed) / wheel_base;    // angular velocity (rad/s)

    const float PI = 3.14f;
    float heading_rad = pos.heading_deg * PI / 180.0f;

    // Integrate heading and position
    heading_rad += angular * dt;
    pos.x += linear * cosf(heading_rad) * dt;
    pos.y += linear * sinf(heading_rad) * dt;

    pos.heading_deg = heading_rad * 180.0f / PI;
    // normalize heading to [0,360)
    while (pos.heading_deg >= 360.0f) pos.heading_deg -= 360.0f;
    while (pos.heading_deg < 0.0f) pos.heading_deg += 360.0f;
}

int main() {

    int fd = shm_open("/tucu_test", O_RDWR, 0600);
    auto* shm = (SimSharedBlock*)mmap(nullptr, sizeof(SimSharedBlock),
                                   PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Ensure shared memory starts in a deterministic state.
    shm->motor_ready.store(0);
    shm->sensor_ready.store(0);
    shm->running.store(true);
    shm->reset_requested.store(false);

    position pos = {0.0f, 0.0f};

    // Use a fixed timestep so that simulation results are deterministic.
    constexpr float kFixedDt = 0.01f; // seconds
    uint64_t sim_time_us = 0;

    for (int i = 1; i <= 150; i++) {
        // Enforce a strict command/step handshake (deterministic ordering).
        // Wait until the robot posts a new motor command for this step.
        while (!shm->motor_ready.load()) {
            // Yield CPU to avoid busy-spinning; order doesn't change determinism.
            usleep(1000);
        }

        float dt = kFixedDt;
        sim_time_us += static_cast<uint64_t>(dt * 1e6f);
        shm->sensors.sim_time_us = sim_time_us;

        printf("Motor command received: pwm_duty[0]=%f, pwm_duty[1]=%f, seq=%u\n",
               shm->motors.pwm_duty[0], shm->motors.pwm_duty[1], shm->motors.seq);
        shm->motor_ready.store(0); // Acknowledge command read

        calculate_position(shm->motors, pos, dt);
        printf("Simulated position: x=%.2f, y=%.2f, heading=%.2f degrees\n",
               pos.x, pos.y, pos.heading_deg);
    }


    shm_unlink("/tucu_test");
}

