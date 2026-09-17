/*
 * FLOODFILL.c
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#include "Floodfill.h"

extern cell_t gMaze[MAZE_SIZE][MAZE_SIZE];
extern int gDirRow[4];
extern int gDirCol[4];
queue_t gQueue;
queue_node gQueueList[MAZE_SIZE * MAZE_SIZE];

void floodFillInit(point_t *goals, int numGoals){

    queueInit(&gQueue, gQueueList, MAZE_SIZE * MAZE_SIZE);
    int count = 0;
    int r = 0, c = 0;

        for(c = 0; c < MAZE_SIZE; c++){
            for(r = 0; r < MAZE_SIZE; r++){
            switch(r){
                case 0:
                    gMaze[r][c].WALLS[SOUTH] = WALL_PRESENT;
                    break;
                case MAZE_SIZE - 1:
                    gMaze[r][c].WALLS[NORTH] = WALL_PRESENT;
                    break;
                default:
                    break;
            }
            switch(c){
                case 0:
                    gMaze[r][c].WALLS[WEST] = WALL_PRESENT;
                    break;
                case MAZE_SIZE - 1:
                    gMaze[r][c].WALLS[EAST] = WALL_PRESENT;
                    break;
                default:
                    break;
            }
        }
    }

    /*Initialize goal positions*/
    for(count = 0; count < numGoals; count++){
        switch(count){
            case 0:
                goals[count].R = GOAL_C1;
                goals[count].C = GOAL_C1;
                break;
            case 1:
                goals[count].R = GOAL_C1;
                goals[count].C = GOAL_C2;
                break;
            case 2:
                goals[count].R = GOAL_C2;
                goals[count].C = GOAL_C1;
                break;
            case 3:
                goals[count].R = GOAL_C2;
                goals[count].C = GOAL_C2;
                break;
            default:
                break;
        }
    }
}

void floodFill(point_t *goals, int numGoals){
    int count = 0;
    dir_t dir = NORTH;
    point_t poppedCell, nCell;
    cell_processing_state_t cell_state[MAZE_SIZE][MAZE_SIZE] = {CELL_UNPROCESSED};
    for(count = 0; count < numGoals; count++){
        gMaze[goals[count].R][goals[count].C].DISTANCE = 0;
        enqueue(&gQueue, goals[count]);
        cell_state[goals[count].R][goals[count].C] = CELL_PROCESSED;
    }
    while(QUEUE_EMPTY != queueStatus(&gQueue)){
        dequeue(&gQueue, &poppedCell);
        for(dir = NORTH; dir <= WEST; dir = (dir_t)(dir + 1)){
            nCell.R = poppedCell.R + gDirRow[dir];
            nCell.C = poppedCell.C + gDirCol[dir];
            if((nCell.R >= 0) && (nCell.R < MAZE_SIZE) && (nCell.C >= 0) && (nCell.C < MAZE_SIZE)){
                if((CELL_UNPROCESSED == cell_state[nCell.R][nCell.C]) 
                    && (WALL_PRESENT != wallCheck(poppedCell.R, poppedCell.C, dir))){
                    gMaze[nCell.R][nCell.C].DISTANCE = gMaze[poppedCell.R][poppedCell.C].DISTANCE + 1;
                    cell_state[nCell.R][nCell.C] = CELL_PROCESSED;
                    enqueue(&gQueue, nCell);
                }
            }
        }
    }
}

dir_t getNextMove(point_t robPos, dir_t heading, robot_state_t state){
    dir_t nextMove = heading;
    dir_t dir;
    int count = 0;
    unsigned char myDist = gMaze[robPos.R][robPos.C].DISTANCE;
    unsigned char nDist = 0;

    dir_t orderExploring[4] = {
        (dir_t)((heading + 1) % 4),        //Right
        heading,                           //Forward
        (dir_t)((heading + 3) % 4),        //Left
        (dir_t)(heading ^ 2)               //Backward
    };
    dir_t orderReturning[4] = {
        (dir_t)((heading + 3) % 4),
        heading,
        (dir_t)((heading + 1) % 4),
        (dir_t)(heading ^ 2)
    };
    dir_t *order = (state == STATE_EXPLORING_TO_CENTER)? orderExploring : orderReturning;
    for(count = 0; count <= 3; count++){
        dir = order[count];
        if((robPos.R + gDirRow[dir] >= 0) && (robPos.R + gDirRow[dir] < MAZE_SIZE) && (robPos.C + gDirCol[dir] >= 0) && (robPos.C + gDirCol[dir] < MAZE_SIZE)){
            nDist = gMaze[robPos.R + gDirRow[dir]][robPos.C + gDirCol[dir]].DISTANCE;
            if(NO_WALL == wallCheck(robPos.R, robPos.C, dir) && nDist < myDist){
                nextMove = dir;
                break;
            }
        }
    }
    return nextMove;
}



