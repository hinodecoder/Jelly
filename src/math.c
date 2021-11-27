#include "math.h"
#include "consts.h"

void NormalizeAngle(float* Angle) {
    *Angle = remainder(*Angle, TWO_PI);
    
    if (*Angle < 0) {
        *Angle = TWO_PI + *Angle;
    }
}

float DistanceBetweenPoints(float X1, float Y1, float X2, float Y2) {
    return sqrtf((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
}

void NormalizeVector2D(float *X, float *Y) {
    float Lenght = sqrt(((*X) * (*X)) + ((*Y) * (*Y)));
    if (Lenght > 0.0f) {
        *X /= Lenght;
        *Y /= Lenght;
    }
}

float Clamp(float Value, float Min, float Max) {
    if (Value > Max) {
        return Max;
    }
    if (Value < Min) {
        return Min;
    }
    return Value;
}
