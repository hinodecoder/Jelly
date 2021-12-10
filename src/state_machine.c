#include "state_machine.h"

void StateMachine_Update(state_machine_t* StateMachine, float DeltaTime, float CurrentTime) {
	if (StateMachine) {
		// Check if we neet to change states.
		if (StateMachine->NextState) {
			if (StateMachine->CurrentState) {
				if (StateMachine->CurrentState->OnExit) {
					StateMachine->CurrentState->OnExit();
				}
			}
			if (StateMachine->NextState->OnEnter) {
				StateMachine->NextState->OnEnter();
			}
			StateMachine->CurrentState = StateMachine->NextState;
			StateMachine->NextState = 0;
		}

		// Execute current state.
		if (StateMachine->CurrentState) {
			StateMachine->CurrentState->OnExecute(DeltaTime, CurrentTime);
		}
	}
}

void StateMachine_ChangeState(state_machine_t* StateMachine, state_t* NewState) {
	if (StateMachine) {
		StateMachine->NextState = NewState; 
	}
}
