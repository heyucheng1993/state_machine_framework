#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "StateMachine_enums.h"
#include "util.h"

typedef struct SM_StateMachine SM_StateMachine;

/* generic state function signatures */
typedef void    (*SM_StateFunc)(SM_StateMachine *self);
typedef uint8_t (*SM_GuardFunc)(SM_StateMachine *self);
typedef void    (*SM_EntryFunc)(SM_StateMachine *self);
typedef void    (*SM_ExitFunc) (SM_StateMachine *self);
typedef void    (*SM_EventEmitFunc) (SM_StateMachine *self);

/* state map struct */
typedef struct
{
    SM_StateFunc pStateFunc;
    SM_GuardFunc pGuardFunc;
    SM_EntryFunc pEntryFunc;
    SM_ExitFunc pExitFunc;
} SM_StateMap;

/* event map struct */
typedef struct
{
    SM_Events eventId;
    SM_EventEmitFunc pEventEmitFunc;
} SM_EventMap;

/* state machine instance data */
struct SM_StateMachine
{
    const char *name;
    const uint8_t maxStates;
    uint8_t newState;
    uint8_t currentState;
    uint8_t eventGenerated;
    const SM_StateMap *stateMap;
    const SM_EventMap *eventMap;
};

/* function declarations */
void SM_ExternalEvent(SM_StateMachine *self, uint8_t newState);
void SM_InternalEvent(SM_StateMachine *self, uint8_t newState);
void SM_ExecuteTransition(SM_StateMachine *self);
void SM_EventEmit(SM_StateMachine *self, SM_Events event);
 
#endif /* STATE_MACHINE_H */