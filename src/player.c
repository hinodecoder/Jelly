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
    .RotationAngle = 0,
    .WalkSpeed = 100,
    .TurnSpeed = 5 * (PI / 180)
};

void GetCurrentMoveData(int32_t *WalkDirection, int32_t *Turn, int32_t *StrafeDirection) {
    if (Keys[EKEY_UP]) {
        *WalkDirection = 1;
    }
    else if (Keys[EKEY_DOWN]) {
        *WalkDirection = -1;
    }
    
    // Check if we should strafe.
    if (Keys[EKEY_LEFT]) {
        *StrafeDirection = -1;
    }
    else if (Keys[EKEY_RIGHT]) {
        *StrafeDirection = 1;
    }
    
    // Check turn.
    *Turn = MouseState.DeltaX;
}

void MovePlayer(float DeltaTime) {
    int32_t WalkDirection = 0;
    int32_t Turn = 0;
    int32_t StrafeDirection = 0;
    GetCurrentMoveData(&WalkDirection, &Turn, &StrafeDirection);
    
    // Rotate player based on turn value and speed.
    Player.RotationAngle += Turn * Player.TurnSpeed * DeltaTime;
    
    // Important thing to do: always normalize rotation angle!
    NormalizeAngle(&Player.RotationAngle);

    // Calculate starfe direction.
    float StrafeX = cos(Player.RotationAngle + HALF_PI) * StrafeDirection;
    float StrafeY = sin(Player.RotationAngle + HALF_PI) * StrafeDirection;
    
    // Walk direction.
    float WalkX = cos(Player.RotationAngle) * WalkDirection;
    float WalkY = sin(Player.RotationAngle) * WalkDirection;
    
    // Combine starfe and walk.
    float DirectionX = StrafeX + WalkX;
    float DirectionY = StrafeY + WalkY;
    
    // Normalize direction and apply walk speed and delta time.
    NormalizeVector2D(&DirectionX, &DirectionY);
    float NewPlayerX = Player.X + (DirectionX * Player.WalkSpeed * DeltaTime);
    float NewPlayerY = Player.Y + (DirectionY * Player.WalkSpeed * DeltaTime);
    
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
