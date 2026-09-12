#include "FLOODFILL.h"

cell_t gMaze[MAZE_SIZE][MAZE_SIZE];

int gDirRow[4] = {1, 0, -1, 0};   // NORTH, EAST, SOUTH, WEST
int gDirCol[4] = {0, 1, 0, -1};

direction_t opposite(direction_t d) {
    return (direction_t)((d + 2) % 4);
}

void setWall(int r, int c, direction_t dir) {
    gMaze[r][c].WALLS |= (1 << dir);

    int nr = r + gDirRow[dir];
    int nc = c + gDirCol[dir];

    if (nr >= 0 && nr < MAZE_SIZE && nc >= 0 && nc < MAZE_SIZE) {
        gMaze[nr][nc].WALLS |= (1 << opposite(dir));
    }
}

bool hasWall(int r, int c, direction_t dir) {
    return (gMaze[r][c].WALLS & (1 << dir)) != 0;
}

void floodFill(point_t goals[], int num_goals) { //goals[4] = {(7,7), (7,8), (8,7), (8,8)}
    point_t queue[MAZE_SIZE * MAZE_SIZE];
    int head = 0, tail = 0;

    for (int r = 0; r < MAZE_SIZE; r++)
        for (int c = 0; c < MAZE_SIZE; c++)
            gMaze[r][c].DISTANCE = MAX_DISTANCE;

    for (int i = 0; i < num_goals; i++) {
        gMaze[goals[i].r][goals[i].c].DISTANCE = 0;
        queue[tail++] = goals[i];
    }

    while (head < tail) {
        point_t cur = queue[head++];
        unsigned char cur_dist = gMaze[cur.r][cur.c].DISTANCE;

        for (direction_t dir = NORTH; dir <= WEST; dir++) {
            if (hasWall(cur.r, cur.c, dir)) continue;

            int nr = cur.r + gDirRow[dir];
            int nc = cur.c + gDirCol[dir];
            if (nr < 0 || nr >= MAZE_SIZE || nc < 0 || nc >= MAZE_SIZE) continue;

            if (gMaze[nr][nc].DISTANCE > cur_dist + 1) {
                gMaze[nr][nc].DISTANCE = cur_dist + 1;
                queue[tail++] = (point_t){nr, nc};
            }
        }
    }
}

direction_t chooseNextMove(int r, int c, direction_t facing, robot_state_t state) { //Use getNextMove not this
    unsigned char my_dist = gMaze[r][c].DISTANCE;
    direction_t best = facing;

    direction_t order_exploring[4] = {
        (direction_t)((facing + 1) % 4),  //Right
        facing,                           //Forward
        (direction_t)((facing + 3) % 4),  //Left
        opposite(facing)                  //Backward
    };
    direction_t order_returning[4] = {
        (direction_t)((facing + 3) % 4),
        facing,
        (direction_t)((facing + 1) % 4),
        opposite(facing)
    };

    direction_t *order = (state == STATE_EXPLORING_TO_CENTER)
                        ? order_exploring : order_returning;

    for (int i = 0; i < 4; i++) {
        direction_t dir = order[i];
        if (hasWall(r, c, dir)) continue;

        int nr = r + gDirRow[dir];
        int nc = c + gDirCol[dir];
        if (nr < 0 || nr >= MAZE_SIZE || nc < 0 || nc >= MAZE_SIZE) continue;

        if (gMaze[nr][nc].DISTANCE < my_dist) {
            best = dir;
            break;
        }
    }
    return best;
}

int computeFinalPath(point_t start, point_t goals[], int num_goals, direction_t path_out[], int max_len) {
    floodFill(goals, num_goals);

    int r = start.r, c = start.c;
    int steps = 0;

    while (gMaze[r][c].DISTANCE != 0 && steps < max_len) {
        unsigned char my_dist = gMaze[r][c].DISTANCE;
        direction_t chosen_dir = NORTH;
        bool moved = false;

        for (direction_t dir = NORTH; dir <= WEST; dir++) {
            if (hasWall(r, c, dir)) continue;

            int nr = r + gDirRow[dir];
            int nc = c + gDirCol[dir];
            if (nr < 0 || nr >= MAZE_SIZE || nc < 0 || nc >= MAZE_SIZE) continue;

            if (gMaze[nr][nc].DISTANCE == my_dist - 1) {
                chosen_dir = dir;
                moved = true;
                break;
            }
        }

        if (!moved) break;   //Noway out - Error in map

        path_out[steps++] = chosen_dir;
        r += gDirRow[chosen_dir];
        c += gDirCol[chosen_dir];
    }
    return steps;
}

direction_t getNextMove(int r, int c, direction_t facing, robot_state_t state, //*Use This
                         direction_t final_path[], int *final_path_index) {
    if (state == STATE_FINAL_RUN) {
        if (final_path == NULL || final_path_index == NULL) {
            return facing; // safety
        }
        direction_t dir = final_path[*final_path_index];
        (*final_path_index)++;
        return dir;
    }
    return chooseNextMove(r, c, facing, state);
}

#ifndef MMS_BUILD
int main(void) {
    memset(gMaze, 0, sizeof(gMaze));

    point_t center[4] = {{7,7}, {7,8}, {8,7}, {8,8}};
    point_t start[1]  = {{0, 0}};

    // --- مرحلة EXPLORING ---
    floodFill(center, 4);
    printf("قيمة الخلية الأولى (0,0) قبل أي اكتشاف: %d\n", gMaze[0][0].DISTANCE);

    setWall(0, 0, EAST);
    floodFill(center, 4);
    printf("قيمة الخلية بعد اكتشاف حيطة: %d\n", gMaze[0][0].DISTANCE);

    // --- مرحلة RETURNING ---
    floodFill(start, 1);
    printf("قيمة خلية المركز كهدف=البداية دلوقتي: %d\n", gMaze[8][8].DISTANCE);

    // --- مرحلة FINAL_RUN (جديد) ---
    direction_t final_path[MAZE_SIZE * MAZE_SIZE];
    int path_len = computeFinalPath(start[0], center, 4, final_path, MAZE_SIZE * MAZE_SIZE);
    printf("عدد خطوات المسار النهائي: %d\n", path_len);

    int final_index = 0;
    int r = 0, c = 0;
    direction_t facing = NORTH;
    for (int i = 0; i < path_len; i++) {
        facing = getNextMove(r, c, facing, STATE_FINAL_RUN, final_path, &final_index);
        r += gDirRow[facing];
        c += gDirCol[facing];
    }
    printf("بعد الـ final run، الروبوت وصل لـ (%d,%d)\n", r, c);
    return 0;
}
#endif
