#ifndef RAY_H
#define RAY_H

#include <stdio.h>
#include <float.h>
#include <stdint.h>
#include <stdbool.h>
#include "consts.h"

typedef struct {
    float RayAngle;
    float WallHitX;
    float WallHitY;
    float Distance;
    bool WasHitVertical;
    int32_t TextureId;
    
    // This can be used by something other than wall to block a ray.
    int32_t BlockedBy;
} ray_t;

extern ray_t Rays[NUM_RAYS];


void CastRay(float RayAngle, int StripId);
void CastAllRays(void);
void RenderMapRays(void);

#endif
