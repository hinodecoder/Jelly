#include "entity.h"
#include "sprite.h"

entity_t Entities[NUM_ENTITIES];



void CreateAllEntities(void) {
    for (int32_t i = 0; i < NUM_ENTITIES; ++i) {
        entity_t* CurrentEntity = &Entities[i];
        CurrentEntity->EntityId = i;
        CurrentEntity->MaxHealth = 1;
        CurrentEntity->CurrentHealth = 1;
    }
}

void ApplyDamage(entity_t* CurrentEntity, int32_t Damage) {
    if (CurrentEntity != 0) {
        CurrentEntity->CurrentHealth = CurrentEntity->CurrentHealth - Damage;

        if (CurrentEntity->CurrentHealth <= 0 && CurrentEntity->OnDeath != 0) {
            CurrentEntity->OnDeath(CurrentEntity);
        }
    }
}

// JELLY ENEMY
// _________________________________________________________________________________
void CreateJellyEnemy(entity_t* CurrentEntity, float X, float Y) {
    CurrentEntity->MaxHealth = 4;
    CurrentEntity->CurrentHealth = 4;
    CurrentEntity->OnDeath = &OnJellyDeath;

    sprite_t* CurrentSprite = &Sprites[CurrentEntity->EntityId];
    CurrentSprite->X = X;
    CurrentSprite->Y = Y;
    CurrentSprite->TextureId = 3;
    CurrentSprite->Empty = false;
    CurrentSprite->Scale = 1.0f;


}

void OnJellyDeath(entity_t* CurrentEntity) {
    if (CurrentEntity != 0) {
        sprite_t* HitSprite = &Sprites[CurrentEntity->EntityId];

        if (HitSprite != 0) {
            HitSprite->TextureId = 5;
            HitSprite->Animate = true;
            HitSprite->FramesCount = 50;
            HitSprite->AnimationFrameTime = 100.0f;

        }
    }
}
