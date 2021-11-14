#include <stdio.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "consts.h"
#include "textures.h"


//
// TODO:
// - use new standard for types and change everything to new ones. (int->int32_t etc.)
//


const int Map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

typedef struct {
    float X;
    float Y;
    float Width;
    float Height;
    int TurnDirection; // -1 left, +1 right
    int WalkDirection; // -1 back
    float RotationAngle;
    float WalkSpeed;
    float TurnSpeed;
} player_t;

// Create a global player
player_t Player;



typedef struct {
    float RayAngle;
    float WallHitX;
    float WallHitY;
    float Distance;
    bool WasHitVertical;
    int32_t WallHitContent;
} ray_t;

ray_t Rays[NUM_RAYS];



SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
bool IsGameRunning = false;
int TicksLastFrame = 0;

uint32_t* ColorBuffer = NULL;
SDL_Texture* ColorBufferTexture = NULL;

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

    int32_t DisplayWidth = WINDOW_W;
    int32_t DisplayHeight = WINDOW_H;
    
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
    FreeTextures();
    
    free(ColorBuffer);
    SDL_DestroyTexture(ColorBufferTexture);
    
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}


void Setup(void) {
    Player.X = 4 * TILE_SIZE;
    Player.Y = 5 * TILE_SIZE;
    Player.Width = 1;
    Player.Height = 1;
    Player.TurnDirection = 0;
    Player.WalkDirection = 0;
    Player.RotationAngle = PI / 2;
    Player.WalkSpeed = 200;
    Player.TurnSpeed = 85 * (PI / 180);
    
    // Allocate color buffer here
    ColorBuffer = (uint32_t*) malloc(sizeof(uint32_t) * (uint32_t)WINDOW_W * (uint32_t)WINDOW_H);
    
    // Create color buffer texture for SDL.
    ColorBufferTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);
    
    LoadTextures();
}


bool MapHasWallAt(float X, float Y) {
    if (X < 0 || X >= MAP_NUM_COLS * TILE_SIZE || Y < 0 || Y >= MAP_NUM_ROWS * TILE_SIZE) {
        return true;
    }
    
    int MapGridIndexX = floor(X / TILE_SIZE);
    int MapGridIndexY = floor(Y / TILE_SIZE);
    
    return Map[MapGridIndexY][MapGridIndexX] != 0;
}


void MovePlayer(float DeltaTime) {
    Player.RotationAngle += Player.TurnDirection * Player.TurnSpeed * DeltaTime;
    float MoveStep = Player.WalkDirection * Player.WalkSpeed * DeltaTime;

    float NewPlayerX = Player.X + cos(Player.RotationAngle) * MoveStep;
    float NewPlayerY = Player.Y + sin(Player.RotationAngle) * MoveStep;

    if (!MapHasWallAt(NewPlayerX, NewPlayerY)) {
        Player.X = NewPlayerX;
        Player.Y = NewPlayerY;
    }
}


void RenderPlayer(void) {
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_Rect PlayerRect = {
        Player.X * MINIMAP_SCALE_FACTOR,
        Player.Y * MINIMAP_SCALE_FACTOR,
        Player.Width * MINIMAP_SCALE_FACTOR,
        Player.Height * MINIMAP_SCALE_FACTOR
    };

    SDL_RenderFillRect(Renderer, &PlayerRect);

    SDL_RenderDrawLine( Renderer,
                        MINIMAP_SCALE_FACTOR * Player.X,
                        MINIMAP_SCALE_FACTOR * Player.Y,
                        MINIMAP_SCALE_FACTOR * Player.X + cos(Player.RotationAngle) * 40,
                        MINIMAP_SCALE_FACTOR * Player.Y + sin(Player.RotationAngle) * 40
                        );
}

float NormalizeAngle(float Angle) {
    Angle = remainder(Angle, TWO_PI);
    
    if (Angle < 0) {
        Angle = TWO_PI + Angle;
    }
    
    return Angle;
}

