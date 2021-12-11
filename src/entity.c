#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "entity.h"
#include "sprite.h"
#include "math.h"
#include "player.h"
#include "map.h"

entity_t Entities[NUM_ENTITIES];


// AI CONFIGURATION
// _____________________________________________________________
#define CHASE_DISTANCE_TO_PLAYER 120.0f
#define AI_SEEK_DISTANCE 10.0f
#define JELLY_SPEED 10.0f
#define JELLY_BOOST_SPEED 50.0f
#define JELLY_SUICIDE_DISTANCE 15.0f
// _____________________________________________________________


void CreateAllEntities(void) {
    // Initialize random number generator.
    srand((unsigned) time(0));
    
    for (int32_t i = 0; i < NUM_ENTITIES; ++i) {
        entity_t* CurrentEntity = &Entities[i];
        CurrentEntity->EntityId = i;
        CurrentEntity->MaxHealth = 1;
        CurrentEntity->CurrentHealth = 1;
		CurrentEntity->CanBeHurt = 1;
		CurrentEntity->Frozen = 0;
		CurrentEntity->DefrostTime = 0.0f;
        
        CurrentEntity->Speed = 0.0f;
        
        CurrentEntity->ThinkFrequency = 2000.0;
        CurrentEntity->NextThinkTime = 0;
        CurrentEntity->DestinationX = 0;
        CurrentEntity->DestinationY = 0;
        
        CurrentEntity->OnDeath = 0;
        CurrentEntity->OnUpdate = 0;
        CurrentEntity->OnAIAction = 0;
        CurrentEntity->OnThink = 0;
		CurrentEntity->OnFreeze = 0;
		CurrentEntity->OnDefrost = 0;
    }
}

bool IsEntityDead(entity_t* CurrentEntity) {
	return CurrentEntity->CurrentHealth <= 0.0f;
}

void ApplyDamage(entity_t* CurrentEntity, int32_t Damage) {
	if (CurrentEntity != 0) {
		//if (CurrentEntity->CanBeHurt) {
			CurrentEntity->CurrentHealth = CurrentEntity->CurrentHealth - Damage;
			if (CurrentEntity->CurrentHealth <= 0 && CurrentEntity->OnDeath != 0) {
				CurrentEntity->OnDeath(CurrentEntity);
			}
		//}
		// TODO: Add callback when entity is invincible and we need to show player some feedback.
	}
}

void Freeze(entity_t* CurrentEntity, float Duration, float CurrentTime) {
	if (CurrentEntity != 0) {
		if (CurrentEntity->OnFreeze != 0) {
			CurrentEntity->OnFreeze(CurrentEntity, Duration, CurrentTime);
		}
	}
}

void UpdateEntity(entity_t* CurrentEntity, float DeltaTime, float CurrentTime){
    if (CurrentEntity) {
        if (CurrentEntity->OnUpdate != 0 && !IsEntityDead(CurrentEntity)) {
            CurrentEntity->OnUpdate(CurrentEntity, DeltaTime, CurrentTime);
        }
    }
}

void OnAIUpdate(entity_t* CurrentEntity, float DeltaTime, float CurrentTime) {
	if (CurrentEntity == 0) {
		return;
	} 

	// Handle frozen status.
	if (CurrentEntity->Frozen) {
		if (CurrentEntity->DefrostTime <= CurrentTime) {
			CurrentEntity->Frozen = 0;	
			if (CurrentEntity->OnDefrost) {
				CurrentEntity->OnDefrost(CurrentEntity);
			}
		}
		else {
			// We are still frozen so any AI logic doesn't make sense.
			return;
		}
	}

	// Let AI think it.
	if (CurrentEntity->NextThinkTime <= CurrentTime) {
		CurrentEntity->NextThinkTime = CurrentEntity->ThinkFrequency + CurrentTime;

		if (CurrentEntity->OnThink) {
			CurrentEntity->OnThink(CurrentEntity);
		}
	}

	// Execute action chosen by AI.
	if (CurrentEntity->OnAIAction) {
		CurrentEntity->OnAIAction(CurrentEntity, DeltaTime);
	}
}


