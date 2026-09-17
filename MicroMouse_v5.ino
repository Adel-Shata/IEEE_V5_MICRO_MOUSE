/*
 * MicroMouse_v5.ino (WALL BOUNCE, NO BLUETOOTH)
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 *
 *  Drives forward one cell. If front wall -> turn 180 -> drive again.
 *  No Bluetooth: nothing may block/slow the sense-drive loop.
 *  One short USB line per decision only (never inside motion).
 */

#include "Hal.h"
#include "Device/Ir/Ir.h"
#include "Device/Motor/Motor.h"
#include "Device/Encoder/Encoder.h"
#include "Device/Imu/Imu.h"
#include "Device/Motion/Motion.h"
#include "Device/Ir/Ir.c"
#include "Device/Motor/Motor.c"
#include "Device/Encoder/Encoder.c"
#include "Device/Imu/Imu.c"
#include "Device/Motion/Motion.c"
#include "Algo/FloodFill/Floodfill.h"
#include "Algo/FloodFill/Floodfill.c"
#include "Algo/Maze/Maze.h"
#include "Algo/Maze/Maze.c"
#include "Algo/Queue/FIFO.h"
#include "Algo/Queue/FIFO.c"
extern int gDirRow[4];
extern int gDirCol[4];
extern cell_t gMaze[MAZE_SIZE][MAZE_SIZE];
point_t gEndGoal[NUM_GOAL_CELLS];
point_t gStartPos = {0, 0};
dir_t robDir = NORTH;
int nextDir = 1;
robot_state_t state = STATE_EXPLORING_TO_CENTER;
point_t robPos = {0, 0};

void setup() {
    
    Serial.begin(SERIAL_BAUD);
    delay(500);
    if (!imuBegin()) { while (1) delay(500); } /* IMU missing: halt */
    irInit();
    motionBegin();
    delay(3000); /* place robot, keep still: gyro already calibrated */
    floodFillInit(gEndGoal, NUM_GOAL_CELLS);
}

void loop() {
    bool ok;
    imuUpdate();
    if (irWallFront()) { /* self-contained: fires front LED, fresh reading */
        Serial.println("U180");
        ok = motionTurn180();
        Serial.println(ok ? "U180 OK" : "U180 TIMEOUT");
    } else {
        Serial.println("FWD");
        ok = motionForwardOneCell();
        Serial.println(ok ? "FWD OK" : "FWD STOPPED");
    }
    
    if (irWallLeft()) {
        wallSet(robPos.R, robPos.C, (dir_t)((robDir + 3) % 4));
    }
    if (irWallFront()) {
        wallSet(robPos.R, robPos.C, robDir);
    }
    if (irWallRight()) {
        wallSet(robPos.R, robPos.C, (dir_t)((robDir + 1) % 4));
    }
        
    if(state == STATE_EXPLORING_TO_CENTER && gMaze[robPos.R][robPos.C].DISTANCE == 0) {
        state = STATE_RETURNING_TO_START;
    }

    if(state == STATE_EXPLORING_TO_CENTER){
        floodFill(gEndGoal, NUM_GOAL_CELLS);
    }
    else if(state == STATE_RETURNING_TO_START){
        floodFill(&gStartPos, 1);
    }
            
    nextDir = getNextMove(robPos, robDir, state);

    if(nextDir == robDir) {
        ok = motionForwardOneCell();
        if (!ok) {
            Serial.println("Crash detected!");
        }
        robPos.R += gDirRow[robDir];
        robPos.C += gDirCol[robDir];
    } else if (nextDir == (robDir + 1) % 4) {
        ok = motionTurnLeft90();
        robDir = (dir_t)((robDir + 1) % 4);
    } else if (nextDir == (robDir + 3) % 4) {
        ok = motionTurnRight90();
        robDir = (dir_t)((robDir + 3) % 4);
    } else if (nextDir == (robDir ^ 2)) {
        ok = motionTurn180();
        robDir = (dir_t)(robDir ^ 2);
    }
}

