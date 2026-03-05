#pragma once
#include <cstdint>
#include <atomic>

struct SensorFrame {
    float    tof_mm[4];          // indexed by SensorPos
    float    ir_norm[4];
    float    imu_heading_deg;
    float    imu_gyro_z_dps;
    int32_t  encoder_ticks[2];   // [LEFT, RIGHT]
    uint64_t sim_time_us;
};

struct MotorCmd {
    float    pwm_duty[2];        // [LEFT, RIGHT]  -1.0 to +1.0
    uint32_t seq;                // monotonic sequence number
};

struct SimSharedBlock {
    std::atomic<uint32_t> sensor_ready;   // sim writes, robot reads
    std::atomic<uint32_t> motor_ready;    // robot writes, sim reads
    SensorFrame  sensors;
    MotorCmd     motors;
    // sim → robot signals
    std::atomic<bool> running;
    std::atomic<bool> reset_requested;
};