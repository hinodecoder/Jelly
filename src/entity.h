#ifndef ENTITY_H
#define ENTITY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "consts.h"

// Struct for every entity in game.
typedef struct entity_t{
    // Entity Id to be able match it with sprite.
    int32_t EntityId;

    // Health of that entity.
    int32_t MaxHealth;
    int32_t CurrentHealth;
	float DefrostTime;
	bool CanBeHurt;
	bool Frozen;
    
    // Movement.
    float Speed;

    // Entity AI.
    float ThinkFrequency;
    float NextThinkTime;
    float DestinationX;
    float DestinationY;
    void (*OnThink)(struct entity_t*);
    void (*OnAIAction)(struct entity_t*, float);

    void (*OnUpdate)(struct entity_t*, float, float);
    void (*OnDeath)(struct entity_t*);
	void (*OnFreeze)(struct entity_t*, float Duration, float CurrentTime);
	void (*OnDefrost)(struct entity_t*);
} entity_t;

extern entity_t Entities[NUM_ENTITIES];

// GENERAL ENTITY FUNCTIONS
// ______________________________________________________________________
void CreateAllEntities(void);
void ApplyDamage(entity_t* CurrentEntity, int32_t Damage);
void Freeze(entity_t* CurrentEntity, float Duration, float CurrentTime);
void UpdateEntity(entity_t* CurrentEntity, float DeltaTime, float CurrentTime);
void OnAIUpdate(entity_t* CurrentEntity, float DeltaTime, float CurrentTime);

// JELLY ENEMY
// ______________________________________________________________________
void CreateJellyEnemy(entity_t* CurrentEntity, float X, float Y);
void OnJellyThink(entity_t* JellyEntity);
void OnJellyChasePlayer(entity_t* JellyEntity, float DeltaTime);
void OnJellyWander(entity_t* JellyEntity, float DeltaTime);
void OnJellyFreeze(entity_t* JellyEntity, float Duration, float CurrentTime);
void OnJellyDefrost(entity_t* JellyEntity);
void OnJellyDeath(entity_t* CurrentEntity);

// FRIENDLY AI
void CreateFriendlyAI(entity_t* CurrentEntity, float X, float Y);
void OnFriendlyThink(entity_t* JellyEntity);

#endif
