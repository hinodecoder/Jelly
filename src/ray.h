#ifndef RAY_H
#define RAY_H

#include <stdio.h>
#include <float.h>
#include <stdbool.h>
#include "consts.h"

typedef struct {
    float RayAngle;
    float WallHitX;
    float WallHitY;
    float Distance;
    bool WasHitVertical;
    int32_t WallHitContent;
} ray_t;

extern ray_t Rays[NUM_RAYS];


void NormalizeAngle(float* Angle);
float DistanceBetweenPoints(float X1, float Y1, float X2, float Y2);
void CastRay(float RayAngle, int StripId);
void CastAllRays(void);
void RenderRays(void);

#endif
