#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    int32_t X;
    int32_t Y;
    int32_t LastX;
    int32_t LastY;
    int32_t DeltaX;
    int32_t DeltaY;
} mouse_state_t;

#define NUMBER_OF_KEYS 4
extern bool Keys[NUMBER_OF_KEYS];
extern mouse_state_t MouseState;

enum keys_e {
    EKEY_UP = 0,
    EKEY_DOWN = 1,
    EKEY_LEFT = 2,
    EKEY_RIGHT = 3
};

void InitInput(void);
void UpdateInput(void);

#endif