float DistanceBetweenPoints(float X1, float Y1, float X2, float Y2) {
    return sqrtf((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
}

void CastRay(float RayAngle, int StripId) {
    // TODO: implement crazy stuff
    
    RayAngle = NormalizeAngle(RayAngle);
    
    // Angle direction
    int IsRayFacingDown = RayAngle > 0 && RayAngle < PI;
    int IsRayFacingUp = !IsRayFacingDown;
    int IsRayFacingRight = RayAngle < 0.5f * PI || RayAngle > 1.5f * PI;
    int IsRayFacingLeft = !IsRayFacingRight;
    
    float xintercept;
    float yintercept;
    float xstep;
    float ystep;

    // -----------------------------------
    // HORIZONTAL RAY-GRID INTERSECTION
    // -----------------------------------
    bool foundHWallHit = false;
    float wallHitX_horizontal = 0;
    float wallHitY_horizontal = 0;
    int HorizontalWallContent = 0;

    // Find the y coordinate of the closest horizontal grid intersection
    yintercept = floor(Player.Y / TILE_SIZE) * TILE_SIZE;
    yintercept += IsRayFacingDown ? TILE_SIZE : 0; // instead of using conditional just * 1 or 0.
    xintercept = Player.X + (yintercept - Player.Y) / tan(RayAngle);

    // Calculate xstep and ystep (deltas)
    ystep = TILE_SIZE;
    ystep *= IsRayFacingUp ? -1 : 1;

    xstep = TILE_SIZE / tan(RayAngle);
    xstep *= (IsRayFacingLeft && xstep > 0) ? -1 : 1;
    xstep *= (IsRayFacingRight && xstep < 0) ? -1 : 1;

    float nextHX = xintercept;
    float nextHY = yintercept;

    // Inceremnt xstep and ystep until we find a wall
    while (nextHX >= 0 && nextHX <= MAP_NUM_COLS * TILE_SIZE && nextHY >= 0 && nextHY <= MAP_NUM_ROWS * TILE_SIZE) {
        float XCheck = nextHX;
        float YCheck = nextHY + (IsRayFacingUp ? -1 : 0);
        
        if (MapHasWallAt(XCheck, YCheck)) {
            // There is a wall hit.
            wallHitX_horizontal = nextHX;
            wallHitY_horizontal = nextHY;
            HorizontalWallContent = Map[(int)floor(YCheck / TILE_SIZE)][(int)floor(XCheck / TILE_SIZE)];
            foundHWallHit = true;
            break;
        } else {
            nextHX += xstep;
            nextHY += ystep;
        }
    }
    


    // -----------------------------------
    // VERTICAL RAY-GRID INTERSECTION
    // -----------------------------------
    bool foundVWallHit = false;
    float wallHitX_vertical = 0;
    float wallHitY_vertical = 0;
    int VerticalWallContent = 0;

    // Find the x coordinate of the closest vertical grid intersection
    xintercept = floor(Player.X / TILE_SIZE) * TILE_SIZE;
    xintercept += IsRayFacingRight ? TILE_SIZE : 0; // instead of using conditional just * 1 or 0.
    
    yintercept = Player.Y + (xintercept - Player.X) * tan(RayAngle);

    // Calculate xstep and ystep (deltas)
    xstep = TILE_SIZE;
    xstep *= IsRayFacingLeft ? -1 : 1;

    ystep = TILE_SIZE * tan(RayAngle);
    ystep *= (IsRayFacingUp && ystep > 0) ? -1 : 1;
    ystep *= (IsRayFacingDown && ystep < 0) ? -1 : 1;

    float nextVX = xintercept;
    float nextVY = yintercept;

    // Inceremnt xstep and ystep until we find a wall
    while (nextVX >= 0 && nextVX <= MAP_NUM_COLS * TILE_SIZE && nextVY >= 0 && nextVY <= MAP_NUM_ROWS * TILE_SIZE) {
        float XCheck = nextVX + (IsRayFacingLeft ? -1 : 0);
        float YCheck = nextVY;
        
        if (MapHasWallAt(XCheck, YCheck)) {
            // There is a wall hit.
            wallHitX_vertical = nextVX;
            wallHitY_vertical = nextVY;
            VerticalWallContent = Map[(int)floor(YCheck / TILE_SIZE)][(int)floor(XCheck / TILE_SIZE)];
            foundVWallHit = true;
            break;
        } else {
            nextVX += xstep;
            nextVY += ystep;
        }
    }
    
    // Calculate both horizontal and vertical distances and choose
    // the smallest one.
    float HorizontalHitDistance = foundHWallHit
    ? DistanceBetweenPoints(Player.X, Player.Y, wallHitX_horizontal, wallHitY_horizontal)
    : FLT_MAX;
    
    float VerticalHitDistance = foundVWallHit
    ? DistanceBetweenPoints(Player.X, Player.Y, wallHitX_vertical, wallHitY_vertical)
    : FLT_MAX;
    
    // TODO: Save content of Ray into array of rays.
    if (VerticalHitDistance < HorizontalHitDistance) {
        Rays[StripId].Distance = VerticalHitDistance;
        Rays[StripId].WallHitX = wallHitX_vertical;
        Rays[StripId].WallHitY = wallHitY_vertical;
        Rays[StripId].WallHitContent = VerticalWallContent;
        Rays[StripId].WasHitVertical = true;
        Rays[StripId].RayAngle = RayAngle;
    } else {
        Rays[StripId].Distance = HorizontalHitDistance;
        Rays[StripId].WallHitX = wallHitX_horizontal;
        Rays[StripId].WallHitY = wallHitY_horizontal;
        Rays[StripId].WallHitContent = HorizontalWallContent;
        Rays[StripId].WasHitVertical = false;
        Rays[StripId].RayAngle = RayAngle;
    }
    
}


void CastAllRays(void) {
    for (int Column = 0; Column < NUM_RAYS; ++Column) {
        float RayAngle = Player.RotationAngle + atan((Column - (NUM_RAYS / 2)) / DISTANCE_TO_PROJECTION_PLANE);
        CastRay(RayAngle, Column);
    }
}


void RenderMap(void) {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int TileX = j * TILE_SIZE;
            int TileY = i * TILE_SIZE;
            int TileColor = Map[i][j] != 0 ? 255 : 0;

            SDL_SetRenderDrawColor(Renderer, TileColor, TileColor, TileColor, 255);

            SDL_Rect MapTileRect = {
                TileX * MINIMAP_SCALE_FACTOR,
                TileY * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR
            };

            SDL_RenderFillRect(Renderer, &MapTileRect);
        }
    }
}


