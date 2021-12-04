#ifndef ENTITY_H
#define ENTITY_H

#include <stdlib.h>
#include <stdint.h>
#include "consts.h"

typedef struct entity_t{
    // Entity Id to be able match it with sprite.
    int32_t EntityId;

    // Health of that entity.
    int32_t MaxHealth;
    int32_t CurrentHealth;

    void (*OnDeath)(struct entity_t*);
} entity_t;

extern entity_t Entities[NUM_ENTITIES];

void CreateAllEntities(void);
void ApplyDamage(entity_t* CurrentEntity, int32_t Damage);

// Functions for Jelly enemy.
void CreateJellyEnemy(entity_t* CurrentEntity, float X, float Y);
void OnJellyDeath(entity_t* CurrentEntity);

#endif
