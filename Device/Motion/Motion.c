/*
 * Motion.c
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 *
 *  Straight: encoder distance + gyro heading hold + IR centering.
 *  - Gyro is the primary angle reference (works with no walls).
 *  - IR centers between side walls / follows one wall (needs irReadAll).
 *  Turns: gyro angle with proportional slowdown, +-2deg tolerance.
 */

#include "Motion.h"
#include "../Motor/Motor.h"
#include "../Encoder/Encoder.h"
#include "../Imu/Imu.h"
#include "../Ir/Ir.h"
#include <Arduino.h>

#define MM_TO_COUNTS(mm) ((long)((mm) / WHEEL_CIRC_MM * (float)COUNTS_PER_WHEEL_REV))
/* Wheel travel per in-place 90deg: pi*BASE/4 */
#define TURN90_MM (3.14159265f * WHEEL_BASE_MM / 4.0f)

/* Gains -- TUNE on floor */
#define K_HEAD_PWM_PER_DEG 4.0f   /* gyro heading hold */
#define K_IR_PWM_PER_UNIT  0.10f  /* IR lateral correction */
#define IR_CENTER_MIN      100     /* min signal to trust a side wall */
#define TURN_TOL_DEG       1.5f
#define TURN_PWM_MIN       50
#define TURN_KI            0.02f  /* integral windup */
#define TURN_KI_MAX        30.0f  /* integral clamp */
#define TURN_KD            0.8f   /* derivative damping */

void motionBegin(void) {
    motorsBegin();
    encodersBegin();
}

static motion_progress_t s_progCb = 0;

void motionSetProgressCb(motion_progress_t cb) {
    s_progCb = cb;
}

static int16_t clampPwm(int16_t v) {
    if (v > PWM_MAX) return PWM_MAX;
    if (v < -PWM_MAX) return -PWM_MAX;
    return v;
}

static bool driveStraight(long targetCounts, uint8_t cruisePwm, unsigned long timeoutMs) {
    unsigned long t0 = millis();
    long l0 = encodersLeft();
    long r0 = encodersRight();
    float targetHead;
    /* Cached side-wall readings (refreshed every few iters, see below). */
    uint16_t sl = 0, sr = 0, sf = 0;
    bool wl = false, wr = false;
    long iter = 0;
    imuUpdate();
    targetHead = imuHeadingDeg();
    while (1) {
        long l = encodersLeft() - l0;
        long r = encodersRight() - r0;
        long avg = (l + r) / 2;
        long remain = targetCounts - avg;
        int16_t base, corrH, corrI, corrE, pl, pr;
        float hErr;
        if (remain <= 0) break;
        if (millis() - t0 > timeoutMs) { motorsStopAll(); return false; }

        /*
         * Encoder sanity: both motors are driven forward, so the signed
         * average must grow. If a lot of travel happened on the wheels
         * but the average is ~0, the encoders count opposite directions
         * (polarity/wiring fault) -> stop instead of fighting to timeout.
         */
        if (millis() - t0 > 500) {
            long trav = (labs(l) + labs(r)) / 2;
            if (trav > 300 && avg < 50 && avg > -50) {
                motorsStopAll();
                return false;
            }
        }

        imuUpdate(); /* gyro is the primary angle ref: every iteration */
        /*
         * IR costs ~10-12ms per full read (settle delays + ADC), so
         * refresh it every 3rd iteration and steer on cached values in
         * between. Fusion stays gyro + IR + encoder, loop runs ~3x faster.
         */
        iter++;
        if (iter == 1 || (iter % 3) == 0) {
            irReadAll();
            sl = irRead(IR_SENSOR_LEFT);
            sr = irRead(IR_SENSOR_RIGHT);
            sf = irRead(IR_SENSOR_FRONT);
            wl = sl > IR_CENTER_MIN;
            wr = sr > IR_CENTER_MIN;
        }

        /*
         * Front-wall safety: never drive into a wall. Stops the motors
         * the moment the front signal reaches the stop threshold and
         * returns false so the caller re-senses and decides (e.g. turn).
         * Checked every iteration on the cached value (refreshed above).
         */
        if (sf >= IR_FRONT_STOP_SIGNAL) { motorsStopAll(); return false; }

        if (remain < MM_TO_COUNTS(30.0f)) base = MOVE_END_SLOW_PWM;
        else base = cruisePwm;

        /* 1. Gyro heading hold (primary, works with no walls) */
        hErr = imuHeadingDeg() - targetHead;
        corrH = (int16_t)(K_HEAD_PWM_PER_DEG * hErr);

        /* 2. IR lateral centering. +corrI always means steer left
         * (slow left, speed right): pl = base - corrI, pr = base + corrI.
         * Both walls: error e = sr - sl (nearer right -> steer left).
         * One wall: estimate e against the target side signal, SAME sign
         * convention: too close to left (sl > target) gives e < 0 ->
         * steer right; too close to right (sr > target) gives e > 0 ->
         * steer left. */
        corrI = 0;
        if (wl && wr) {
            /* centered when sr == sl; nearer right -> steer left */
            corrI = (int16_t)(K_IR_PWM_PER_UNIT * ((float)sr - (float)sl));
        } else if (wl) {
            /* hold TARGET distance from left wall */
            corrI = (int16_t)(K_IR_PWM_PER_UNIT * ((float)IR_TARGET_SIDE_SIGNAL - (float)sl));
        } else if (wr) {
            /* hold TARGET distance from right wall */
            corrI = (int16_t)(K_IR_PWM_PER_UNIT * ((float)sr - (float)IR_TARGET_SIDE_SIGNAL));
        }
        /* corrI sign: + means steer left (slow left, speed right) */

        /* 3. Encoder differential trim */
        corrE = (int16_t)((l - r) / 20);

        pl = clampPwm((int16_t)(base + corrH - corrI - corrE));
        pr = clampPwm((int16_t)(base - corrH + corrI + corrE));
        motorsSet(pl, pr);
        if (s_progCb && (iter % 10) == 0) {
            s_progCb(remain, pl, pr, hErr);
        }
        delay(2);
    }
    motorsStopAll();
    return true;
}

