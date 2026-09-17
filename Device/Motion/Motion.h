/*
 * Motion.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef MOTION_H_
#define MOTION_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../config.h"

/* Init encoders + motors. Call once. */
void motionBegin(void);

/*
 * Progress callback, called from inside drive/turn loops so the
 * main sketch can stream telemetry (e.g. over Bluetooth) while
 * the robot is moving. remainCounts = distance left (drive) or
 * angle error x100 (turn). hErr = heading error in degrees.
 * May be NULL (no reports). Keep the callback short.
 */
typedef void (*motion_progress_t)(long remainCounts, int16_t pwmL,
                                  int16_t pwmR, float hErr);
void motionSetProgressCb(motion_progress_t cb);

/* Drive straight mm (>0 fwd). Returns true if reached in time. */
bool motionDistanceMm(float mm, uint8_t cruisePwm);

/* Maze primitives. Return true on success. */
bool motionForwardOneCell(void);
bool motionTurnLeft90(void);
bool motionTurnRight90(void);
bool motionTurn180(void);

#endif /* MOTION_H_ */
