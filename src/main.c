#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "consts.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"
#include "wall.h"
#include "sprite.h"
#include "input.h"
#include "entity.h"
#include "state_machine.h"
#include "game_states.h"
#include "audio.h"


SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
bool IsGameRunning = false;
int TicksLastFrame = 0;
SDL_Texture* ColorBufferTexture = NULL;
int32_t SHOW_CURSOR = 1;

extern void Render(void);



bool InitializeWindow(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// TODO: Doesn't work properly on MacOS with external 5k UltraFine display.
	//SDL_DisplayMode DisplayMode;
	//SDL_GetDisplayMode(0, 0, &DisplayMode);
	//int32_t DisplayWidth = DisplayMode.w;
	//int32_t DisplayHeight = DisplayMode.h;

	int32_t DisplayWidth = 1280;
	int32_t DisplayHeight = 720;

	Window = SDL_CreateWindow( NULL,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			DisplayWidth,
			DisplayHeight,
			//SDL_WINDOW_BORDERLESS);
			//SDL_WINDOW_FULLSCREEN );
		   0);

	SDL_SetWindowTitle(Window, "Jelly");

	if (!Window) {
		fprintf(stderr, "Error creating window.\n");
		return false;
	}

	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);

	if (!Renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
	}

	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);

	return true;
}

void DestroyWindow(void) {
	DestroyColorBuffer();
	SDL_DestroyTexture(ColorBufferTexture);

	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

// TODO: Move to player
void CalculateWeaponScreenPosition(int32_t TextureId, float* X, float* Y, int32_t FramesCount) {
	texture_t* WeaponTexture = GetTexture(TextureId);
	*Y = WINDOW_H - WeaponTexture->Height;
	*X = (WINDOW_W / 2) - ((WeaponTexture->Width / FramesCount) / 2);
}

// TODO: Move somewhere
void CreateHUD(void) {
	// Create static weapon sprite.
	sprite_t* StaticWeaponSprite = CreateSprite(ETEXTURE_GUN_DEFAULT, 0, 0);
	StaticWeaponSprite->Use2D = true;
	StaticWeaponSprite->Empty = false;
	StaticWeaponSprite->IsVisible = true;
	StaticWeaponSprite->Animate = true;
	StaticWeaponSprite->AnimationFrameTime = 100.0f;
	StaticWeaponSprite->FramesCount = 5;

	// Weapon positioning.
	CalculateWeaponScreenPosition(ETEXTURE_GUN_DEFAULT, &StaticWeaponSprite->X, &StaticWeaponSprite->Y, 5);

	// Create freeze shot weapon sprite.
	const static int32_t WEAPON_SHOOT_FRAMES_COUNT = 7;
	sprite_t* ShootWeaponSprite = CreateSprite(ETEXTURE_SHOTGUN_SHOOT, 0, 0);
	ShootWeaponSprite->Use2D = true;
	ShootWeaponSprite->Empty = false;
	ShootWeaponSprite->IsVisible = false;
	ShootWeaponSprite->Animate = true;
	ShootWeaponSprite->AnimationFrameTime = 50.0f;
	ShootWeaponSprite->FramesCount = WEAPON_SHOOT_FRAMES_COUNT;

	CalculateWeaponScreenPosition(ETEXTURE_SHOTGUN_SHOOT, &ShootWeaponSprite->X, &ShootWeaponSprite->Y, WEAPON_SHOOT_FRAMES_COUNT);

	// Create fire shoot sprite.
	
	sprite_t* FireShootSprite = CreateSprite(ETEXTURE_GUN_FIRE_SHOT, 0, 0);
	FireShootSprite->Use2D = true;
	FireShootSprite->Empty = false;
	FireShootSprite->IsVisible = false;
	FireShootSprite->Animate = true;
	FireShootSprite->AnimationFrameTime = 50.0f;
	FireShootSprite->FramesCount = WEAPON_SHOOT_FRAMES_COUNT;

	CalculateWeaponScreenPosition(ETEXTURE_GUN_FIRE_SHOT, &FireShootSprite->X, &FireShootSprite->Y, WEAPON_SHOOT_FRAMES_COUNT);

	Player.WeaponSprites[0] = StaticWeaponSprite->SpriteId;
	Player.WeaponSprites[1] = ShootWeaponSprite->SpriteId;
	Player.WeaponSprites[2] = FireShootSprite->SpriteId;

	// Create health HUD.
	const static int32_t HEALTH_OFFSET = 20;
	int32_t PosX = HEALTH_OFFSET;
	for (int32_t i=0; i < Player.Health; ++i) {
		sprite_t* Health1 = CreateSprite(ETEXTURE_HEART, PosX, 20);
		Health1->Use2D = true;
		Health1->Empty = false;
		Health1->IsVisible = true;
		Health1->TextureId = 12;
		PosX += HEALTH_OFFSET;
		Player.HealthUI[i] = Health1->SpriteId;
	}
}

void UpdateAllEntities(float DeltaTime, float CurrentTime) {
	for (int32_t i=0; i < NUM_ENTITIES; ++i) {
		entity_t* CurrentEntity = &Entities[i];
		UpdateEntity(CurrentEntity, DeltaTime, CurrentTime);
	}
}

// GAMEPLAY STATE FUNCTIONS
// ______________________________________________________________________________________________________________
void GameplayEnter(void) {
	// Important: Create HUD AFTER loading a map due to sprite indexing.
	CreateHUD();
}

void GameplayExit(void) {
}

void GameplayExecute(float DeltaTime, float CurrentTime) {
	PlayerMove(DeltaTime);
	PlayerShoot(TicksLastFrame);
	PlayerOpenDoors();
	UpdateAllEntities(DeltaTime, CurrentTime);

	// Update all animated sprites.
	UpdateAnimatedSprites(TicksLastFrame);

	// Update all rays.
	CastAllRays();
    
    Render();
}

// ______________________________________________________________________________________________________________
// END GAMEPLAY STATE FUNCTIONS

// MAP LOAD STATE FUNCTIONS
// ______________________________________________________________________________________________________________
void MapLoadExecute(float DeltaTime, float CurrentTime) {
	LoadMap(MapLoadState.CustomData);
	StateMachine_ChangeState(&GameStateMachine, &GameplayState);
}

// ______________________________________________________________________________________________________________


void MAIN_Setup(void) {
	// Allocate color buffer here
	CreateColorBuffer(WINDOW_W, WINDOW_H);

	// Create color buffer texture for SDL.
	ColorBufferTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);

	InitializeMapsList();
	LoadTextures();
	InitializeSprites();
	CreateAllEntities();

	// Player health after death.
	if (Player.Health <= 0) {
		Player.Health = Player.MaxHealth;
	}

	// Create game states here.
	GameplayState.OnEnter = &GameplayEnter;
	GameplayState.OnExit = &GameplayExit;
	GameplayState.OnExecute = &GameplayExecute;

	// Create another game states.
	MapLoadState.OnEnter = 0;
	MapLoadState.OnExecute = &MapLoadExecute;
	MapLoadState.OnExit = 0;

	MapLoadState.CustomData = 0;
	StateMachine_ChangeState(&GameStateMachine, &MapLoadState);
}


