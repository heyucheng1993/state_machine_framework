#ifndef STATE_MACHINE_CFG_H
#define STATE_MACHINE_CFG_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "StateMachine_enums.h"
#include "StateMachine.h"

/* rice cooker status */
typedef struct
{
    int16_t temprature;
    uint16_t timer;
    char userCommand;
} RiceCookerStatus;

/* declare the state machine object*/
extern SM_StateMachine SM_riceCooker;
extern RiceCookerStatus riceCookerStatus;

/* state machine event functions */
void RiceCooker_Start(SM_StateMachine *self);
void RiceCooker_Cancel(SM_StateMachine *self);

#endif /* STATE_MACHINE_CFG_H */