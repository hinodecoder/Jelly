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


SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
bool IsGameRunning = false;
int TicksLastFrame = 0;
SDL_Texture* ColorBufferTexture = NULL;
int32_t SHOW_CURSOR = 0;

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
    int32_t DisplayHeight = 800;
    
    Window = SDL_CreateWindow( NULL,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               DisplayWidth,
                               DisplayHeight,
                               SDL_WINDOW_BORDERLESS);
                               //SDL_WINDOW_FULLSCREEN );
                               //0);

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

// TODO: Move somewhere
void CreateHUD(void) {
    sprite_t* WeaponSprite = &Sprites[NextFreeSpriteIndex++];
    WeaponSprite->Use2D = true;
    WeaponSprite->Empty = false;
    WeaponSprite->TextureId = 4;
    
    // Weapon positioning.
    texture_t* WeaponTexture = GetTexture(WeaponSprite->TextureId);
    WeaponSprite->Y = WINDOW_H - WeaponTexture->Height;
    WeaponSprite->X = (WINDOW_W / 2) - (WeaponTexture->Width / 2);
}

void Setup(void) {
    // Allocate color buffer here
    CreateColorBuffer(WINDOW_W, WINDOW_H);
    
    // Create color buffer texture for SDL.
    ColorBufferTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);
    
    LoadTextures();
    InitializeSprites();
    CreateAllEntities();
    
    LoadMap("./data/maps/Test.png");
    
    // Important: Create HUD AFTER loading a map due to sprite indexing.
    CreateHUD();
}

void ProcessAppInput(void) {
    SDL_Event Event;
    SDL_PollEvent(&Event);
    
    switch (Event.type) {
        case SDL_KEYDOWN:
            if (Event.key.keysym.sym == SDLK_ESCAPE) {
                IsGameRunning = false;
            }
            break;
    }
    
    SDL_ShowCursor(SHOW_CURSOR);
    SDL_CaptureMouse(SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void UpdateAllEntities(float DeltaTime, float CurrentTime) {
    for (int32_t i=0; i < NUM_ENTITIES; ++i) {
        entity_t* CurrentEntity = &Entities[i];
        UpdateEntity(CurrentEntity, DeltaTime, CurrentTime);
    }
}

void Update(void) {
    int TimeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - TicksLastFrame);

    if (TimeToWait > 0 && TimeToWait <= FRAME_TIME_LENGTH) {
        SDL_Delay(TimeToWait);
    }

    float DeltaTime = (SDL_GetTicks() - TicksLastFrame) / 1000.0f;

    TicksLastFrame = SDL_GetTicks();

    // TODO: Move gameplay code to some kind of state machine.
    PlayerMove(DeltaTime);
    PlayerShoot();
    UpdateAllEntities(DeltaTime, TicksLastFrame);

    // Update all animated sprites.
    UpdateAnimatedSprites(TicksLastFrame);

    // Update all rays.
    CastAllRays();
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
            uint32_t TileColor = GetMapAt(i, j) != 0 ? 0xffffffff : 0xff000000;
            
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
    RenderMapGrid();
    RenderMapRays();
    RenderMapSprites();
    RenderMapPlayer();
    
    // Render 2D layer.
    Render2DLayer();
    
    RenderColorBuffer();
}

void ReleaseResources(void) {
    FreeTextures();
}

int main(void) {
    IsGameRunning = InitializeWindow();

    InitInput();
    Setup();

    while (IsGameRunning) {
        ProcessAppInput();
        UpdateInput();
        Update();
        Render();
    }

    ReleaseResources();
    DestroyWindow();

    return EXIT_SUCCESS;
}
