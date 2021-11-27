#include "input.h"

bool Keys[NUMBER_OF_KEYS];
mouse_state_t MouseState;

void InitInput(void) {
    // Initialize keyboard input by default values.
    for(int32_t i = 0; i < NUMBER_OF_KEYS; i++) {
        Keys[i] = false;
    }
    
    // Initialize mouse input by current mouse values.
    int32_t CurrentMouseX;
    int32_t CurrentMouseY;
    SDL_GetMouseState(&CurrentMouseX, &CurrentMouseY);
    MouseState.X = CurrentMouseX;
    MouseState.Y = CurrentMouseY;
    MouseState.LastX = CurrentMouseX;
    MouseState.LastY = CurrentMouseY;
    MouseState.DeltaX = 0;
    MouseState.DeltaY = 0;
}

void UpdateInput(void) {
    // Update keyboard keys.
    int32_t NumKeys = 0;
    const uint8_t* KeyStates = SDL_GetKeyboardState(&NumKeys);
    Keys[EKEY_UP] = KeyStates[SDL_SCANCODE_UP];
    Keys[EKEY_DOWN] = KeyStates[SDL_SCANCODE_DOWN];
    Keys[EKEY_LEFT] = KeyStates[SDL_SCANCODE_LEFT];
    Keys[EKEY_RIGHT] = KeyStates[SDL_SCANCODE_RIGHT];
    
    // Update mouse state.
    int32_t LastMouseX = MouseState.X;
    int32_t LastMouseY = MouseState.Y;
    int32_t CurrentMouseX;
    int32_t CurrentMouseY;
    
    // Set current state.
    SDL_GetMouseState(&CurrentMouseX, &CurrentMouseY);
    MouseState.X = CurrentMouseX;
    MouseState.Y = CurrentMouseY;
    
    // Set last state (from previous frame).
    MouseState.LastX = LastMouseX;
    MouseState.LastY = LastMouseY;
    
    // Calculate delta for X and Y.
    MouseState.DeltaX = MouseState.X - MouseState.LastX;
    MouseState.DeltaY = MouseState.Y - MouseState.LastY;
    
    // debug print
    printf("Mouse Delta X: %d and Y: %d \n", MouseState.DeltaX, MouseState.DeltaY);
}
