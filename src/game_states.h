#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "state_machine.h"

extern state_machine_t GameStateMachine;
extern state_t GameplayState;
extern state_t MapLoadState;

// Extern main setup function.
extern void MAIN_Setup(void);

#endif
