#include "input.h"

bool Keys[NUMBER_OF_KEYS];
bool KeysPrevious[NUMBER_OF_KEYS];
mouse_state_t MouseState;

void InitInput(void) {
    // Initialize keyboard input by default values.
    for(int32_t i = 0; i < NUMBER_OF_KEYS; i++) {
        Keys[i] = 0;
        KeysPrevious[i] = 0;
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
    // _________________________________________________________
    int32_t NumKeys = 0;
    const uint8_t* KeyStates = SDL_GetKeyboardState(&NumKeys);
    Keys[EKEY_UP] = KeyStates[SDL_SCANCODE_UP] || KeyStates[SDL_SCANCODE_W];
    Keys[EKEY_DOWN] = KeyStates[SDL_SCANCODE_DOWN] || KeyStates[SDL_SCANCODE_S];
    Keys[EKEY_LEFT] = KeyStates[SDL_SCANCODE_LEFT] || KeyStates[SDL_SCANCODE_A];
    Keys[EKEY_RIGHT] = KeyStates[SDL_SCANCODE_RIGHT] || KeyStates[SDL_SCANCODE_D];
    
    // Update mouse state.
    // _________________________________________________________
    int32_t LastMouseX = MouseState.X;
    int32_t LastMouseY = MouseState.Y;
    int32_t CurrentMouseX;
    int32_t CurrentMouseY;
    
    // Set current state.
    int32_t MouseButtons = SDL_GetMouseState(&CurrentMouseX, &CurrentMouseY);
    MouseState.X = CurrentMouseX;
    MouseState.Y = CurrentMouseY;

    // Set last state (from previous frame).
    MouseState.LastX = LastMouseX;
    MouseState.LastY = LastMouseY;

    // Calculate delta for X and Y.
    MouseState.DeltaX = MouseState.X - MouseState.LastX;
    MouseState.DeltaY = MouseState.Y - MouseState.LastY;

    // Update mouse buttons.
    bool LeftMouseButtonPressed = MouseButtons & SDL_BUTTON_LMASK;
    Keys[EKEY_SHOOT] = LeftMouseButtonPressed && !KeysPrevious[EKEY_SHOOT];
    
    // Copy new to old.
    memcpy(KeysPrevious, Keys, sizeof(bool) * NUMBER_OF_KEYS);
    
    // Update previous keys states.
    KeysPrevious[EKEY_SHOOT] = LeftMouseButtonPressed;
}
