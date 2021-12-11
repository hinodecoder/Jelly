#include <stdio.h>
#include <string.h>
#include "map.h"
#include "consts.h"
#include "upng.h"
#include "player.h"
#include "sprite.h"
#include "entity.h"

static map_object_t Map[MAP_NUM_ROWS][MAP_NUM_COLS];

map_object_t MapObjectsDefinitions[NUM_MAP_OBJECTS] = {
	{
		.Type = EOBJECT_EMPTY,
		.ColorCode = 0,
		.TextureId = 0
	},
	{
		.Type = EOBJECT_BASIC_WALL,
		.ColorCode = 0xff32283f,
		.TextureId = 7
	},
	{
		.Type = EOBJECT_PLAYER,
		.ColorCode = 0xff62e7ff,
		.TextureId = 0
	},
	{
		.Type = EOBJECT_GHOST,
		.ColorCode = 0xff443be5,
		.TextureId = 0
	},
	{
		.Type = EOBJECT_DOORS,
		.ColorCode = 0xfffa1579,
		.TextureId = 9
	},
	{
		.Type = EOBJECT_LIGHTS_WALL,
		.ColorCode = 0,
		.TextureId = 0
	},
	{
		.Type = EOBJECT_COCKPIT_COMPUTER,
		.ColorCode = 0xff3b0d76,
		.TextureId = 8
	},
	{
		.Type = EOBJECT_DOOR_CLOSED,
		.ColorCode = 0xff003ba2,
		.TextureId = 10
	},
	{
		.Type = EOBJECT_WALL_WINDOW,
		.ColorCode = 0xff6b5389,
		.TextureId = 11
	},
};





// MAPS INITIALIZATION AND LOGIC DEFINITION
// __________________________________________________________________________________________
#define NUM_MAPS 8
#define MAX_MAP_FILE_NAME 32

// List of all maps in the game.
char Maps[NUM_MAPS][MAX_MAP_FILE_NAME] = {
	"./data/maps/cockpit.png",
	"./data/maps/corridor.png",
	"./data/maps/arena.png"
};

// List of logic definitions for all maps.
map_logic_t MapLogic[NUM_MAPS];

// The map we're currently playing.
int32_t CurrentMapId;

void ClearDoorInfo(map_logic_t* CurrentLogic) {
	for (int32_t i=0; i < NUM_DOORS; ++i) {
		CurrentLogic->Doors[i].Valid = false;
	}
}


// LIST OF MAP LOGICS
// __________________________________________________________________________________________
void CreateLogicForMap_01(map_logic_t* CurrentLogic) {
	if (CurrentLogic) {
		// TODO: Create logic here. It's messy as hell...
		door_info_t* Door = &CurrentLogic->Doors[0];
		Door->Valid = true;
		Door->X = 7;
		Door->Y = 10;
		Door->LinkMapId = 1;
		Door->PlayerAngle = -PI / 2;
	}
}

void CreateLogicForMap_02(map_logic_t* CurrentLogic) {
	if (CurrentLogic) {
		{
			door_info_t* Door = &CurrentLogic->Doors[0];
			Door->Valid = true;
			Door->X = 7;
			Door->Y = 15;
			Door->LinkMapId = 0;
			Door->PlayerAngle = -PI / 2;
		}

		{
			door_info_t* Door = &CurrentLogic->Doors[1];
			Door->Valid = true;
			Door->X = 4;
			Door->Y = 6;
			Door->LinkMapId = 2;
			Door->PlayerAngle = PI;
		}
	}
}

void CreateLogicForMap_03(map_logic_t* CurrentLogic) {
	if (CurrentLogic) {
		door_info_t* Door = &CurrentLogic->Doors[0];
		Door->Valid = true;
		Door->X = 13;
		Door->Y = 11;
		Door->LinkMapId = 1;
		Door->PlayerAngle = -PI;
	}
}
// __________________________________________________________________________________________

// Array with functions pointers to create proper logic for all maps.
void (*MapLogicCreationFunctions[NUM_MAPS])(map_logic_t*) = {
	&CreateLogicForMap_01,
	&CreateLogicForMap_02,
	&CreateLogicForMap_03
};

// Create proper maps definitions on game start.
void InitializeMapsList(void) {
	for (int32_t i=0; i < NUM_MAPS; ++i) {
		map_logic_t* Logic = &MapLogic[i];
		Logic->Id = i;
		memcpy(&Logic->FilePath, &Maps[i], sizeof(char) * MAX_MAP_FILE_NAME);
		if (MapLogicCreationFunctions[i]) {
			MapLogicCreationFunctions[i](Logic); // Create actual logic for that map "i"
		}
	}
}
// END OF: MAPS INITIALIZATION AND LOGIC DEFINITION
// __________________________________________________________________________________________

bool MapHasDoorsAt(float X, float Y) {
    if (X < 0 || X >= MAP_NUM_COLS * TILE_SIZE || Y < 0 || Y >= MAP_NUM_ROWS * TILE_SIZE) {
        return true;
    }

    int MapGridIndexX = floor(X / TILE_SIZE);
    int MapGridIndexY = floor(Y / TILE_SIZE); 

    return Map[MapGridIndexY][MapGridIndexX].Type == EOBJECT_DOORS;
}


