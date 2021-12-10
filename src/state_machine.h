#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <stdint.h>

typedef struct {
	void (*OnEnter)(void);
	void (*OnExecute)(float, float);
	void (*OnExit)(void);
	int32_t CustomData;
} state_t;

typedef struct {
	state_t* CurrentState;
	state_t* NextState; // queue it
} state_machine_t;


void StateMachine_Update(state_machine_t* StateMachine, float DeltaTime, float CurrentTime);
void StateMachine_ChangeState(state_machine_t* StateMachine, state_t* NewState);

#endif
