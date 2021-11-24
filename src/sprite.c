#include "sprite.h"
#include "graphics.h"
#include "player.h"
#include "textures.h"
#include "consts.h"
#include "math.h"
// ________________________________________________________________________
#define NUM_SPRITES 2
static sprite_t Sprites[NUM_SPRITES] = {
    /*barrel*/{
    .X = 320,
    .Y = 315,
    .TextureId = 2,
    .IsVisible = false
    },
    /*barrel test*/{
    .X = 275,
    .Y = 315,
    .TextureId = 2,
    .IsVisible = false
    }
};
// ________________________________________________________________________
void RenderSpriteProjection(void) {
    // Store all visible sprites to render.
    sprite_t* VisibleSprites[NUM_SPRITES] = {};
    int32_t NumberOfVisibleSprites = 0;

    // Brute-force check all sprites to be rendered or not.
    for (int32_t i=0; i < NUM_SPRITES; ++i) {
        sprite_t* CurrentSprite = &Sprites[i];
        float AngleToSprite = Player.RotationAngle - atan2(CurrentSprite->Y - Player.Y, CurrentSprite->X - Player.X);

        // Clamp angle to be 0-180.
        if (AngleToSprite > PI) {
            AngleToSprite -= TWO_PI;
        }
        if (AngleToSprite < -PI) {
            AngleToSprite += TWO_PI;
        }
        AngleToSprite = fabs(AngleToSprite);

        // If sprite is in field of view of a player then add it to render array.
        if (AngleToSprite < FOV_HALF_ANGLE) {
            CurrentSprite->IsVisible = true;
            CurrentSprite->Angle = AngleToSprite;
            CurrentSprite->Distance = DistanceBetweenPoints(CurrentSprite->X, CurrentSprite->Y, Player.X, Player.Y);
            VisibleSprites[NumberOfVisibleSprites] = CurrentSprite;
            NumberOfVisibleSprites++;
        }
        else {
            CurrentSprite->IsVisible = false;
        }
    }

    // Draw the projected sprites.
    for (int32_t i=0; i < NumberOfVisibleSprites; ++i) {
        // Draw sprite.
    }
}
// ________________________________________________________________________
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