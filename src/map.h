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
bool LoadMap(int32_t MapId);


// MAP LOGIC
// _____________________________________________________________________
#define NUM_DOORS 8


typedef struct {
	// Doors location.
	int32_t X;
	int32_t Y;

	// Link to another map.
	int32_t LinkMapId;
	float PlayerAngle;
	bool Valid;
} door_info_t;


typedef struct {
	int32_t Id;
	char FilePath[32];
	door_info_t Doors[NUM_DOORS];
} map_logic_t;


door_info_t* GetDoorInfo(float X, float Y);

void InitializeMapsList(void);

// END MAP LOGIC
// _____________________________________________________________________

#endif /* MAP_H */
