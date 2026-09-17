/*
 * FLOODFILL.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef FLOODFILL_H_
#define FLOODFILL_H_

/*Section 1: Include Files*/
#include "../Maze/Maze.h"
#include "../Queue/FIFO.h"
/*Section 2: Macros*/
/*Section 3: Macro Functions*/
/*Section 4: User defined data type*/
typedef enum{
    CELL_UNPROCESSED,
    CELL_PROCESSED
}cell_processing_state_t;

typedef enum {
    STATE_EXPLORING_TO_CENTER,
    STATE_RETURNING_TO_START,
    STATE_FINAL_RUN
} robot_state_t;

typedef queue_node point_t;
/*Section 5: Function Prototypes*/

/**
 * @brief Initializes the flood fill algorithm
 * 
 */
void floodFillInit(point_t *goals, int numGoals);

/**
 * @brief Executes the flood fill algorithm
 * 
 */
void floodFill(point_t *goals, int numGoals);

/**
 * @brief Gets the next move for the robot based on flood fill distances
 * @param robPos The current position of the robot
 * @param heading The current heading of the robot
 * @return The direction of the next move
 */
dir_t getNextMove(point_t robPos, dir_t heading, robot_state_t state);

#endif /* FLOODFILL_H_ */
