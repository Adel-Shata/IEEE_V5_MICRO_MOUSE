/*
 * Encoder.c
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#include "Encoder.h"
#include <Arduino.h>

static volatile long s_left = 0;
static volatile long s_right = 0;

static void IRAM_ATTR onLeftA(void) {
    int a = digitalRead(PIN_L_ENC_A);
    int b = digitalRead(PIN_L_ENC_B);
#if ENCODER_LEFT_INVERT
    b = !b;
#endif
    if (a == b) s_left -= 1;
    else s_left += 1;
}

static void IRAM_ATTR onRightA(void) {
    int a = digitalRead(PIN_R_ENC_A);
    int b = digitalRead(PIN_R_ENC_B);
#if ENCODER_RIGHT_INVERT
    b = !b;
#endif
    if (a == b) s_right += 1;
    else s_right -= 1;
}

void encodersBegin(void) {
    pinMode(PIN_L_ENC_A, INPUT_PULLUP);
    pinMode(PIN_L_ENC_B, INPUT_PULLUP);
    pinMode(PIN_R_ENC_A, INPUT_PULLUP);
    pinMode(PIN_R_ENC_B, INPUT_PULLUP);
    s_left = 0;
    s_right = 0;
    attachInterrupt(digitalPinToInterrupt(PIN_L_ENC_A), onLeftA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_R_ENC_A), onRightA, CHANGE);
}

void encodersReset(void) {
    noInterrupts();
    s_left = 0;
    s_right = 0;
    interrupts();
}

long encodersLeft(void) {
    long v;
    noInterrupts();
    v = s_left;
    interrupts();
    return v;
}

long encodersRight(void) {
    long v;
    noInterrupts();
    v = s_right;
    interrupts();
    return v;
}
