#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"

#define NUM_WEAPON_STATES 2

typedef struct {
    float X;
    float Y;
    float Width;
    float Height;
    float RotationAngle;
    float WalkSpeed;
    float TurnSpeed;

	// Shooting mechanic.
	int32_t WeaponSprites[NUM_WEAPON_STATES];
	float ShootFrequency;
	float NextShootTime;
	int32_t BasicWeaponDamage;
} player_t;


extern player_t Player;


void PlayerMove(float DeltaTime);
void PlayerShoot(float CurrentTime);
void RenderMapPlayer(void);
void OnShootAnimationEnd(sprite_t* CurrentSprite);


#endif // PLAYER_H
