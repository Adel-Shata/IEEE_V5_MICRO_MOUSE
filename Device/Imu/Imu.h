/*
 * Imu.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef IMU_H_
#define IMU_H_

/*Section 1: Include Files*/
#include <stdint.h>
#include <stdbool.h>
#include "../../config.h"

/*Section 2: Macros*/
/*Section 3: Macro Functions*/
/*Section 4: User defined data type*/

/*Section 5: Function Prototypes*/

/**
 * @brief Init MPU6050 over I2C + calibrate gyro bias (keep robot still).
 * @return true if sensor found and ready.
 */
bool imuBegin(void);

/**
 * @brief Poll gyro Z and integrate heading. Call as often as possible.
 */
void imuUpdate(void);

/**
 * @brief Integrated yaw in degrees (continuous, + = CCW seen from top).
 */
float imuHeadingDeg(void);

/**
 * @brief Last bias-corrected gyro Z in deg/s.
 */
float imuGyroZDps(void);

/**
 * @brief Zero the heading integrator.
 */
void imuResetHeading(void);

#endif /* IMU_H_ */
