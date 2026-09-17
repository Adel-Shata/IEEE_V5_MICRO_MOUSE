/*
 * Project: IEEE Micromouse V5 - MIROSAURUS_V2 (C version)
 * File: config.h
 * Date: 11 September 2026
 * Author: Eng. Adel Shat
 *
 * Description:
 *   Central configuration in pure C. No magic numbers elsewhere.
 *   Items marked TODO: CALIBRATE must be measured on your robot.
 */

#ifndef MICROMOUSE_CONFIG_H
#define MICROMOUSE_CONFIG_H

#include "pins.h"

/* Serial */
#define SERIAL_BAUD 115200UL

/* Maze geometry (VICTORIS 5.0 rulebook: 16x16, 18cm cells, 4 center goals) */
#define MAZE_SIZE   16
#define NUM_GOAL_CELLS 4
#define CELL_SIZE_MM 180.0f
#define GOAL_C1 3
#define GOAL_C2 3

/* Wheel / chassis -- TODO: CALIBRATE with calipers/ruler */
#define WHEEL_DIAMETER_MM 40.0f   /* measured 16Dec */
#define WHEEL_BASE_MM     80.0f   /* TODO: CALIBRATE wheel-center distance */
#define WHEEL_CIRC_MM     (3.14159265f * WHEEL_DIAMETER_MM)
/* Encoder: 700 counts/wheel-rev, 2ch (CHANGE on A). */
#define GEAR_RATIO_EST       500.0f  /* unused */
#define COUNTS_PER_WHEEL_REV 1400L  /* measured 16Dec (CHANGE on A, 2x) */
#define ENCODER_LEFT_INVERT  0
#define ENCODER_RIGHT_INVERT 0
#define MOTOR_LEFT_INVERT    0       /* TODO: CALIBRATE */
#define MOTOR_RIGHT_INVERT   0       /* TODO: CALIBRATE */

/* PWM (ESP32 LEDC) */
#define PWM_FREQ_HZ     20000
#define PWM_RES_BITS    8
#define PWM_MAX         255
#define PWM_MIN_USEFUL  40    /* TODO: CALIBRATE deadband */
#define BASE_SPEED_PWM  150   /* TODO: CALIBRATE search cruise */
#define TURN_SPEED_PWM  120   /* TODO: CALIBRATE */
#define PWM_SLEW_PER_MS 2

/* Speed PID (counts/s -> PWM) -- TODO: CALIBRATE */
#define KP_SPEED 0.35f
#define KI_SPEED 0.06f
#define KD_SPEED 0.02f
#define SPEED_PID_IMAX 120.0f
#define TARGET_SPEED_CPS 900.0f  /* TODO: CALIBRATE */

/* Wall / heading -- TODO: CALIBRATE */
#define KP_WALL 0.55f
#define KI_WALL 0.00f
#define KD_WALL 0.18f
#define KP_HEADING 2.2f
#define KD_HEADING 0.35f

/* IR sensing -- thresholds on background-subtracted signal (0..4095) */
#define IR_SETTLE_US        120
#define IR_SAMPLES_PER_READ 8
#define IR_AMBIENT_SAMPLES  4
#define IR_EMA_ALPHA        0.35f
#define IR_WALL_THRESHOLD_FRONT 700   /* 11x12 robot, 3cm front gap: (30+550)/2 */
#define IR_WALL_THRESHOLD_SIDE  700   /* 11x12 robot, 3.5cm side gap: (30+500)/2 */
#define IR_WALL_THRESHOLD_DIAG  120   /* diag sees farther, keep */
#define IR_FRONT_STOP_SIGNAL    600   /* 0.8*730 near */
#define IR_TARGET_SIDE_SIGNAL   500   /* 3.5cm centered reading */

/* Motion */
#define TURN_TOL_DEG 2.0f
#define MOVE_CELL_TIMEOUT_MS 6000UL  /* 30 RPM needs ~5s for 180mm */
#define TURN_TIMEOUT_MS      3500UL  /* slow approach + correction pass need room */
#define MOVE_END_SLOW_PWM    70
/* Speed-run: faster straight cruise, still below PWM_MAX. Tune after BASE. */
#define SPEEDRUN_PWM         210   /* TODO: CALIBRATE (BASE < SPEEDRUN <= 255) */

/* Safety */
#define LOOP_WATCHDOG_MS 5000UL


#endif /* MICROMOUSE_CONFIG_H */