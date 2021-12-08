#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "consts.h"
#include "upng.h"

enum textures_e {
	ETEXTURE_WALL = 0,
	ETEXTURE_WALL_2 = 1,
	ETEXTURE_ENEMY = 2,
	ETEXTURE_SKULL = 3,
	ETEXTURE_SHOTGUN_SHOOT = 4,
	ETEXTURE_PLANET = 5,
	ETEXTURE_SHOTGUN_STATIC = 6
}; 

typedef struct {
    int32_t Width;
    int32_t Height;
    uint32_t* Buffer;
} texture_t;


void LoadTextures(void);
texture_t* GetTexture(int32_t Index);
void FreeTextures(void);

#endif /* TEXTURES_H */
