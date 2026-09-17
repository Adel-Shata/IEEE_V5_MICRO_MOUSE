/*
 * Motor.c
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#include "Motor.h"
#include <Arduino.h>

#define MOTOR_CH_LEFT  0
#define MOTOR_CH_RIGHT 1

static int16_t s_leftCur = 0;
static int16_t s_rightCur = 0;
static unsigned long s_leftT = 0;
static unsigned long s_rightT = 0;

static int16_t applySlew(int16_t target, int16_t *cur, unsigned long *lastT) {
    unsigned long now = millis();
    unsigned long dt = now - *lastT;
    int16_t maxDelta;
    int16_t delta;
    if (dt == 0) dt = 1;
    maxDelta = (int16_t)(PWM_SLEW_PER_MS * dt);
    if (maxDelta < 1) maxDelta = 1;
    delta = target - *cur;
    if (delta > maxDelta) delta = maxDelta;
    else if (delta < -maxDelta) delta = -maxDelta;
    *cur += delta;
    *lastT = now;
    return *cur;
}

static int16_t applyDeadband(int16_t pwm) {
    if (pwm != 0 && pwm > -(int16_t)PWM_MIN_USEFUL && pwm < (int16_t)PWM_MIN_USEFUL) {
        pwm = (pwm > 0) ? PWM_MIN_USEFUL : -PWM_MIN_USEFUL;
    }
    if (pwm > PWM_MAX) pwm = PWM_MAX;
    if (pwm < -PWM_MAX) pwm = -PWM_MAX;
    return pwm;
}

static void writeMotor(uint8_t in1, uint8_t in2, uint8_t ch, int16_t pwm) {
    uint8_t mag;
    if (pwm > 0) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        mag = (uint8_t)pwm;
    } else if (pwm < 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        mag = (uint8_t)(-pwm);
    } else {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        mag = 0;
    }
    ledcWrite(ch, mag);
}

void motorsBegin(void) {
    pinMode(PIN_L_IN1, OUTPUT);
    pinMode(PIN_L_IN2, OUTPUT);
    pinMode(PIN_R_IN1, OUTPUT);
    pinMode(PIN_R_IN2, OUTPUT);
    digitalWrite(PIN_L_IN1, LOW);
    digitalWrite(PIN_L_IN2, LOW);
    digitalWrite(PIN_R_IN1, LOW);
    digitalWrite(PIN_R_IN2, LOW);
    ledcSetup(MOTOR_CH_LEFT, PWM_FREQ_HZ, PWM_RES_BITS);
    ledcSetup(MOTOR_CH_RIGHT, PWM_FREQ_HZ, PWM_RES_BITS);
    ledcAttachPin(PIN_L_PWM, MOTOR_CH_LEFT);
    ledcAttachPin(PIN_R_PWM, MOTOR_CH_RIGHT);
    ledcWrite(MOTOR_CH_LEFT, 0);
    ledcWrite(MOTOR_CH_RIGHT, 0);
    s_leftCur = 0;
    s_rightCur = 0;
    s_leftT = millis();
    s_rightT = millis();
}

void motorLeftSet(int16_t pwm) {
#if MOTOR_LEFT_INVERT
    pwm = -pwm;
#endif
    pwm = applyDeadband(pwm);
    pwm = applySlew(pwm, &s_leftCur, &s_leftT);
    writeMotor(PIN_L_IN1, PIN_L_IN2, MOTOR_CH_LEFT, pwm);
}

void motorRightSet(int16_t pwm) {
#if MOTOR_RIGHT_INVERT
    pwm = -pwm;
#endif
    pwm = applyDeadband(pwm);
    pwm = applySlew(pwm, &s_rightCur, &s_rightT);
    writeMotor(PIN_R_IN1, PIN_R_IN2, MOTOR_CH_RIGHT, pwm);
}

void motorsSet(int16_t leftPwm, int16_t rightPwm) {
    motorLeftSet(leftPwm);
    motorRightSet(rightPwm);
}

void motorsStopAll(void) {
    digitalWrite(PIN_L_IN1, LOW);
    digitalWrite(PIN_L_IN2, LOW);
    digitalWrite(PIN_R_IN1, LOW);
    digitalWrite(PIN_R_IN2, LOW);
    ledcWrite(MOTOR_CH_LEFT, 0);
    ledcWrite(MOTOR_CH_RIGHT, 0);
    s_leftCur = 0;
    s_rightCur = 0;
    s_leftT = millis();
    s_rightT = millis();
}

void motorsBrakeAll(void) {
    digitalWrite(PIN_L_IN1, HIGH);
    digitalWrite(PIN_L_IN2, HIGH);
    digitalWrite(PIN_R_IN1, HIGH);
    digitalWrite(PIN_R_IN2, HIGH);
    ledcWrite(MOTOR_CH_LEFT, 0);
    ledcWrite(MOTOR_CH_RIGHT, 0);
    s_leftCur = 0;
    s_rightCur = 0;
    s_leftT = millis();
    s_rightT = millis();
}
