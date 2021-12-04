#include <stdlib.h>
#include <math.h>
#include "graphics.h"

// Color buffer in memory where we override pixels to render.
static uint32_t* ColorBuffer = NULL;

// Store buffer width and do not pass as parameter in every function.
static uint32_t BufferWidth = 0;
static uint32_t BufferHeight = 0;


void CreateColorBuffer(uint32_t Width, uint32_t Height) {
    ColorBuffer = (uint32_t*) malloc(sizeof(uint32_t) * (uint32_t)Width * (uint32_t)Height);
    BufferWidth = Width;
    BufferHeight = Height;
}

uint32_t* GetColorBuffer(void) {
    return ColorBuffer;
}

void DrawPixel(int32_t X, int32_t Y, uint32_t Color) {
    if (X > 0 && X < BufferWidth && Y > 0 && Y < BufferHeight) {
        // Skip "magenta" color
        if (Color != 0xffff00ff) {
            ColorBuffer[(BufferWidth * Y) + X] = Color;
        }
    }
}

void DrawRectangle(int32_t X, int32_t Y, int32_t Width, int32_t Height, uint32_t Color) {
    for (int32_t i=X; i <= (X + Width); ++i) {
        for (int32_t j=Y; j <= (Y + Height); j++) {
            DrawPixel(i, j, Color);
        }
    }
}

void DrawLine(int32_t X0, int32_t Y0, int32_t X1, int32_t Y1, uint32_t Color) {
    // TODO: DDA, replace with Bresenham later.
    const int32_t DeltaX = (X1 - X0);
    const int32_t DeltaY = (Y1 - Y0);
    
    const int32_t AbsDeltaX = abs(DeltaX);
    const int32_t AbsDeltaY = abs(DeltaY);
    const int32_t LongestLenght = (AbsDeltaX >= AbsDeltaY) ? AbsDeltaX : AbsDeltaY;
    
    const float XIncrement = DeltaX / (float) LongestLenght;
    const float YIncrement = DeltaY / (float) LongestLenght;
    
    float CurrentX = X0;
    float CurrentY = Y0;
    
    for (int32_t i=0; i < LongestLenght; i++) {
        const float RoundedCurrentX = round(CurrentX);
        const float RoundedCurrentY = round(CurrentY);
        
        if (RoundedCurrentX >= 0 && RoundedCurrentY >= 0) {
            DrawPixel(RoundedCurrentX, RoundedCurrentY, Color); // TODO: slow?
        }
        
        CurrentX += XIncrement;
        CurrentY += YIncrement;
    }
}

void ClearColorBuffer(uint32_t Color, uint32_t Width, uint32_t Height) {
    for (int i = 0; i < Width * Height; ++i) {
        ColorBuffer[i] = Color;
    }
}

void DestroyColorBuffer(void) {
    free(ColorBuffer);
}

void ChangeColorIntensity(uint32_t* Color, float Factor) {
    uint32_t A = (*Color & 0xFF000000);
    uint32_t R = (*Color & 0x00FF0000) * Factor;
    uint32_t G = (*Color & 0x0000FF00) * Factor;
    uint32_t B = (*Color & 0x000000FF) * Factor;
    
    *Color = A | (R & 0x00FF0000) | (G & 0x0000FF00) | (B & 0x000000FF);
}
