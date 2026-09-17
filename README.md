# IEEE V5 MicroMouse — MIROSAURUS_V2

ESP32-based micromouse firmware in C (Arduino IDE). Senses walls with 5 IR
pairs, drives with fused gyro + IR + encoder control, maps a 16×16 maze and
solves it with flood fill.

Author: Eng. Adel Shata

---

## 1. Hardware

| Part | Spec |
|---|---|
| MCU | ESP32-S, 30-pin dev board |
| Motors | 2× N20 gear motors |
| Driver | TB6612FNG dual H-bridge |
| Wheels | 40 mm diameter (measured) |
| Encoders | Quadrature, CHANGE interrupt on channel A, **1400 counts / wheel rev** (measured) |
| IMU | MPU6050 over I2C (gyro Z only, raw registers, no library) |
| Wall sensors | 5× IR LED (MOSFET-switched) + phototransistor pairs: front, left, right, top-left, top-right |
| Maze | 16×16 cells, 180 mm cell size |

## 2. Pin map (`pins.h`)

IR receivers (analog):

| Sensor | GPIO |
|---|---|
| Right | 36 |
| Front | 34 |
| Left | 4 (ADC2 — keep WiFi/BT off) |
| Top-left | 39 |
| Top-right | 35 |

IR LEDs (digital output):

| LED | GPIO |
|---|---|
| Right | 2 |
| Front | 15 |
| Left | 5 |
| Top-right | 17 |
| Top-left | 16 |

Motors (TB6612FNG):

| Signal | Left | Right |
|---|---|---|
| IN1 | 26 | 33 |
| IN2 | 27 | 25 |
| PWM (LEDC) | 14 (ch 1) | 32 (ch 0) |
| ENC A | 18 | 19 |
| ENC B | 13 | 23 |

IMU: SDA = 21, SCL = 22.

> Do not use GPIO 16/17 for `Serial2` (IR LEDs live there) and do not put
> encoder or IR-receiver inputs on 2, 5, 12, 15.

## 3. Repository layout

```
MicroMouse_v5.ino        # entry point: setup()/loop(), maze state machine
Hal.h / Hal_Arduino.cpp  # hardware abstraction (GPIO, ADC, delay)
config.h                 # ALL tuning: geometry, PWM, IR thresholds, timeouts
pins.h                   # GPIO map (above)
Device/
  Ir/        Ir.h/.c       # ambient-subtract + EMA filtered wall sensing
  Motor/     Motor.h/.c    # TB6612FNG via ESP32 LEDC, deadband + slew limiter
  Encoder/   Encoder.h/.c  # quadrature ISR counters
  Imu/       Imu.h/.c      # MPU6050 gyro Z, bias cal, heading integrator
  Motion/    Motion.h/.c   # fused drive/turn primitives (see §6)
Algo/
  Maze/      Maze.h/.c     # 16×16 cell grid, wall set/check, globals
  Queue/     FIFO.h/.c     # BFS queue for flood fill (+ Project_Config.h)
  FloodFill/ Floodfill.h/.c# floodFillInit / floodFill / getNextMove
  main.c                 # unused PC stub, not part of the build
```

## 4. Build & flash (Arduino IDE — read this first)

1. Board: **ESP32 Dev Module**. Upload speed 921600 (lower if flaky).
2. ESP32 Arduino core **2.x** — the code uses `ledcSetup` / `ledcAttachPin` /
   `ledcWrite`. (`analogWrite` does not exist on ESP32.)
3. Open `MicroMouse_v5.ino` from this folder and upload. No extra libraries.
4. **Why the `.ino` includes `.c` files:** the IDE compiles only sketch-root
   sources and ignores `.c` files in subfolders. Each module's `.c` is
   `#include`d into the sketch so it actually gets built. If you add a new
   module, add both its `.h` and its `.c` to the `.ino`, and use
   file-relative includes inside subfolders (`../Motor/Motor.h`,
   `../../config.h`) — sketch-root-relative paths like
   `"Algo/FloodFill/Floodfill.h"` do **not** resolve from nested files.
5. Subfolder code is compiled as **C++**: no `++` on enums, no implicit
   `int → enum` or `unsigned char → enum` conversions — cast explicitly
   (`(dir_t)(x)`).

## 5. Runtime behavior (`MicroMouse_v5.ino`)

`setup()`: USB serial @115200 → MPU6050 init + 500-sample gyro bias cal
(**robot must be perfectly still**) → IR init → motors + encoders init →
3 s placement pause → `floodFillInit` (border walls + goal cells).

`loop()` per iteration:

