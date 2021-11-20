#include <stdlib.h>
#include "graphics.h"

// Color buffer in memory where we override pixels to render.
static uint32_t* ColorBuffer = NULL;


void CreateColorBuffer(uint32_t Width, uint32_t Height) {
    ColorBuffer = (uint32_t*) malloc(sizeof(uint32_t) * (uint32_t)Width * (uint32_t)Height);
}

uint32_t* GetColorBuffer(void) {
    return ColorBuffer;
}

void WriteColorBuffer(int32_t X, int32_t Y, uint32_t Color, uint32_t BufferWidth) {
    // Just override data in memory. I don't check if index is valid. This should be straight forward func.
    ColorBuffer[(BufferWidth * Y) + X] = Color;
}

void DrawRectangle(int32_t X, int32_t Y, int32_t Width, int32_t Height, uint32_t Color, uint32_t BufferWidth) {
    for (int32_t i=X; i <= (X + Width); ++i) {
        for (int32_t j=Y; j <= (Y + Height); j++) {
            WriteColorBuffer(i, j, Color, BufferWidth);
        }
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
