#include "StateMachine.h"
#include "StateMachine_cfg.h"

/* generates an external event. Called once per external event */
void SM_ExternalEvent(SM_StateMachine *self, uint8_t newState)
{
    assert(self);

    /* ignore event if new state required to do so */
    if (newState != STATE_EVENT_IGNORED) 
    {
        /* generate the event */ 
        SM_InternalEvent(self, newState);

        /* execute state transition */
        SM_ExecuteTransition(self);
    }
}

/* generates an internal event. Called from within a state */
void SM_InternalEvent(SM_StateMachine *self, uint8_t newState)
{
    assert(self);

    self->eventGenerated = true;
    self->newState = newState;
}

/* the state engine executes the state machine state transition */
void SM_ExecuteTransition(SM_StateMachine *self)
{
    uint8_t guardResult = true;

    assert(self);

    /* while events are being generated keep executing transition */
    while (self->eventGenerated)
    {
        /* error check that the new state is valid before proceeding */
        assert(self->newState < self->maxStates);

        /* get the pointers from the state map */
        SM_StateFunc state = self->stateMap[self->newState].pStateFunc;
        SM_GuardFunc guard = self->stateMap[self->newState].pGuardFunc;
        SM_EntryFunc entry = self->stateMap[self->newState].pEntryFunc;
        SM_ExitFunc exit = self->stateMap[self->currentState].pExitFunc;

        /* event used up, reset the flag */
        self->eventGenerated = false;

        /* execute the guard condition */
        if (guard != NULL)
        {
            guardResult = guard(self);
        }

        /* do transition according to guard result */
        if (guardResult == true)
        {
            /* wether it is necessary to transition to new state */
            if (self->newState != self->currentState)
            {
                /* execute the state exit action on current state before switching to new state */
                if (exit != NULL)
                {
                    exit(self);
                }

                /* execute the state entry action on the new state */
                if (entry != NULL)
                {
                    entry(self);
                }

                /* ensure exit/entry actions didn't call SM_InternalEvent by accident */ 
                assert(self->eventGenerated == false);
            }

            /* switch to the new current state */
            self->currentState = self->newState;

            /* execute the state action */
            assert(state != NULL);
            state(self);
        }
    }
}

void SM_EventEmit(SM_StateMachine *self, SM_Events event)
{
    assert(self);

    /* emit event */
    SM_EventEmitFunc eventEmit = self->eventMap[event].pEventEmitFunc;
    eventEmit(self);
}