/* Turn approach: full speed far away, crawl near the target. */
#define TURN_SLOW_DEG      25.0f  /* below this: capped PWM */
#define TURN_SLOW_PWM      55     /* approach cap near target */
#define TURN_SLOW_MIN      30     /* min kick near target (deadband lifts to 40) */
#define TURN_SETTLE_MS     120
#define TURN_CORR_TOL_DEG  2.0f
#define TURN_CORR_PWM      45
#define TURN_CORR_MIN      30
#define TURN_CORR_MS       600

static bool turnGyro(float degrees, uint8_t pwm, unsigned long timeoutMs) {
    unsigned long t0 = millis();
    unsigned long tSet, tCorr;
    float target;
    float integral = 0.0f;
    float prevErr = 0.0f;
    float err, pTerm, iTerm, dTerm, corr;
    int16_t p;
    int16_t cap, kick;
    long iter = 0;
    imuUpdate();
    target = imuHeadingDeg() + degrees;
    while (1) {
        imuUpdate();
        err = target - imuHeadingDeg();
        if (err > -TURN_TOL_DEG && err < TURN_TOL_DEG) break;
        if (millis() - t0 > timeoutMs) { motorsStopAll(); return false; }
        /* PID with conditional integral: only near target, no windup */
        pTerm = err;
        if (err < 30.0f && err > -30.0f) {
            integral += err;
        } else {
            integral = 0.0f;
        }
        if (integral > TURN_KI_MAX) integral = TURN_KI_MAX;
        if (integral < -TURN_KI_MAX) integral = -TURN_KI_MAX;
        iTerm = TURN_KI * integral;
        dTerm = TURN_KD * (err - prevErr);
        prevErr = err;
        corr = pTerm + iTerm + dTerm;
        /* convert to PWM: fast far away, crawl near target */
        if (err > TURN_SLOW_DEG || err < -TURN_SLOW_DEG) {
            cap = pwm;
            kick = TURN_PWM_MIN;
        } else {
            cap = TURN_SLOW_PWM;
            kick = TURN_SLOW_MIN;
        }
        p = (int16_t)(corr);
        if (p > 0 && p < kick) p = kick;
        if (p < 0 && p > -kick) p = -kick;
        if (p > cap) p = cap;
        if (p < -cap) p = -cap;
        /* turn direction: positive corr = CCW (left), negative = CW (right) */
        motorsSet((int16_t)(-p), (int16_t)p);
        iter++;
        if (s_progCb && (iter % 25) == 0) {
            s_progCb((long)(err * 100.0f), (int16_t)(-p), p, err);
        }
        delay(1);
    }
    motorsBrakeAll(); /* short brake: kill coast overshoot */
    /* settle WITH gyro running so heading stays true */
    tSet = millis();
    while (millis() - tSet < TURN_SETTLE_MS) {
        imuUpdate();
        delay(5);
    }
    /* correction pass: undo any residual over/undershoot, slowly */
    tCorr = millis();
    while (millis() - tCorr < TURN_CORR_MS) {
        imuUpdate();
        err = target - imuHeadingDeg();
        if (err > -TURN_CORR_TOL_DEG && err < TURN_CORR_TOL_DEG) break;
        p = (int16_t)(err);
        if (p > 0 && p < TURN_CORR_MIN) p = TURN_CORR_MIN;
        if (p < 0 && p > -TURN_CORR_MIN) p = -TURN_CORR_MIN;
        if (p > TURN_CORR_PWM) p = TURN_CORR_PWM;
        if (p < -TURN_CORR_PWM) p = -TURN_CORR_PWM;
        motorsSet((int16_t)(-p), (int16_t)p);
        delay(2);
    }
    motorsBrakeAll();
    return true;
}

bool motionDistanceMm(float mm, uint8_t cruisePwm) {
    if (mm <= 0) return false;
    return driveStraight(MM_TO_COUNTS(mm), cruisePwm, MOVE_CELL_TIMEOUT_MS);
}

bool motionForwardOneCell(void) {
    return driveStraight(MM_TO_COUNTS(CELL_SIZE_MM), BASE_SPEED_PWM, MOVE_CELL_TIMEOUT_MS);
}

bool motionTurnLeft90(void) {
    return turnGyro(90.0f, TURN_SPEED_PWM, TURN_TIMEOUT_MS);
}

bool motionTurnRight90(void) {
    return turnGyro(-90.0f, TURN_SPEED_PWM, TURN_TIMEOUT_MS);
}

bool motionTurn180(void) {
    return turnGyro(180.0f, TURN_SPEED_PWM, TURN_TIMEOUT_MS * 2);
}
