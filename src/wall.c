#include "wall.h"

#define WALL_SHADOW_FACTOR 0.7f

void RenderWallProjection(void) {
    for (int x=0; x < NUM_RAYS; x++) {
        // Calculate perpendicular distance to avoid fish-eye effect (distortion).
        float PerpendicularDistance = Rays[x].Distance * cos(Rays[x].RayAngle - Player.RotationAngle);
        
        // Calculate projected wall height.
        float WallHeight = (TILE_SIZE / PerpendicularDistance) * DISTANCE_TO_PROJECTION_PLANE;
        
        int WallTopPixel = (WINDOW_H / 2) - (WallHeight / 2);
        WallTopPixel = WallTopPixel < 0 ? 0 : WallTopPixel;
        
        int WallBottomPixel = (WINDOW_H / 2) + (WallHeight / 2);
        WallBottomPixel = WallBottomPixel > WINDOW_H ? WINDOW_H : WallBottomPixel;
        
        // Draw ceiling.
        for (int y = 0; y < WallTopPixel; ++y) {
            DrawPixel(x, y, 0xff000000);
        }
        
        int32_t TextureOffsetX = 0;
        
        // Base on vertical or horizontal hit we take remainder of proper axis.
        if (Rays[x].WasHitVertical) {
            TextureOffsetX = (int32_t) Rays[x].WallHitY % TILE_SIZE;
        } else {
            TextureOffsetX = (int32_t) Rays[x].WallHitX % TILE_SIZE;
        }
        
        
        // TODO: Add some null checks
        texture_t* CurrentTexture = GetTexture(Rays[x].TextureId - 1);
        uint32_t* TextureBuffer = CurrentTexture->Buffer;
        const int32_t TextureW = CurrentTexture->Width;
        const int32_t TextureH = CurrentTexture->Height;
        
        // Render the wall from top to bottom pixels.
        for (int y = WallTopPixel; y < WallBottomPixel; ++y) {
            // NOTE: Calculate distance from top because we do clamping on top pixel height and it
            // distorts our view while mapping a texture.
            int32_t DistanceFromTop = (y + WallHeight * 0.5f) - (WINDOW_H * 0.5f);
            
            // NOTE: Calculating texture offset in Y but we need to take "perspective" into consideration.
            // To calculate it we need overall wall height.
            int32_t TextureOffsetY = DistanceFromTop * ((float)TextureH / WallHeight);
            
            // Set color from sample texture.
            uint32_t TexelColor = TextureBuffer[(TextureW * TextureOffsetY) + TextureOffsetX];
            
            if (Rays[x].WasHitVertical) {
                ChangeColorIntensity(&TexelColor, WALL_SHADOW_FACTOR);
            }
            
            DrawPixel(x, y, TexelColor);
        }
        
        // Draw floor.
        for (int y = WallBottomPixel; y < WINDOW_H; ++y) {
            DrawPixel(x, y, 0xff131313);
        }
    }
}
