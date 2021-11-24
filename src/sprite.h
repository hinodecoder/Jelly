#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float X;
    float Y;
    float Distance;
    float Angle;
    int32_t TextureId;
    bool IsVisible;
} sprite_t;

void RenderSpriteProjection(void);
void RenderMapSprites(void);

#endif