#include "math.h"
#include "consts.h"
// ________________________________________________________________________
void NormalizeAngle(float* Angle) {
    *Angle = remainder(*Angle, TWO_PI);
    
    if (*Angle < 0) {
        *Angle = TWO_PI + *Angle;
    }
}
// ________________________________________________________________________
float DistanceBetweenPoints(float X1, float Y1, float X2, float Y2) {
    return sqrtf((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
}
// ________________________________________________________________________