#include "sprite.h"
#include "graphics.h"
#include "player.h"
#include "textures.h"
#include "consts.h"
#include "math.h"
#include "ray.h"

sprite_t Sprites[NUM_SPRITES];
int32_t NextFreeSpriteIndex;

void InitializeSprites(void) {
    for (int i=0; i < NUM_SPRITES; ++i) {
        sprite_t* CurrentSprite = &Sprites[i];
        CurrentSprite->X = -1;
        CurrentSprite->Y = -1;
        CurrentSprite->TextureId = -1;
        CurrentSprite->Angle = 0;
        CurrentSprite->Distance = -1;
        CurrentSprite->IsVisible = false;
        CurrentSprite->Scale = 1; // TODO: After applying scale -> move sprite by it's size / 2 to place it on ground
        CurrentSprite->Empty = true;
        CurrentSprite->Use2D = false;
        
        // Default animation settings.
        CurrentSprite->Animate = false;
        CurrentSprite->CurrentFrame = 0;
        CurrentSprite->AnimationFrameTime = 0.0f;
        CurrentSprite->FramesCount = 1;
    }
    
    NextFreeSpriteIndex = 0;
}

void Render2DSprite(sprite_t* CurrentSprite) {
    texture_t* Texture = GetTexture(CurrentSprite->TextureId);
    int32_t MaxWidth = Texture->Width / CurrentSprite->FramesCount;
    int32_t StartPixel = CurrentSprite->Animate ? CurrentSprite->CurrentFrame * MaxWidth : 0;
    
    for (int32_t x = StartPixel; x < MaxWidth + StartPixel; ++x) {
        for (int32_t y = 0; y < Texture->Height; ++y) {
            uint32_t TexelColor = Texture->Buffer[(Texture->Width * y) + x];
            DrawPixel(x + CurrentSprite->X - StartPixel, y + CurrentSprite->Y, TexelColor);
        }
    }
}

