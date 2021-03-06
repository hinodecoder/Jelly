#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>
#include "consts.h"


// This is struct for every sprite on map.
// ________________________________________________________________
// @ IsVisible -> Calculated realtime.
// @ Empty -> It is not initialised yet so do not render anyway.
// ________________________________________________________________
typedef struct sprite_t {
    // Sprite id. Corresponds to entity id.
    int32_t SpriteId;

    // Sprite location (could be in world or screen coordinates)
    float X;
    float Y;
    
    // Distance from camera to sprite
    float Distance;
    
    // Angle between camera and sprite
    float Angle;
    
    // Used for scaling
    float Scale;
    
    // Index of texture from textures array
    int32_t TextureId;
    
    // Tells if this sprite is visible and should be rendered
    bool IsVisible;
    
    // TODO: Change some bools here to set of flags
    // If TRUE: this sprite has been initialized and can be draw.
    // If FALSE: this means that this is sprite that hasn't been initialized yet
    // and it waits in sprites pool to be used later (ex. when loaded in new map)
    bool Empty;
    
    // TODO: Use sprite struct for 2d sprite in screen coordinates
    bool Use2D;
    
    // Current animation frame.
    int32_t CurrentFrame;
    
    // Animation time (every X ms)
    float AnimationFrameTime;

    // Time we go to next frame.
    float NextTimeFrame;
    
    // Enable or disable sprite animation.
    bool Animate;
    
    // Frames count.
    int32_t FramesCount;

	// Callback when animation ends.
	void(*OnAnimationEnd)(struct sprite_t*);
} sprite_t;


extern sprite_t Sprites[NUM_SPRITES];
extern int32_t NextFreeSpriteIndex; // This is index of next sprite we could use.


// Simply initialize all sprites with default values and prepare to be used.
void InitializeSprites(void);
sprite_t* CreateSprite(int32_t TextureId, float X, float Y);
void RenderSpriteProjection(void);
void RenderMapSprites(void);
void Render2DSprite(sprite_t* CurrentSprite);
void UpdateAnimatedSprites(float CurrentTime);

#endif
