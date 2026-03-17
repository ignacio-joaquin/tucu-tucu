#include "shared/ipc_layout.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <cmath>
#include <chrono> // For time measurement
#include <cstdlib>

extern "C" {
#include "lib/mazIO/maze.h"
}

struct position
{
    float x;
    float y;
    float heading_deg;
};

static void print_maze(const maze_t *m) {
    int w = m->width;
    int h = m->height;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int cell = m->data[y * w + x];
            char bottom = (cell & S) ? ' ' : '_';
            char right = (cell & E) ? ' ' : '|';
            putchar(bottom);
            putchar(right);
        }
        putchar('\n');
    }
}

static bool can_transition(const maze_t *m, int ox, int oy, int nx, int ny) {
    if (ox < 0 || ox >= m->width || oy < 0 || oy >= m->height) return false;
    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height) return false;

    int cell = m->data[oy * m->width + ox];
    int next = m->data[ny * m->width + nx];

    if (nx == ox + 1 && ny == oy) {
        // moving east
        return (cell & E) && (next & W);
    }
    if (nx == ox - 1 && ny == oy) {
        // moving west
        return (cell & W) && (next & E);
    }
    if (nx == ox && ny == oy + 1) {
        // moving south
        return (cell & S) && (next & N);
    }
    if (nx == ox && ny == oy - 1) {
        // moving north
        return (cell & N) && (next & S);
    }
    return false;
}

static void move_in_maze(const maze_t *m, position &pos, float dx, float dy) {
    constexpr float cell_size = 0.18f; // 18cm standard Micromouse cell
    constexpr float eps = 1e-3f;

    auto to_cell = [&](float v) {
        int c = (int)floor(v / cell_size);
        return c;
    };

    float nx = pos.x + dx;
    float ny = pos.y + dy;

    int cx = to_cell(pos.x);
    int cy = to_cell(pos.y);
    int tx = to_cell(nx);
    int ty = to_cell(ny);

    // Move in X direction first
    if (tx != cx) {
        int aimx = tx;
        if (can_transition(m, cx, cy, aimx, cy)) {
            pos.x = nx;
        } else {
            // clamp to boundary
            float boundary = (aimx > cx) ? (cx + 1) * cell_size - eps : (cx) * cell_size + eps;
            pos.x = boundary;
        }
    } else {
        pos.x = nx;
    }

    // Recompute current cell after X move
    cx = to_cell(pos.x);

    // Move in Y direction
    if (ty != cy) {
        int aimy = ty;
        if (can_transition(m, cx, cy, cx, aimy)) {
            pos.y = ny;
        } else {
            float boundary = (aimy > cy) ? (cy + 1) * cell_size - eps : (cy) * cell_size + eps;
            pos.y = boundary;
        }
    } else {
        pos.y = ny;
    }
}

void calculate_position(const MotorCmd& cmd, position& pos, float dt, const maze_t *maze) {
    constexpr float scale = 0.1f;
    constexpr float wheel_base = 0.5f; // distance between wheels (meters)

    float left_speed = cmd.pwm_duty[0] * scale;
    float right_speed = cmd.pwm_duty[1] * scale;

    float linear = 0.5f * (left_speed + right_speed);            // forward velocity (m/s)
    float angular = (right_speed - left_speed) / wheel_base;    // angular velocity (rad/s)

    const float PI = 3.14f;
    float heading_rad = pos.heading_deg * PI / 180.0f;

    // Integrate heading
    heading_rad += angular * dt;
    pos.heading_deg = heading_rad * 180.0f / PI;
    // normalize heading to [0,360)
    while (pos.heading_deg >= 360.0f) pos.heading_deg -= 360.0f;
    while (pos.heading_deg < 0.0f) pos.heading_deg += 360.0f;

    // Move with wall collisions
    float dx = linear * cosf(heading_rad) * dt;
    float dy = linear * sinf(heading_rad) * dt;
    move_in_maze(maze, pos, dx, dy);
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

    // Build a small deterministic maze for the simulation.
    srand(12345);
    const int maze_w = 16;
    const int maze_h = 16;
    int *maze_data = (int *)calloc(maze_w * maze_h, sizeof(int));
    maze_t maze = {maze_data, maze_w, maze_h};
    carve_maze(&maze);
    puts("-- Maze layout (bottom/right=#wall, ' ' = open):");
    print_maze(&maze);

    position pos = {0.1f, 0.1f, 0.0f};

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

        calculate_position(shm->motors, pos, dt, &maze);
        printf("Simulated position: x=%.2f, y=%.2f, heading=%.2f degrees\n",
               pos.x, pos.y, pos.heading_deg);
    }


    free(maze_data);
    shm_unlink("/tucu_test");
}

