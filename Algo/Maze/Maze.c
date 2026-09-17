/*
 * MAZE.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#include "Maze.h"

cell_t gMaze[MAZE_SIZE][MAZE_SIZE];  // Initialize all cells to zero

int gDirRow[4] = {1, 0, -1, 0};   // NORTH, EAST, SOUTH, WEST
int gDirCol[4] = {0, 1, 0, -1};

void wallSet(unsigned char R, unsigned char C, dir_t dir){
    gMaze[R][C].WALLS[dir] = WALL_PRESENT;
    int nR = R + gDirRow[dir];
    int nC = C + gDirCol[dir];

    /* Set the wall on the adjacent cell */
    if (nR >= 0 && nR < MAZE_SIZE && nC >= 0 && nC < MAZE_SIZE) {
        gMaze[nR][nC].WALLS[dir ^ 2] = WALL_PRESENT;
    }
}

wall_state_t wallCheck(unsigned char R, unsigned char C, dir_t dir){
    return (wall_state_t)gMaze[R][C].WALLS[dir];
}