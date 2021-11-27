#include "input.h"

bool Keys[NUMBER_OF_KEYS];

void InitKeyboard(void) {
    for(int32_t i = 0; i < NUMBER_OF_KEYS; i++) {
        Keys[i] = false;
    }
}

void UpdateKeyboard(void) {
    int32_t NumKeys = 0;
    const uint8_t* KeyStates = SDL_GetKeyboardState(&NumKeys);
    
    Keys[EKEY_UP] = KeyStates[SDL_SCANCODE_UP];
    Keys[EKEY_DOWN] = KeyStates[SDL_SCANCODE_DOWN];
    Keys[EKEY_LEFT] = KeyStates[SDL_SCANCODE_LEFT];
    Keys[EKEY_RIGHT] = KeyStates[SDL_SCANCODE_RIGHT];
}