1. `imuUpdate()`, then `irWallFront()` (self-contained: fires the front LED,
   ~2 ms, needs no prior `irReadAll()`).
2. Wall-bounce reflex: front wall → `motionTurn180()`, else
   `motionForwardOneCell()`. One short USB line per decision.
3. Record sensed walls into the map (`wallSet` left/front/right).
4. State check: reaching a distance-0 cell switches
   `STATE_EXPLORING_TO_CENTER` → `STATE_RETURNING_TO_START`.
5. Re-run `floodFill()` toward the active goal, pick `getNextMove()`
   (preferred order: right → forward → left → back, exploring) and execute
   it, updating `robPos` / `robDir`.

## 6. Motion control (`Device/Motion/`)

`driveStraight` fuses three corrections around a cruise PWM every loop:

- **Gyro heading hold** (primary, works with no walls): `4.0 PWM/°`.
- **IR lateral centering** (`0.10 PWM/unit`): both walls → center on
  `right − left`; one wall → hold `IR_TARGET_SIDE_SIGNAL` from it.
- **Encoder differential trim**: `(left − right)/20`.

Details: full IR scan every 3rd iteration (gyro every iteration);
slow-down to 70 PWM for the last 30 mm; **front-wall safety stop** at
`IR_FRONT_STOP_SIGNAL` (returns `false` so the caller re-senses);
encoder-polarity fault trip (stops instead of fighting to timeout);
6 s cell timeout for the 30 RPM gear motors.

`turnGyro` (90°/180°): gyro PID, full speed far away, crawl (≤55 PWM)
inside the last 25°, conditional integral (no windup), **short-brake**
finish (`motorsBrakeAll`, IN1=IN2=HIGH — `motorsStopAll` is coast),
120 ms settle with gyro running, then a ≤45 PWM correction pass to ±2°.

Public API: `motionBegin`, `motionDistanceMm`, `motionForwardOneCell`,
`motionTurnLeft90/Right90/180`, `motionSetProgressCb` (optional live
telemetry hook — keep it short, never block inside motion).

## 7. Sensing (`Device/Ir/`)

Each read: all LEDs off → sample ambient → LED on → settle 120 µs →
sample active → background subtract → EMA (`α = 0.35`).
`irReadAll()` scans all 5 (~10 ms); `irWallFront/Left/Right()` take a
fresh single-sensor reading by themselves.

Calibrated signals (this robot): open air ≈ 30, 15 cm ≈ 130, 2 cm ≈ 730.
Thresholds in `config.h`: front 700, side 700, diag 120, front-stop 600,
side target 500.

## 8. Key tuning (`config.h`)

| Symbol | Value | Meaning |
|---|---|---|
| `CELL_SIZE_MM` | 180 | maze cell pitch |
| `GOAL_C1` / `GOAL_C2` | 3 / 3 | goal-cell coordinates (see §10) |
| `COUNTS_PER_WHEEL_REV` | 1400 | measured, CHANGE on A |
| `BASE_SPEED_PWM` / `TURN_SPEED_PWM` | 150 / 120 | cruise / turn effort |
| `PWM_MIN_USEFUL` | 40 | deadband; `PWM_SLEW_PER_MS` 2 |
| `MOVE_CELL_TIMEOUT_MS` | 6000 | one cell needs ≈5 s at PWM 150 |
| `TURN_TIMEOUT_MS` | 3500 | ×2 for U-turns |
| `K_HEAD_PWM_PER_DEG` / `K_IR_PWM_PER_UNIT` | 4.0 / 0.10 | straight-line gains |

## 9. Troubleshooting (earned the hard way)

- **Motors silent with `Motor.c`, but raw `ledcWrite` works** — `motorsSet()`
  must be called **repeatedly**: the slew limiter only advances toward the
  target on each call. One call right after `motorsBegin()` yields ≈10/255
  (below friction) and stays there.
- **`undefined reference` to a module** — its `.c` is not `#include`d in the
  `.ino` (see §4.4).
- **One motor looks dead in `driveStraight`** — check correction signs and
  encoder polarity: if `(left+right)/2` doesn't grow while wheels travel,
  the fault trip fires and it reports `FWD STOPPED`.
- **Turn overshoots past target** — keep robot still during gyro cal, then
  lower `TURN_SPEED_PWM`; check turns on the floor, not held in hand.
- **Hits wall before sensing** — never print/block inside motion; the
  front-stop threshold is `IR_FRONT_STOP_SIGNAL`.
- **No upload / boot loop after adding code** — bisect: raw LEDC → driver →
  +encoders → +IMU, with one serial checkpoint per stage.