void RenderRays(void) {
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    
    for (int i=0; i < NUM_RAYS; ++i) {
        SDL_RenderDrawLine(Renderer,
                           MINIMAP_SCALE_FACTOR * Player.X,
                           MINIMAP_SCALE_FACTOR * Player.Y,
                           MINIMAP_SCALE_FACTOR * Rays[i].WallHitX,
                           MINIMAP_SCALE_FACTOR * Rays[i].WallHitY);
    }
}


void ProcessInput(void) {
    SDL_Event Event;
    SDL_PollEvent(&Event);

    switch (Event.type) {
    case SDL_QUIT:
        IsGameRunning = false;
        break;

    case SDL_KEYDOWN:
        if (Event.key.keysym.sym == SDLK_ESCAPE) {
            IsGameRunning = false;
        }

        if (Event.key.keysym.sym == SDLK_UP) {
            Player.WalkDirection = +1;
        }

        if (Event.key.keysym.sym == SDLK_DOWN) {
            Player.WalkDirection = -1;
        }

        if (Event.key.keysym.sym == SDLK_LEFT) {
            Player.TurnDirection = -1;
        }

        if (Event.key.keysym.sym == SDLK_RIGHT) {
            Player.TurnDirection = +1;
        }

        break;

    case SDL_KEYUP:
        if (Event.key.keysym.sym == SDLK_UP) {
            Player.WalkDirection = 0;
        }

        if (Event.key.keysym.sym == SDLK_DOWN) {
            Player.WalkDirection = 0;
        }

        if (Event.key.keysym.sym == SDLK_LEFT) {
            Player.TurnDirection = 0;
        }

        if (Event.key.keysym.sym == SDLK_RIGHT) {
            Player.TurnDirection = 0;
        }

        break;

    }
}


