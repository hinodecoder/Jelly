#ifndef CONSTS_H
#define CONSTS_H

#define FALSE 0
#define TRUE 1

#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

#define MINIMAP_SCALE_FACTOR 0.25

#define WINDOW_W (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_H (MAP_NUM_ROWS * TILE_SIZE)

#define FOV_ANGLE (60 * (PI / 180))

#define NUM_RAYS WINDOW_W

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

#endif
