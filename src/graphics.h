#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

void CreateColorBuffer(uint32_t Width, uint32_t Height);
uint32_t* GetColorBuffer(void);
void WriteColorBuffer(int32_t X, int32_t Y, uint32_t Color, uint32_t BufferWidth);
void DrawRectangle(int32_t X, int32_t Y, int32_t Width, int32_t Height, uint32_t Color, uint32_t BufferWidth);
void ClearColorBuffer(uint32_t Color, uint32_t Width, uint32_t Height);
void DestroyColorBuffer(void);

#endif /* GRAPHICS_H */
