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
    uint8_t pollActive;
    char userCommand;
} RiceCookerStatus;

/* declare the state machine object*/
extern SM_StateMachine SM_riceCooker;
extern RiceCookerStatus riceCookerStatus;

/* state machine event functions */
void RiceCooker_Start(SM_StateMachine *self);
void RiceCooker_Cancel(SM_StateMachine *self);
void RiceCooker_Poll(SM_StateMachine *self);

void StartPoll(void);
void StopPoll(void);
uint8_t RiceCooker_IsPollActive(void);

#endif /* STATE_MACHINE_CFG_H */