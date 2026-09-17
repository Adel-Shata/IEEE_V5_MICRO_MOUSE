/*
 * Hal_Arduino.cpp (MINIMAL IR-TEST ONLY)
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#include "Hal.h"

void halGpioMode(uint8_t pin, uint8_t mode) {
    pinMode(pin, mode == HAL_GPIO_OUTPUT ? OUTPUT : INPUT);
}

void halDigitalWrite(uint8_t pin, uint8_t val) {
    digitalWrite(pin, val == HAL_HIGH ? HIGH : LOW);
}

uint16_t halAdcRead(uint8_t pin) {
    return (uint16_t)analogRead(pin);
}

void halDelayMs(unsigned long ms) {
    delay(ms);
}
