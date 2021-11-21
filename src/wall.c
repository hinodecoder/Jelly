#include "wall.h"

#define WALL_SHADOW_FACTOR 0.7f

void RenderWallProjection(void) {
    for (int x=0; x < NUM_RAYS; x++) {
        float PerpendicularDistance = Rays[x].Distance * cos(Rays[x].RayAngle - Player.RotationAngle);
        
        float ProjectedWallHeight = (TILE_SIZE / PerpendicularDistance) * DISTANCE_TO_PROJECTION_PLANE;
        
        // TODO: Replace, duplicate variable
        int WallStripHeight = ProjectedWallHeight;
        
        int WallTopPixel = (WINDOW_H / 2) - (WallStripHeight / 2);
        WallTopPixel = WallTopPixel < 0 ? 0 : WallTopPixel;
        
        int WallBottomPixel = (WINDOW_H / 2) + (WallStripHeight / 2);
        WallBottomPixel = WallBottomPixel > WINDOW_H ? WINDOW_H : WallBottomPixel;
        
        // Color for ceiling.
        for (int y = 0; y < WallTopPixel; ++y) {
            WriteColorBuffer(x, y, 0xff000000);
        }
        
        int32_t TextureOffsetX = 0;
        
        // Base on vertical or horizontal hit we take remainder of proper axis.
        if (Rays[x].WasHitVertical) {
            TextureOffsetX = (int32_t) Rays[x].WallHitY % TILE_SIZE;
        } else {
            TextureOffsetX = (int32_t) Rays[x].WallHitX % TILE_SIZE;
        }
        
        
        // TODO: Add some null checks
        texture_t* CurrentTexture = GetTexture(Rays[x].WallHitContent - 1);
        uint32_t* TextureBuffer = CurrentTexture->Buffer;
        const int32_t TextureW = CurrentTexture->Width;
        const int32_t TextureH = CurrentTexture->Height;
        
        // Render the wall from top to bottom pixels.
        for (int y = WallTopPixel; y < WallBottomPixel; ++y) {
            // NOTE: Calculate distance from top because we do clamping on top pixel height and it
            // distorts our view while mapping a texture.
            int32_t DistanceFromTop = (y + WallStripHeight * 0.5f) - (WINDOW_H * 0.5f);
            
            // NOTE: Calculating texture offset in Y but we need to take "perspective" into consideration.
            // To calculate it we need overall wall height (wall strip height).
            int32_t TextureOffsetY = DistanceFromTop * ((float)TextureH / WallStripHeight);
            
            // Set color from sample texture.
            uint32_t TexelColor = TextureBuffer[(TextureW * TextureOffsetY) + TextureOffsetX];
            
            if (Rays[x].WasHitVertical) {
                // TODO: Maybe use different textures? In case we need optimization (a little).
                ChangeColorIntensity(&TexelColor, WALL_SHADOW_FACTOR);
            }
            
            WriteColorBuffer(x, y, TexelColor);
        }
        
        // Color for floor.
        for (int y = WallBottomPixel; y < WINDOW_H; ++y) {
            WriteColorBuffer(x, y, 0xff131313);
        }
    }
}
