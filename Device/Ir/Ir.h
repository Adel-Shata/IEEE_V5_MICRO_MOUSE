/*
 * Ir.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef IR_H_
#define IR_H_

/*Section 1: Include Files*/
#include "../../Hal.h"

/*Section 2: Macros*/
#define IR_NUM_SENSORS 5

/*Section 3: Macro Functions*/

/*Section 4: User defined data type*/
typedef enum {
    IR_SENSOR_RIGHT = 0,
    IR_SENSOR_FRONT,
    IR_SENSOR_LEFT,
    IR_SENSOR_TOP_LEFT,
    IR_SENSOR_TOP_RIGHT
} ir_sensor_id_t;

typedef struct {
    int16_t AMBIENT;
    int16_t RAW;
    int16_t FILTERED;
} ir_reading_t;

/*Section 5: Function Prototypes*/

/**
 * @brief Initializes the IR sensors
 * 
 */
void irInit(void);

/**
 * @brief Reads all IR sensors
 * 
 */
void irReadAll(void);

/**
 * @brief Reads a specific IR sensor
 * @param sensor The ID of the sensor to read
 * @return The reading from the sensor
 */
uint16_t irRead(ir_sensor_id_t sensor);

/**
 * @brief Checks if there is a wall in front of the robot
 * @return 1 if there is a wall, 0 otherwise
 * @note Self-contained: fires the front LED and refreshes its reading.
 *       Does NOT need irReadAll() first.
 */
uint8_t irWallFront(void);

/**
 * @brief Checks if there is a wall to the left of the robot
 * @return 1 if there is a wall, 0 otherwise
 * @note Self-contained: fires the left LED and refreshes its reading.
 *       Does NOT need irReadAll() first.
 */
uint8_t irWallLeft(void);

/**
 * @brief Checks if there is a wall to the right of the robot
 * @return 1 if there is a wall, 0 otherwise
 * @note Self-contained: fires the right LED and refreshes its reading.
 *       Does NOT need irReadAll() first.
 */
uint8_t irWallRight(void);

#endif /* IR_H_ */
