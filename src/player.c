#include <stdio.h>
#include "player.h"
#include "consts.h"
#include "map.h"
#include "graphics.h"
#include "math.h"
#include "input.h"
#include "ray.h"
#include "sprite.h"
#include "entity.h"


player_t Player = {
    .X = 5 * TILE_SIZE,
    .Y = 2 * TILE_SIZE,
    .Width = 10,
    .Height = 10,
    .RotationAngle = -HALF_PI,
    .WalkSpeed = 100,// 60,
    .TurnSpeed = 8 * (PI / 180)
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

void PlayerMove(float DeltaTime) {
    int32_t WalkDirection = 0;
    int32_t Turn = 0;
    int32_t StrafeDirection = 0;
    GetCurrentMoveData(&WalkDirection, &Turn, &StrafeDirection);
    
    // ROTATION
    // ___________________________________________
    // Rotate player based on turn value and speed.
    float RotationAmount = Turn * Player.TurnSpeed * DeltaTime;
    RotationAmount = Clamp(RotationAmount, -Player.TurnSpeed, Player.TurnSpeed);
    Player.RotationAngle += RotationAmount;
    
    // Important thing to do: always normalize rotation angle!
    NormalizeAngle(&Player.RotationAngle);

    // MOVEMENT
    // ___________________________________________
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

void PlayerShoot() {
    if (Keys[EKEY_SHOOT]) {
        int32_t BlockedId = Rays[CENTER_RAY].BlockedBy;
        entity_t* HitEntity = &Entities[BlockedId];

        if (HitEntity != 0) {
            ApplyDamage(HitEntity, 1);
        }
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