void HandleInputExit(void) {
	int32_t NumKeys;
	const uint8_t* KeyStates = SDL_GetKeyboardState(&NumKeys);
	if (KeyStates[SDL_SCANCODE_ESCAPE]) {
		IsGameRunning = false;
	}
}

void UpdateStateMachine(void) {
	int TimeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - TicksLastFrame);

	if (TimeToWait > 0 && TimeToWait <= FRAME_TIME_LENGTH) {
		SDL_Delay(TimeToWait);
	}

	float DeltaTime = (SDL_GetTicks() - TicksLastFrame) / 1000.0f;

	TicksLastFrame = SDL_GetTicks();

	StateMachine_Update(&GameStateMachine, DeltaTime, TicksLastFrame);
}

void RenderColorBuffer(void) {
	SDL_UpdateTexture(ColorBufferTexture, NULL, GetColorBuffer(), (int)((uint32_t)WINDOW_W * sizeof(uint32_t)));
	SDL_RenderCopy(Renderer, ColorBufferTexture, NULL, NULL);
	SDL_RenderPresent(Renderer);
}

void RenderMapGrid(void) {
	for (int i = 0; i < MAP_NUM_ROWS; i++) {
		for (int j = 0; j < MAP_NUM_COLS; j++) {
			int TileX = j * TILE_SIZE;
			int TileY = i * TILE_SIZE;
			uint32_t TileColor = GetMapAt(i, j).Type != 0 ? 0xffffffff : 0xff000000;

			DrawRectangle(
					TileX * MINIMAP_SCALE_FACTOR,
					TileY * MINIMAP_SCALE_FACTOR,
					TILE_SIZE * MINIMAP_SCALE_FACTOR,
					TILE_SIZE * MINIMAP_SCALE_FACTOR,
					TileColor
					);
		}
	}
}

void Render2DLayer(void) {
	for (int32_t i=0; i < NUM_SPRITES; ++i) {
		sprite_t* CurrentSprite = &Sprites[i];

		if (CurrentSprite->Use2D) {
			Render2DSprite(CurrentSprite);
		}
	}
}

void Render(void) {
	ClearColorBuffer(0xff000000, WINDOW_W, WINDOW_H);

	// Render projection of sprites and walls.
	RenderWallProjection();
	RenderSpriteProjection();

	// Render minimap objects.
	if (ENABLE_DEBUG_MINIMAP) {
		RenderMapGrid();
		RenderMapRays();
		RenderMapSprites();
		RenderMapPlayer();
	}

	// Render 2D layer.
	Render2DLayer();

	RenderColorBuffer();


	SDL_ShowCursor(SHOW_CURSOR);
	SDL_CaptureMouse(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void ReleaseResources(void) {
	FreeTextures();
}

void InitMusic(void) {
	SDL_Init(SDL_INIT_AUDIO);
	initAudio();
	playMusic("./data/music/music.wav", SDL_MIX_MAXVOLUME / 2);
}

int main(int argc, char *argv[]) {
	// HACK: hacky way to silent warnings about unused parameters
	if (argc) {
		if (argv) {
		}
	}

	IsGameRunning = InitializeWindow();

	InitInput();
	MAIN_Setup();

	InitMusic();

	while (IsGameRunning) {
		SDL_PumpEvents();

		HandleInputExit();
		UpdateInput();
        UpdateStateMachine();
	}

	ReleaseResources();
	DestroyWindow();

	return 0;
}
