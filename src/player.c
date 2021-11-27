#include <stdio.h>
#include "player.h"
#include "consts.h"
#include "map.h"
#include "graphics.h"
#include "math.h"
#include "input.h"

player_t Player = {
    .X = 4 * TILE_SIZE,
    .Y = 5 * TILE_SIZE,
    .Width = 10,
    .Height = 10,
    .RotationAngle = PI / 2,
    .WalkSpeed = 200,
    .TurnSpeed = 5 * (PI / 180)
};

void MovePlayer(float DeltaTime) {
    int32_t WalkDirection = 0;
    int32_t Turn = 0;
    //int32_t StrafeDirection = 0;
    
    // Check if we can move in some direction.
    if (Keys[EKEY_UP]) {
        WalkDirection = 1;
    }
    else if (Keys[EKEY_DOWN]) {
        WalkDirection = -1;
    }
    
    // Check turn.
    Turn = MouseState.DeltaX;
   
    Player.RotationAngle += Turn * Player.TurnSpeed * DeltaTime;
    
    // Important thing to do: always normalize rotation angle!
    NormalizeAngle(&Player.RotationAngle);

    // TODO: Do strafing, use StrafeDirection
    
    // Move player if there is no collision.
    float MoveStep = WalkDirection * Player.WalkSpeed * DeltaTime;
    float NewPlayerX = Player.X + cos(Player.RotationAngle) * MoveStep;
    float NewPlayerY = Player.Y + sin(Player.RotationAngle) * MoveStep;

    if (!MapHasWallAt(NewPlayerX, NewPlayerY)) {
        Player.X = NewPlayerX;
        Player.Y = NewPlayerY;
    }
}

void RenderMapPlayer(void) {
    DrawRectangle(
                  Player.X * MINIMAP_SCALE_FACTOR,
                  Player.Y * MINIMAP_SCALE_FACTOR,
                  Player.Width * MINIMAP_SCALE_FACTOR,
                  Player.Height * MINIMAP_SCALE_FACTOR,
                  0xff0000ff
                  );
}
