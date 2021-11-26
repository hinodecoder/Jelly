#include <stdio.h>
#include "player.h"
#include "consts.h"
#include "map.h"
#include "graphics.h"
#include "math.h"

player_t Player = {
    .X = 4 * TILE_SIZE,
    .Y = 5 * TILE_SIZE,
    .Width = 10,
    .Height = 10,
    .TurnDirection = 0,
    .WalkDirection = 0,
    .RotationAngle = PI / 2,
    .StrafeDirection = 0,
    .WalkSpeed = 200,
    .TurnSpeed = 85 * (PI / 180)
};

void MovePlayer(float DeltaTime) {
    Player.RotationAngle += Player.TurnDirection * Player.TurnSpeed * DeltaTime;
    // Important thing to do: always normalize rotation angle!
    NormalizeAngle(&Player.RotationAngle);

    // TODO: Do strafing, use StrafeDirection
    
    // Move player if there is no collision.
    float MoveStep = Player.WalkDirection * Player.WalkSpeed * DeltaTime;
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
