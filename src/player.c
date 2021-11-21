#include "player.h"
#include "consts.h"
#include "map.h"
#include "graphics.h"

player_t Player = {
    .X = 4 * TILE_SIZE,
    .Y = 5 * TILE_SIZE,
    .Width = 30,
    .Height = 30,
    .TurnDirection = 0,
    .WalkDirection = 0,
    .RotationAngle = PI / 2,
    .StrafeDirection = 0,
    .WalkSpeed = 200,
    .TurnSpeed = 85 * (PI / 180)
};

void MovePlayer(float DeltaTime) {
    Player.RotationAngle += Player.TurnDirection * Player.TurnSpeed * DeltaTime;
    float MoveStep = Player.WalkDirection * Player.WalkSpeed * DeltaTime;

    // TODO: Do strafing, use StrafeDirection
    
    float NewPlayerX = Player.X + cos(Player.RotationAngle) * MoveStep;
    float NewPlayerY = Player.Y + sin(Player.RotationAngle) * MoveStep;

    if (!MapHasWallAt(NewPlayerX, NewPlayerY)) {
        Player.X = NewPlayerX;
        Player.Y = NewPlayerY;
    }
}

void RenderPlayer(void) {
    DrawRectangle(
                  Player.X * MINIMAP_SCALE_FACTOR,
                  Player.Y * MINIMAP_SCALE_FACTOR,
                  Player.Width * MINIMAP_SCALE_FACTOR,
                  Player.Height * MINIMAP_SCALE_FACTOR,
                  0xff0000ff,
                  WINDOW_W
                  );
}
