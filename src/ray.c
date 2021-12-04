#include "ray.h"
#include "player.h"
#include "map.h"
#include "graphics.h"
#include "math.h"

#define RAYS_RENDERING_DENSITY 30

ray_t Rays[NUM_RAYS];

void CastAllRays(void) {
    for (int Column = 0; Column < NUM_RAYS; ++Column) {
        float RayAngle = Player.RotationAngle + atan((Column - (NUM_RAYS / 2)) / DISTANCE_TO_PROJECTION_PLANE);
        CastRay(RayAngle, Column);
    }
}

void CastRay(float RayAngle, int StripId) {
    NormalizeAngle(&RayAngle);
    
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
    //while (nextHX >= 0 && nextHX <= MAP_NUM_COLS * TILE_SIZE && nextHY >= 0 && nextHY <= MAP_NUM_ROWS * TILE_SIZE) {
    while (IsInsideMap(nextHX, nextHY)) {
        float XCheck = nextHX;
        float YCheck = nextHY + (IsRayFacingUp ? -1 : 0);
        
        if (MapHasWallAt(XCheck, YCheck)) {
            // There is a wall hit.
            wallHitX_horizontal = nextHX;
            wallHitY_horizontal = nextHY;
            HorizontalWallContent = GetMapAt((int)floor(YCheck / TILE_SIZE), (int)floor(XCheck / TILE_SIZE));
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
    //while (nextVX >= 0 && nextVX <= MAP_NUM_COLS * TILE_SIZE && nextVY >= 0 && nextVY <= MAP_NUM_ROWS * TILE_SIZE) {
    while (IsInsideMap(nextVX, nextVY)) {
        float XCheck = nextVX + (IsRayFacingLeft ? -1 : 0);
        float YCheck = nextVY;
        
        if (MapHasWallAt(XCheck, YCheck)) {
            // There is a wall hit.
            wallHitX_vertical = nextVX;
            wallHitY_vertical = nextVY;
            VerticalWallContent = GetMapAt((int)floor(YCheck / TILE_SIZE), (int)floor(XCheck / TILE_SIZE));
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
    
    // Store some content.
    if (VerticalHitDistance < HorizontalHitDistance) {
        Rays[StripId].Distance = VerticalHitDistance;
        Rays[StripId].WallHitX = wallHitX_vertical;
        Rays[StripId].WallHitY = wallHitY_vertical;
        Rays[StripId].TextureId = VerticalWallContent;
        Rays[StripId].WasHitVertical = true;
        Rays[StripId].RayAngle = RayAngle;
    } else {
        Rays[StripId].Distance = HorizontalHitDistance;
        Rays[StripId].WallHitX = wallHitX_horizontal;
        Rays[StripId].WallHitY = wallHitY_horizontal;
        Rays[StripId].TextureId = HorizontalWallContent;
        Rays[StripId].WasHitVertical = false;
        Rays[StripId].RayAngle = RayAngle;
    }
    
    // (NOTE): Important. Reset information that the ray is blocked by something other than wall.
    Rays[StripId].BlockedBy = -1;
}

void RenderMapRays(void) {
    for (int i=0; i < NUM_RAYS; i += RAYS_RENDERING_DENSITY) {
        DrawLine(
                 MINIMAP_SCALE_FACTOR * Player.X,
                 MINIMAP_SCALE_FACTOR * Player.Y,
                 MINIMAP_SCALE_FACTOR * Rays[i].WallHitX,
                 MINIMAP_SCALE_FACTOR * Rays[i].WallHitY,
                 0xff0000ff);
    }
    
    // Draw center ray.
    DrawLine(
             MINIMAP_SCALE_FACTOR*Player.X,
             MINIMAP_SCALE_FACTOR*Player.Y,
             MINIMAP_SCALE_FACTOR* Rays[NUM_RAYS/2].WallHitX,
             MINIMAP_SCALE_FACTOR* Rays[NUM_RAYS/2].WallHitY,
             0xffff0000);
}