// JELLY ENEMY
// _________________________________________________________________________________
void CreateJellyEnemy(entity_t* CurrentEntity, float X, float Y) {
	// Health stuff.
    CurrentEntity->MaxHealth = 4;
    CurrentEntity->CurrentHealth = 4;
	CurrentEntity->CanBeHurt = 0;

	// Some events and "polymorphic" functions.
    CurrentEntity->OnDeath = &OnJellyDeath;
    CurrentEntity->OnUpdate = &OnAIUpdate;
    CurrentEntity->OnThink = &OnJellyThink;
	CurrentEntity->OnFreeze = &OnJellyFreeze;
	CurrentEntity->OnDefrost = &OnJellyDefrost;

    CurrentEntity->Speed = JELLY_SPEED;
    
	// Set random think frequency to look less dumb.
    int32_t RandomNumber = (rand() % 20 + 18);
    CurrentEntity->ThinkFrequency = (float) (RandomNumber * 100.0f);

	// Setup basic sprite.
    sprite_t* CurrentSprite = &Sprites[CurrentEntity->EntityId];
    CurrentSprite->X = X;
    CurrentSprite->Y = Y;
    CurrentSprite->TextureId = 1;
    CurrentSprite->Empty = false;
    CurrentSprite->Scale = 0.5f;
    
    // Jelly animation.
    CurrentSprite->CurrentFrame = 0;
    CurrentSprite->FramesCount = 5;
    CurrentSprite->Animate = true;
    CurrentSprite->AnimationFrameTime = 200.0f;
}

void GetRandomDirection(float* X, float* Y) {
    *X = 0;
    *Y = 0;
    int32_t Direction = rand() % 3;
    if (Direction == 0) {
        *X = -1;
    } else if (Direction == 1) {
        *X = 1;
    } else if (Direction == 2) {
        *Y = -1;
    } else if (Direction == 3) {
        *Y = 1;
    }
    
    *X *= TILE_SIZE;
    *Y *= TILE_SIZE;
}

void GetBestFirstDirection(float* X, float* Y, float CurrentX, float CurrentY) {
    *X = 0.0f;
    *Y = 0.0f;
    
    const bool HasWallUp = MapHasWallAt(CurrentX, CurrentY + TILE_SIZE);
    if (!HasWallUp) {
        *Y = TILE_SIZE;
        return;
    }
    
    const bool HasWallLeft = MapHasWallAt(CurrentX - TILE_SIZE, CurrentY);
    if (!HasWallLeft) {
        *X = -TILE_SIZE;
        return;
    }
    
    const bool HasWallRight = MapHasWallAt(CurrentX + TILE_SIZE, CurrentY);
    if (!HasWallRight) {
        *X = TILE_SIZE;
        return;
    }
    
    const bool HasWallDown = MapHasWallAt(CurrentX, CurrentY - TILE_SIZE);
    if (!HasWallDown) {
        *Y = -TILE_SIZE;
        return;
    }
}

void CalculateJellyWanderDestination(entity_t* JellyEntity) {
    sprite_t* JellySprite = &Sprites[JellyEntity->EntityId];
    if (JellySprite) {
        // Get random direction.
        float DirectionX = 0.0f;
        float DirectionY = 0.0f;
        GetRandomDirection(&DirectionX, &DirectionY);
        JellyEntity->DestinationX = JellySprite->X + DirectionX;
        JellyEntity->DestinationY = JellySprite->Y + DirectionY;
        
        // Check if there is a wall.
        const bool HitWall = MapHasWallAt(JellyEntity->DestinationX, JellyEntity->DestinationY);
        if (HitWall) {
            // We've hit wall, need better direction.
            GetBestFirstDirection(&DirectionX, &DirectionY, JellySprite->X, JellySprite->Y);
            JellyEntity->DestinationX = JellySprite->X + DirectionX;
            JellyEntity->DestinationY = JellySprite->Y + DirectionY;
        }
    }
}

void CalculateDirectionToPlayer(entity_t* CurrentEntity) {
    sprite_t* CurrentSprite = &Sprites[CurrentEntity->EntityId];
    if (CurrentSprite) {
        float DirectionX = Player.X - CurrentSprite->X;
        float DirectionY = Player.Y - CurrentSprite->Y;
        CurrentEntity->DestinationX = CurrentSprite->X + DirectionX;
        CurrentEntity->DestinationY = CurrentSprite->Y + DirectionY;
    }
}

