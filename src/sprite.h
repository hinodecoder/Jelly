#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>


// This is struct for every sprite on map.
// ________________________________________________________________
// @ IsVisible -> Calculated realtime.
// @ Empty -> It is not initialised yet so do not render anyway.
// ________________________________________________________________
typedef struct {
    float X;
    float Y;
    float Distance;
    float Angle;
    float Scale;
    int32_t TextureId;
    bool IsVisible;
    bool Empty;
} sprite_t;


#define NUM_SPRITES 16
extern sprite_t Sprites[NUM_SPRITES];


void InitializeSprites(void);
void RenderSpriteProjection(void);
void RenderMapSprites(void);

#endif
