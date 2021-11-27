#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define NUMBER_OF_KEYS 4
extern bool Keys[NUMBER_OF_KEYS];

enum keys_e {
    EKEY_UP = 0,
    EKEY_DOWN = 1,
    EKEY_LEFT = 2,
    EKEY_RIGHT = 3
};

void InitKeyboard(void);
void UpdateKeyboard(void);

#endif