void OnJellyThink(entity_t* JellyEntity) {
    sprite_t* JellySprite = &Sprites[JellyEntity->EntityId];
    
    // Calculate distance to player.
    float DistanceToPlayer = DistanceBetweenPoints(JellySprite->X, JellySprite->Y, Player.X, Player.Y);
    
    // TODO: Calculate if this is straight line to player before start chasing.
    if (DistanceToPlayer <= CHASE_DISTANCE_TO_PLAYER) {
        JellyEntity->OnAIAction = &OnJellyChasePlayer;
    }
    else {
        CalculateJellyWanderDestination(JellyEntity);
        JellyEntity->OnAIAction = &OnJellyWander;
    }
}

void OnJellyChasePlayer(entity_t* JellyEntity, float DeltaTime) {
    // Boost Jelly speed to chase player.
    JellyEntity->Speed = JELLY_BOOST_SPEED;
    
    // Do move.
    CalculateDirectionToPlayer(JellyEntity);
    OnJellyWander(JellyEntity, DeltaTime);

	// Check distance to player.
	sprite_t* JellySprite = &Sprites[JellyEntity->EntityId];
	float DistanceToPlayer = DistanceBetweenPoints(Player.X, Player.Y, JellySprite->X, JellySprite->Y);
	if (DistanceToPlayer <= JELLY_SUICIDE_DISTANCE) {
		JellyEntity->CanBeHurt = true;
		ApplyDamage(JellyEntity, 9999); // Just kill myself.
		OnPlayerDamage();
	}
    
    // Return to normal speed.
    JellyEntity->Speed = JELLY_SPEED;
}

void OnJellyWander(entity_t* JellyEntity, float DeltaTime) {
    sprite_t* JellySprite = &Sprites[JellyEntity->EntityId];
    if (JellySprite && (JellyEntity->DestinationX != 0.0f || JellyEntity->DestinationY != 0.0f)) {
        float DeltaX = JellyEntity->DestinationX - JellySprite->X;
        float DeltaY = JellyEntity->DestinationY - JellySprite->Y;
        NormalizeVector2D(&DeltaX, &DeltaY);
        DeltaX *= JellyEntity->Speed * DeltaTime;
        DeltaY *= JellyEntity->Speed * DeltaTime;
        
        const float FinalDestinationX = JellySprite->X + DeltaX;
        const float FinalDestinationY = JellySprite->Y + DeltaY;

		// Check if destination reached.
		float DistanceToDestination = DistanceBetweenPoints(JellySprite->X, JellySprite->Y, JellyEntity->DestinationX, JellyEntity->DestinationY);
		if (DistanceToDestination <= AI_SEEK_DISTANCE) {
			JellyEntity->DestinationX = 0.0f;
			JellyEntity->DestinationY = 0.0f;
			return;
		}

		if (!MapHasWallAt(FinalDestinationX, JellySprite->Y)) {
			JellySprite->X = FinalDestinationX;
		}

		if (!MapHasWallAt(JellySprite->X, FinalDestinationY)) {
			JellySprite->Y = FinalDestinationY;
		}
	}
}

void OnJellyDeath(entity_t* CurrentEntity) {
    if (CurrentEntity != 0) {
        sprite_t* HitSprite = &Sprites[CurrentEntity->EntityId];

        if (HitSprite != 0) {
			HitSprite->IsVisible = false;
			HitSprite->Empty = true;
        }
        
        CurrentEntity->CurrentHealth = 0;
    }
}

void OnJellyFreeze(entity_t* JellyEntity, float Duration, float CurrentTime) {
	if (JellyEntity != 0) {
		JellyEntity->Frozen = 1;
		JellyEntity->DefrostTime = Duration + CurrentTime;
		JellyEntity->CanBeHurt = 1; // Jelly can be hurt if frozen.

		// Set new animation.
		sprite_t* JellySprite = &Sprites[JellyEntity->EntityId];
		if (JellySprite) {
			JellySprite->CurrentFrame = 0;
			JellySprite->TextureId = 2;
			JellySprite->Animate = false;
		}
	}
}

void OnJellyDefrost(entity_t* JellyEntity) {
	if (JellyEntity) {
		JellyEntity->CanBeHurt = 0;

		// Set new animation.
		sprite_t* JellySprite = &Sprites[JellyEntity->EntityId];
		if (JellySprite) {
			JellySprite->CurrentFrame = 0;
			JellySprite->TextureId = 1;
			JellySprite->Animate = true;
		}
	}
}
