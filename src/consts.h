#ifndef CONSTS_H
#define CONSTS_H

#include <math.h>

#define PI 3.14159265
#define TWO_PI 6.28318530
#define HALF_PI PI / 2

#define TILE_SIZE 32

#define MINIMAP_SCALE_FACTOR 0.35

#define WINDOW_W 800
#define WINDOW_H 600

#define FOV_ANGLE (60 * (PI / 180))
#define FOV_HALF_ANGLE FOV_ANGLE/2

#define NUM_RAYS WINDOW_W
#define CENTER_RAY NUM_RAYS/2

// TODO: Move to some kind of a function?
#define DISTANCE_TO_PROJECTION_PLANE ((WINDOW_W / 2) / tan(FOV_ANGLE / 2))

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

#endif
