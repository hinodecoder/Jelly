#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "consts.h"
#include "upng.h"

typedef struct {
    int32_t Width;
    int32_t Height;
    uint32_t* Buffer;
} texture_t;


void LoadTextures(void);
texture_t* GetTexture(int32_t Index);
void FreeTextures(void);

#endif /* TEXTURES_H */
