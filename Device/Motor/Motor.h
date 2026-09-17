/*
 * Motor.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef MOTOR_H_
#define MOTOR_H_

/*Section 1: Include Files*/
#include <stdint.h>
#include <stdbool.h>
#include "../../config.h"

/*Section 2: Macros*/
/*Section 3: Macro Functions*/
/*Section 4: User defined data type*/

/*Section 5: Function Prototypes*/

/**
 * @brief Init TB6612FNG pins + ESP32 LEDC PWM. Call once.
 */
void motorsBegin(void);

/**
 * @brief Set left motor PWM (-PWM_MAX..+PWM_MAX). Handles invert + slew + deadband.
 */
void motorLeftSet(int16_t pwm);

/**
 * @brief Set right motor PWM (-PWM_MAX..+PWM_MAX). Handles invert + slew + deadband.
 */
void motorRightSet(int16_t pwm);

/**
 * @brief Set both motors at once.
 */
void motorsSet(int16_t leftPwm, int16_t rightPwm);

/**
 * @brief Immediate stop both motors (bypasses slew).
 */
void motorsStopAll(void);

/**
 * @brief Short-brake both motors (IN1=IN2=HIGH, PWM 0). Use at the end
 * of turns to kill coast overshoot. Next motorsSet() overrides it.
 */
void motorsBrakeAll(void);

#endif /* MOTOR_H_ */
