#include "state_machine.h"

void StateMachine_Update(state_machine_t* StateMachine, float DeltaTime, float CurrentTime) {
	if (StateMachine) {
		if (StateMachine->CurrentState) {
			StateMachine->CurrentState->OnExecute(DeltaTime, CurrentTime);
		}
	}
}

void StateMachine_ChangeState(state_machine_t* StateMachine, state_t* NewState) {
	if (StateMachine) {
		if (NewState) {
			if (StateMachine->CurrentState) {
				if (StateMachine->CurrentState->OnExit) {
					StateMachine->CurrentState->OnExit();
				}
			}
			if (NewState->OnEnter) {
				NewState->OnEnter();
			}
			StateMachine->CurrentState = NewState;
		}
	}
}
