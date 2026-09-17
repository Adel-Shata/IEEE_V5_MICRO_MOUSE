/*
 * Project: IEEE Micromouse V5 - MIROSAURUS_V2 (C version)
 * File: pins.h
 * Date: 11 September 2026
 * Author: Eng. Adel Shat
 *
 * Description:
 *   Exact GPIO mapping for ESP32-S 30-pin dev board. Pure C header.
 *   DO NOT change unless schematic/rulebook proves a conflict.
 */

#ifndef MICROMOUSE_PINS_H
#define MICROMOUSE_PINS_H

#include <stdint.h>


/* IR receivers (analog phototransistor). All ADC-capable:
   36/34/39/35 = ADC1 input-only, 4 = ADC2_CH0 (WiFi must stay OFF). */
#define PIN_IR_RIGHT      36
#define PIN_IR_FRONT      34
#define PIN_IR_LEFT       4
#define PIN_IR_TOP_LEFT   39
#define PIN_IR_TOP_RIGHT  35

/* IR transmitters (MOSFET gates, digital output, 100k pulldown).
   2/5/15 strapping safe with weak pulldown. 16/17 = UART2, do NOT use Serial2. */
#define PIN_LED_RIGHT     2
#define PIN_LED_FRONT     15
#define PIN_LED_LEFT      5
#define PIN_LED_TOP_RIGHT 17
#define PIN_LED_TOP_LEFT  16

/* Left motor TB6612FNG */
#define PIN_L_IN1   26
#define PIN_L_IN2   27
#define PIN_L_ENC_A 18
#define PIN_L_ENC_B 13
#define PIN_L_PWM   14

/* Right motor TB6612FNG */
#define PIN_R_IN1   33
#define PIN_R_IN2   25
#define PIN_R_ENC_A 19
#define PIN_R_ENC_B 23
#define PIN_R_PWM   32

/* MPU6050 I2C */
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22

/* Forbidden for encoder inputs or IR receiver inputs: 2, 5, 12, 15.
   Current mapping complies: encoders 19/23/18/13, receivers 36/34/4/39/35.
   Tx LEDs intentionally use 2/5/15/17/16 as OUTPUTS. */


#endif /* MICROMOUSE_PINS_H */