void Update(void) {
    int TimeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - TicksLastFrame);

    if (TimeToWait > 0 && TimeToWait <= FRAME_TIME_LENGTH) {
        SDL_Delay(TimeToWait);
    }

    float DeltaTime = (SDL_GetTicks() - TicksLastFrame) / 1000.0f;

    TicksLastFrame = SDL_GetTicks();

    // move player
    MovePlayer(DeltaTime);
    CastAllRays();
}

void Generate3DProjection(void) {
    for (int x=0; x < NUM_RAYS; x++) {
        float PerpendicularDistance = Rays[x].Distance * cos(Rays[x].RayAngle - Player.RotationAngle);
        
        float ProjectedWallHeight = (TILE_SIZE / PerpendicularDistance) * DISTANCE_TO_PROJECTION_PLANE;
        
        // TODO: Replace, duplicate variable
        int WallStripHeight = ProjectedWallHeight;
        
        int WallTopPixel = (WINDOW_H / 2) - (WallStripHeight / 2);
        WallTopPixel = WallTopPixel < 0 ? 0 : WallTopPixel;
        
        int WallBottomPixel = (WINDOW_H / 2) + (WallStripHeight / 2);
        WallBottomPixel = WallBottomPixel > WINDOW_H ? WINDOW_H : WallBottomPixel;
        
        // Color for ceiling.
        for (int y = 0; y < WallTopPixel; ++y) {
            ColorBuffer[(WINDOW_W * y) + x] = 0xffcccccc;
        }
        
        int32_t TextureOffsetX = 0;
        
        // Base on vertical or horizontal hit we take remainder of proper axis.
        if (Rays[x].WasHitVertical) {
            TextureOffsetX = (int32_t) Rays[x].WallHitY % TILE_SIZE;
        } else {
            TextureOffsetX = (int32_t) Rays[x].WallHitX % TILE_SIZE;
        }
        
        
        // TODO: Add some null checks
        texture_t* CurrentTexture = GetTexture(Rays[x].WallHitContent - 1);
        uint32_t* TextureBuffer = CurrentTexture->Buffer;
        const int32_t TextureW = CurrentTexture->Width;
        const int32_t TextureH = CurrentTexture->Height;
        
        // Render the wall from top to bottom pixels.
        for (int y = WallTopPixel; y < WallBottomPixel; ++y) {
            // NOTE: Calculate distance from top because we do clamping on top pixel height and it
            // distorts our view while mapping a texture.
            int32_t DistanceFromTop = (y + WallStripHeight * 0.5f) - (WINDOW_H * 0.5f);
            
            // NOTE: Calculating texture offset in Y but we need to take "perspective" into consideration.
            // To calculate it we need overall wall height (wall strip height).
            int32_t TextureOffsetY = DistanceFromTop * ((float)TextureH / WallStripHeight);
            
            // Set color from sample texture.
            uint32_t TexelColor = TextureBuffer[(TextureW * TextureOffsetY) + TextureOffsetX];
            
            ColorBuffer[(WINDOW_W * y) + x] = TexelColor;
        }
        
        // Color for floor.
        for (int y = WallBottomPixel; y < WINDOW_H; ++y) {
            ColorBuffer[(WINDOW_W * y) + x] = 0xff1e3a29;
        }
    }
}

void ClearColorBuffer(uint32_t Color) {
    for (int i = 0; i < WINDOW_W * WINDOW_H; ++i) {
        ColorBuffer[i] = Color;
    }
}


void RenderColorBuffer(void) {
    SDL_UpdateTexture(ColorBufferTexture, NULL, ColorBuffer, (int)((uint32_t)WINDOW_W * sizeof(uint32_t)));
    SDL_RenderCopy(Renderer, ColorBufferTexture, NULL, NULL);
}


void Render(void) {
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Renderer);
    
    Generate3DProjection();
    
    // Render scene to color buffer.
    RenderColorBuffer();

    // Clear the Color Buffer.
    ClearColorBuffer(0xff000000);
    
    // Render minimap.
    RenderMap();
    RenderRays();
    RenderPlayer();

    SDL_RenderPresent(Renderer);
}


int main(void) {
    IsGameRunning = InitializeWindow();

    Setup();

    while (IsGameRunning) {
        ProcessInput();
        Update();
        Render();
    }

    DestroyWindow();

    return 0;
}
