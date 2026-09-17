/*
 * Imu.c
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 *
 *  MPU6050 raw registers, no external library.
 *  Gyro full scale +-250 dps (131 LSB/dps).
 */

#include "Imu.h"
#include <Arduino.h>
#include <Wire.h>

#define MPU_ADDR        0x68
#define MPU_WHO_AM_I    0x75
#define MPU_PWR_MGMT_1  0x6B
#define MPU_CONFIG      0x1A
#define MPU_GYRO_CONFIG 0x1B
#define MPU_GYRO_Z_H    0x47

static float s_bias = 0.0f;
static float s_heading = 0.0f;
static float s_gz = 0.0f;
static unsigned long s_lastUs = 0;

static void regWrite(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

static int16_t gyroZRaw(void) {
    int16_t v;
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_GYRO_Z_H);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
    v = (int16_t)((Wire.read() << 8) | Wire.read());
    return v;
}

bool imuBegin(void) {
    uint8_t who;
    long sum = 0;
    const int N = 500;
    int i;
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(400000);
    delay(100);
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_WHO_AM_I);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)1);
    who = Wire.read();
    if (who != 0x68) return false;
    regWrite(MPU_PWR_MGMT_1, 0x00); /* wake, internal 8MHz */
    delay(100);
    regWrite(MPU_CONFIG, 0x03);      /* DLPF 44Hz */
    regWrite(MPU_GYRO_CONFIG, 0x00); /* +-250 dps */
    delay(100);
    /* bias: robot MUST be still */
    for (i = 0; i < N; i++) {
        sum += gyroZRaw();
        delay(2);
    }
    s_bias = (float)sum / N;
    s_heading = 0.0f;
    s_gz = 0.0f;
    s_lastUs = micros();
    return true;
}

void imuUpdate(void) {
    unsigned long now = micros();
    float dt = (now - s_lastUs) / 1000000.0f;
    int16_t raw;
    if (dt <= 0) return;
    if (dt > 0.5f) dt = 0.5f;
    s_lastUs = now;
    raw = gyroZRaw();
    s_gz = (raw - s_bias) / 131.0f;
    s_heading += s_gz * dt;
}

float imuHeadingDeg(void) {
    return s_heading;
}

float imuGyroZDps(void) {
    return s_gz;
}

void imuResetHeading(void) {
    s_heading = 0.0f;
}
