#include "map.h"
#include "consts.h"
#include "upng.h"
#include "player.h"
#include "sprite.h"

#define NUM_OF_OBJECT_TYPES 4

static uint32_t Map[MAP_NUM_ROWS][MAP_NUM_COLS];

// Check TileColors array and set proper enum tile indices
// This indices are direct references into TileColors array.
enum ObjectTypes {
    EOBJECT_EMPTY = 0,
    EOBJECT_SAMPLE_WALL = 1,
    EOBJECT_PLAYER = 2,
    EOBJECT_GHOST = 3
};

// TileColors directly map color with object type in map png file.
static uint32_t TileColors[NUM_OF_OBJECT_TYPES] = {
    /*0*/0xffffffff, // empty floor
    /*1*/0xff32283f, // sample wall
    /*2*/0xff62e7ff, // player
    /*3*/0xff443be5, // ghost enemy
};


bool MapHasWallAt(float X, float Y) {
    if (X < 0 || X >= MAP_NUM_COLS * TILE_SIZE || Y < 0 || Y >= MAP_NUM_ROWS * TILE_SIZE) {
        return true;
    }
    
    int MapGridIndexX = floor(X / TILE_SIZE);
    int MapGridIndexY = floor(Y / TILE_SIZE);
    
    return Map[MapGridIndexY][MapGridIndexX] != 0;
}

int32_t GetMapAt(int32_t I, int32_t J) {
    return Map[I][J];
}

bool IsInsideMap(float X, float Y) {
    return (X >= 0 && X <= MAP_NUM_COLS * TILE_SIZE && Y >= 0 && Y <= MAP_NUM_ROWS * TILE_SIZE);
}

int32_t GetTileFromColor(uint32_t Color) {
    for (int32_t i=0; i < NUM_OF_OBJECT_TYPES; i++) {
        if (TileColors[i] == Color) {
            return i;
        }
    }
    return 0;
}

void GetTileCenterPosition(int32_t x, int32_t y, float *OutX, float *OutY) {
    // We take x,y coordinates + 1 tile and we move player to center of that
    // tile with TILE_SIZE / 2 calculation.
    *OutX = ((x + 1) * TILE_SIZE) - (TILE_SIZE / 2);
    *OutY = ((y + 1) * TILE_SIZE) - (TILE_SIZE / 2);
}

bool LoadMap(const char* MapFilePath) {
    upng_t* PngFile = upng_new_from_file(MapFilePath);
    
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
                int32_t ObjectType = GetTileFromColor(MapColor);
                
                // Check if this is a tile marked as "player".
                if (ObjectType == EOBJECT_PLAYER) {
                    // Set player start position.
                    GetTileCenterPosition(x, y, &Player.X, &Player.Y);
                    
                    // Clear space by setting it as empty.
                    ObjectType = EOBJECT_EMPTY;
                }
                
                // Handling spawning of ghost enemy
                if (ObjectType == EOBJECT_GHOST) {
                    if (CurrentSpriteIndex < NUM_SPRITES) {
                        sprite_t* GhostSprite = &Sprites[CurrentSpriteIndex++];
                        GetTileCenterPosition(x, y, &GhostSprite->X, &GhostSprite->Y);
                        GhostSprite->TextureId = 3;
                        GhostSprite->Empty = false;
                        GhostSprite->Scale = 1.0f;
                    }
                    
                    ObjectType = EOBJECT_EMPTY;
                }
                
                Map[y][x] = ObjectType;
            }
            
            NextFreeSpriteIndex = CurrentSpriteIndex + 1;
        }
    }
    
    upng_free(PngFile);
    
    return true;
}
