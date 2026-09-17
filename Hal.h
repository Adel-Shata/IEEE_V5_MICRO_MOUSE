/*
 * Hal.h (MINIMAL IR-TEST ONLY)
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "config.h"

#define HAL_GPIO_INPUT  0
#define HAL_GPIO_OUTPUT 1
#define HAL_LOW  0
#define HAL_HIGH 1

void halGpioMode(uint8_t pin, uint8_t mode);
void halDigitalWrite(uint8_t pin, uint8_t val);
uint16_t halAdcRead(uint8_t pin);
void halDelayMs(unsigned long ms);

#endif /* HAL_H_ */
