#ifndef FLOODFILL_H_
#define FLOODFILL_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAZE_SIZE 16
#define MAX_DISTANCE 255

typedef enum {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
} direction_t;

typedef enum {
    STATE_EXPLORING_TO_CENTER,
    STATE_RETURNING_TO_START,
    STATE_FINAL_RUN
} robot_state_t; //RobotState

typedef struct {
    unsigned char DISTANCE;
    unsigned char WALLS;   // bit0=NORTH, bit1=EAST, bit2=SOUTH, bit3=WEST
} cell_t; //Cell

direction_t opposite(direction_t d);

void setWall(int r, int c, direction_t dir);

bool hasWall(int r, int c, direction_t dir);

typedef struct { int r, c; } point_t;

void floodFill(point_t goals[], int num_goals);

direction_t chooseNextMove(int r, int c, direction_t facing, robot_state_t state);

int computeFinalPath(point_t start, point_t goals[], int num_goals,direction_t path_out[], int max_len);

direction_t getNextMove(int r, int c, direction_t facing, robot_state_t state, direction_t final_path[], int *final_path_index);

extern cell_t gMaze[MAZE_SIZE][MAZE_SIZE];
extern int gDirRow[4];
extern int gDirCol[4];


#endif /*FLOODFILL_H_*/

