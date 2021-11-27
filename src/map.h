#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdint.h>

#define MAP_NUM_ROWS 16
#define MAP_NUM_COLS 16

bool MapHasWallAt(float X, float Y);
int32_t GetMapAt(int32_t I, int32_t J);
bool IsInsideMap(float X, float Y);
bool LoadMap(const char* MapFilePath);

#endif /* MAP_H */
