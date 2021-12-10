#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdint.h>

#define MAP_NUM_ROWS 16
#define MAP_NUM_COLS 16

enum ObjectTypes {
    EOBJECT_EMPTY,
    EOBJECT_BASIC_WALL,
    EOBJECT_PLAYER,
    EOBJECT_GHOST,
    EOBJECT_DOORS,
	EOBJECT_LIGHTS_WALL,
	EOBJECT_COCKPIT_COMPUTER,
};

typedef struct map_object_t {
	int32_t Type;
	uint32_t ColorCode;
	int32_t TextureId;
} map_object_t;

#define NUM_MAP_OBJECTS 7
extern map_object_t MapObjectsDefinitions[NUM_MAP_OBJECTS];


bool MapHasWallAt(float X, float Y);
bool MapHasDoorsAt(float X, float Y);
map_object_t GetMapAt(int32_t I, int32_t J);
bool IsInsideMap(float X, float Y);
bool LoadMap(const char* MapFilePath);

// NAVIGATION FUNCTIONS
// _____________________________________________________________

#endif /* MAP_H */