bool MapHasWallAt(float X, float Y) {
    if (X < 0 || X >= MAP_NUM_COLS * TILE_SIZE || Y < 0 || Y >= MAP_NUM_ROWS * TILE_SIZE) {
        return true;
    }
    
    int MapGridIndexX = floor(X / TILE_SIZE);
    int MapGridIndexY = floor(Y / TILE_SIZE); 

    return Map[MapGridIndexY][MapGridIndexX].Type != EOBJECT_EMPTY;
}

map_object_t GetMapAt(int32_t I, int32_t J) {
    return Map[I][J];
}

bool IsInsideMap(float X, float Y) {
    return (X >= 0 && X <= MAP_NUM_COLS * TILE_SIZE && Y >= 0 && Y <= MAP_NUM_ROWS * TILE_SIZE);
}

map_object_t GetTileFromColor(uint32_t Color) {
    for (int32_t i=0; i < NUM_MAP_OBJECTS; i++) {
        if (MapObjectsDefinitions[i].ColorCode == Color) {
            return MapObjectsDefinitions[i];
        }
    }
    return MapObjectsDefinitions[0];
}

void GetTileCenterPosition(int32_t x, int32_t y, float *OutX, float *OutY) {
    // We take x,y coordinates + 1 tile and we move player to center of that
    // tile with TILE_SIZE / 2 calculation.
    *OutX = ((x + 1) * TILE_SIZE) - (TILE_SIZE / 2);
    *OutY = ((y + 1) * TILE_SIZE) - (TILE_SIZE / 2);
}

void ClearMap(void) {
	InitializeSprites();
	NextFreeSpriteIndex = 0;
	for (int32_t i=0; i < MAP_NUM_COLS; i++) {
		for (int32_t j=0; j < MAP_NUM_ROWS; j++) {
			Map[i][j] = MapObjectsDefinitions[0];
		}
	}
}

bool LoadMap(int32_t MapId) {
	ClearMap();
	CurrentMapId = MapId;
	map_logic_t* CurrentMapLogic = &MapLogic[MapId];

    upng_t* PngFile = upng_new_from_file(CurrentMapLogic->FilePath);
    
    if (PngFile == 0) {
        return false;
    }
    
    upng_decode(PngFile);
    
    if (upng_get_error(PngFile) == UPNG_EOK) {
        // Get buffer of map texture.
        uint32_t* Buffer = (uint32_t*) upng_get_buffer(PngFile);
        
        // TODO: Remove this later, this is only for testing map loading
        int32_t CurrentSpriteIndex = 0;
        
        // Reasign map with data from the buffer.
        for (int32_t y = 0; y < MAP_NUM_COLS; ++y) {
            for (int32_t x = 0; x < MAP_NUM_ROWS; ++x) {
                uint32_t MapColor = Buffer[(MAP_NUM_COLS * y) + x];
                map_object_t ObjectType = GetTileFromColor(MapColor);
                
                // PLAYER
                // ____________________________________________________
                if (ObjectType.Type == EOBJECT_PLAYER) {
                    // Set player start position.
                    GetTileCenterPosition(x, y, &Player.X, &Player.Y);
                    
                    // Clear space by setting it as empty.
                    ObjectType.Type = EOBJECT_EMPTY;
               }
                
                // JELLY ENEMY
                // ____________________________________________________
                if (ObjectType.Type == EOBJECT_GHOST) {
                    if (CurrentSpriteIndex < NUM_SPRITES) {
                        // Setup entity object.
                        float PositionX;
                        float PositionY;
                        GetTileCenterPosition(x, y, &PositionX, &PositionY);
                        CreateJellyEnemy(&Entities[CurrentSpriteIndex], PositionX, PositionY);
                        CurrentSpriteIndex++;
                    }
                    
                    ObjectType.Type = EOBJECT_EMPTY;
                }
                
                Map[y][x] = ObjectType;
            }
            
            NextFreeSpriteIndex = CurrentSpriteIndex + 1;
        }
	}
	else {
		printf("Cannot load map!\n");
	}

    upng_free(PngFile);
    
    return true;
}


// GAME MAP LOGIC

door_info_t* GetDoorInfo(float X, float Y) {
	if (X < 0 || X >= MAP_NUM_COLS * TILE_SIZE || Y < 0 || Y >= MAP_NUM_ROWS * TILE_SIZE) {
        return 0;
    }

	int MapGridIndexX = floor(X / TILE_SIZE);
	int MapGridIndexY = floor(Y / TILE_SIZE);

	for (int32_t i = 0; i < NUM_DOORS; ++i) {
		door_info_t* Door = &MapLogic[CurrentMapId].Doors[i];
		if (Door && Door->Valid) {
			if (Door->X == MapGridIndexX && Door->Y == MapGridIndexY) {
				return Door;
            }
		}
	}
	return 0;
}
