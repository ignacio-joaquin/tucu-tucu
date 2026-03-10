# tucu-tucu — build plan
Tucu tucu is a Micromouse inspired simulator.

---

## Goals

- Robot C++ compiles for both native (sim) and `xtensa-esp32-elf` (hardware) with zero source changes
- Simulator and robot run as separate OS processes, communicating over POSIX shared memory at 1 kHz
- Sensor models: ToF ray-cast, IR falloff, encoder ticks derived from kinematics
- Dear ImGui GUI: maze view, robot pose, sensor rays, telemetry, pause/step/reset
- Same `maze_solver.cpp` that solves the sim maze flashes directly onto the board

---

## Milestones

### M0 — IPC skeleton DONE
- CMake root with `BUILD_TARGET=SIM|ESP32` switch
- `shared/ipc_layout.h` and `shared/driver_iface.h` finalized — these never change after this milestone
- Simulator opens shared memory block, writes dummy sensor values
- Robot sim driver reads them and prints at 100 Hz
- **Done when:** two processes exchange data at 100 Hz with no errors

### M1 — Maze + physics
- `.maz` binary loader (16×16, standard Micromouse Online format)
- Differential drive kinematics step (1 ms tick, first-order lag on PWM)
- Wall collision + tunneling guard (`v·dt > wall_thickness/2` → subdivide step)
- **Done when:** robot moves in a straight line and stops at a wall

### M2 — Sensor models
- ToF ray-cast against axis-aligned wall segments, Gaussian noise σ = 3 mm
- IR inverse-square falloff, saturates < 30 mm, blind > 300 mm
- Encoder tick accumulation from wheel speed
- Noise togglable via config struct
- **Done when:** static robot in a known cell reports correct distances on all 4 sensors

### M3 — GUI
- Dear ImGui + OpenGL 3.3 window
- Maze grid, robot as oriented rectangle, sensor rays
- Telemetry panel: sensor values, encoder counts, speed, heading, sim time
- Pause / step / reset / speed multiplier
- **Done when:** robot can be watched and manually controlled from the GUI

### M4 — Robot control loop
- `setup()` / `loop()` structure matching ESP-IDF style
- PID speed controller using encoder feedback (`motion.cpp`)
- Wall-follower algorithm (`maze_solver.cpp`)
- **Done when:** robot completes a simple maze autonomously

### M5 — ESP32 driver swap
- ESP-IDF implementations for all `driver_iface.h` functions
- CMake cross-compiles for `xtensa-esp32-elf` when `BUILD_TARGET=ESP32`
- `robot/src/` untouched
- **Done when:** `maze_solver.cpp` runs on physical hardware without changes

### M6 — Polish
- Maze editor (click walls on/off)
- Run stats: solve time, path length, revisited cells
- Sensor noise presets (ideal / realistic / degraded)
- YAML/JSON config for robot parameters (wheelbase, max speed, etc.)
- Save/load run recordings

---

## Stack

| Concern | Choice |
|---|---|
| IPC | POSIX shared memory + `std::atomic` |
| GUI | Dear ImGui + OpenGL 3.3 |
| Physics | Custom — differential drive only, no Box2D |
| Maze format | `.maz` binary (Micromouse Online standard) |
| Build | CMake 3.20 + Ninja |
| C++ standard | C++17 |
| Config | nlohmann/json (header-only) |

---

