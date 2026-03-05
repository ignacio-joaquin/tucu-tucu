#pragma once
#include <cstdint>

enum class SensorPos { FRONT_LEFT, FRONT_RIGHT, LEFT, RIGHT };
enum class MotorSide  { LEFT, RIGHT };

// ── Sensors ──────────────────────────────────────────────────────
float    driver_tof_read_mm(SensorPos s);       // VL53L1X / ray-cast
float    driver_ultrasonic_read_mm(SensorPos s);
float    driver_ir_read_normalized(SensorPos s); // 0.0 – 1.0

// ── Motors ───────────────────────────────────────────────────────
void     driver_motor_set_pwm(MotorSide m, float duty); // -1.0 to +1.0
int32_t  driver_encoder_get_ticks(MotorSide m);
void     driver_encoder_reset(MotorSide m);

// ── IMU (optional ESP32 internal) ────────────────────────────────
float    driver_imu_heading_deg();
float    driver_imu_gyro_z_dps();

// ── Time ─────────────────────────────────────────────────────────
uint64_t driver_time_us();   // sim returns simulated time, HW returns esp_timer