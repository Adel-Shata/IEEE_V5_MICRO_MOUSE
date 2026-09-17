/*
 * MAZE.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef MAZE_H_
#define MAZE_H_


/*Section 1: Include Files*/
#include "../../config.h"
/*Section 2: Macros*/
/*Section 3: Macro Functions*/
/*Section 4: User defined data type*/

typedef enum{
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
}dir_t;

typedef enum{
    NO_WALL = 0,
    WALL_PRESENT
}wall_state_t;

typedef struct{
    unsigned int DISTANCE;
    unsigned char WALLS[4];
}cell_t;

/*Section 5: Function Prototypes*/

/**
 * @brief Set the Wall object
 * 
 * @param R 
 * @param C 
 * @param dir 
 */
void wallSet(unsigned char R, unsigned char C, dir_t dir);

/**
 * @brief Check if a wall exists at a specific position and direction
 * 
 * @param R 
 * @param C 
 * @param dir 
 * @return wall_state_t 
 */
wall_state_t wallCheck(unsigned char R, unsigned char C, dir_t dir);

#endif /* MAZE_H_ */