void RenderSpriteProjection(void) {
    // Store all visible sprites to render.
    sprite_t* VisibleSprites[NUM_SPRITES] = {};
    int32_t NumberOfVisibleSprites = 0;

    // Brute-force check all sprites to be rendered or not.
    for (int32_t i=0; i < NUM_SPRITES; ++i) {
        sprite_t* CurrentSprite = &Sprites[i];
        
        // Skip uninitialised sprite.
        if (CurrentSprite->Empty) {
            continue;
        }
        
        if (CurrentSprite->Use2D) {
            Render2DSprite(CurrentSprite);
            continue;
        }
        
        float AngleToSprite = Player.RotationAngle - atan2(CurrentSprite->Y - Player.Y, CurrentSprite->X - Player.X);

        // Clamp angle to be 0-180.
        if (AngleToSprite > PI) {
            AngleToSprite -= TWO_PI;
        }
        if (AngleToSprite < -PI) {
            AngleToSprite += TWO_PI;
        }
        AngleToSprite = fabs(AngleToSprite);

        // If sprite is in field of view of a player then add it to render array. (+margin)
        const float SPRITE_MARGIN_EPSILON = 0.2f;
        // Basically allow to draw sprite if it's a little outside of FOV.
        if (AngleToSprite < FOV_HALF_ANGLE + SPRITE_MARGIN_EPSILON) {
            CurrentSprite->IsVisible = true;
            CurrentSprite->Angle = AngleToSprite;
            CurrentSprite->Distance = DistanceBetweenPoints(CurrentSprite->X, CurrentSprite->Y, Player.X, Player.Y);
            VisibleSprites[NumberOfVisibleSprites] = CurrentSprite;
            NumberOfVisibleSprites++;

            // TODO: Instead of bubble sorting below maybe this can happen inplace?
        }
        else {
            CurrentSprite->IsVisible = false;
        }
    }

    // Bubble sort sprites by distance.
    for (int32_t i=0; i < NumberOfVisibleSprites - 1; ++i) {
        for (int32_t j = i + 1; j < NumberOfVisibleSprites; ++j) {
            sprite_t* CurrentSprite = VisibleSprites[i];
            sprite_t* NextSprite = VisibleSprites[j];

            if (CurrentSprite->Distance < NextSprite->Distance) {
                VisibleSprites[i] = NextSprite;
                VisibleSprites[j] = CurrentSprite;
            }
        } 
    }

    // Draw the projected sprites.
    for (int32_t i=0; i < NumberOfVisibleSprites; ++i) {
        sprite_t* CurrentSprite = VisibleSprites[i];

        // Calculate perpendicular distance to avoid fish-eye effect.
        float PerpendicularDistance = CurrentSprite->Distance * cos(CurrentSprite->Angle);

        // Calculate sprite projected height. Width is the same as height (32x32, 64x64).
        float SpriteHeight = (TILE_SIZE / PerpendicularDistance) * DISTANCE_TO_PROJECTION_PLANE;
        float SpriteWidth = SpriteHeight;

        // TODO: Test scaling and implement proper offset
        SpriteWidth *= CurrentSprite->Scale;
        SpriteHeight *= CurrentSprite->Scale;

        // Top pixel coords calculation.
        float SpriteTopPixel = (WINDOW_H / 2) - (SpriteHeight / 2);
        SpriteTopPixel = (SpriteTopPixel < 0) ? 0 : SpriteTopPixel;
        
        // Bottom pixel coords calculation.
        float SpriteBottomPixel = (WINDOW_H / 2) + (SpriteHeight / 2);
        SpriteBottomPixel = (SpriteBottomPixel > WINDOW_H) ? WINDOW_H : SpriteBottomPixel;

        // Calculate sprite position on the projection plane
        float SpriteAngle = atan2(CurrentSprite->Y - Player.Y, CurrentSprite->X - Player.X) - Player.RotationAngle;
        float SpritePositionX = tan(SpriteAngle) * DISTANCE_TO_PROJECTION_PLANE;

        // Calculate sprite left X
        float SpriteLeftX = (WINDOW_W / 2) + SpritePositionX - (SpriteWidth / 2);

        // Calculate sprite right X
        float SpriteRightX = SpriteLeftX + SpriteWidth;

        // Query texture W and H.
        // TODO: Add check if texture ID is valid.
        texture_t* CurrentTexture = GetTexture(CurrentSprite->TextureId);
        int32_t TextureWidth = CurrentTexture->Width;
        int32_t TextureHeight = CurrentTexture->Height;

        // Draw sprite.
        // TODO: Optimize, performance problem when close to camera (filling screen)
        for (int32_t x=SpriteLeftX; x < SpriteRightX; ++x) {
            float TexelWidth = (TextureWidth / SpriteWidth);
            int32_t TextureOffsetX = (x - SpriteLeftX) * TexelWidth;
            for (int32_t y = SpriteTopPixel; y < SpriteBottomPixel; ++y) {
                int32_t DistanceFromTop = y + (SpriteHeight / 2) -  (WINDOW_H / 2);
                int32_t TextureOffsetY = DistanceFromTop * (TextureHeight / SpriteHeight);
                if (x > 0 && x < WINDOW_W && y > 0 && y < WINDOW_H) {
                    // Get texel and draw.
                    uint32_t* TextureBuffer = CurrentTexture->Buffer;
                    uint32_t TexelColor = TextureBuffer[(TextureWidth * TextureOffsetY) + TextureOffsetX];

                    // Check if current pixel is behind a wall.
                    const bool IsPixelBehindWall = CurrentSprite->Distance > Rays[x].Distance;
                    
                    if (!IsPixelBehindWall) {
                        DrawPixel(x, y, TexelColor);
                        
                        // (NOTE): Store information in ray struct that it hit something other than wall.
                        // This is my simple way to cache information about ray blocked by sprite.
                        // It will be important in game mechanics involving ray casting.
                        Rays[x].BlockedBy = CurrentSprite;
                    }
                }
            }
        }
    }
}

void RenderMapSprites(void) {
    for (int i=0; i < NUM_SPRITES; ++i) {
        sprite_t* CurrentSprite = &Sprites[i];
        DrawRectangle(
            CurrentSprite->X * MINIMAP_SCALE_FACTOR,
            CurrentSprite->Y * MINIMAP_SCALE_FACTOR,
            2,
            2,
            CurrentSprite->IsVisible ? 0xffffffff : 0xff0000ff
            );
    }
}
