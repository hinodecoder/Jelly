#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    float X;
    float Y;
    float Width;
    float Height;
    float RotationAngle;
    float WalkSpeed;
    float TurnSpeed;
} player_t;

extern player_t Player;


void MovePlayer(float DeltaTime);
void RenderMapPlayer(void);


#endif // PLAYER_H
