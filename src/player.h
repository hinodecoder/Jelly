#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"

#define NUM_WEAPON_STATES 3
#define MAX_HEALTH 3

typedef struct {
    float X;
    float Y;
    float Width;
    float Height;
    float RotationAngle;
    float WalkSpeed;
    float TurnSpeed;
	float InteractionDistance;

	// Health
	int32_t MaxHealth;
	int32_t Health;

	// HUD
	int32_t HealthUI[3];

	// Shooting mechanic.
	int32_t WeaponSprites[NUM_WEAPON_STATES];
	float ShootFrequency;
	float NextShootTime;
	int32_t BasicWeaponDamage;
	float WeaponAltModeDuration; // Entity frozen time.
} player_t;


extern player_t Player;


void InitPlayer(void);
void PlayerMove(float DeltaTime);
void PlayerShoot(float CurrentTime);
void PlayerOpenDoors(void);
void RenderMapPlayer(void);
void OnShootAnimationEnd(sprite_t* CurrentSprite);
void OnPlayerDamage(void);


#endif // PLAYER_H
