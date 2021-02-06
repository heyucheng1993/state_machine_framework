#ifndef STATE_MACHINE_CFG_H
#define STATE_MACHINE_CFG_H

#include "StateMachine.h"

/* rice cooker status */
typedef struct
{
    int16_t temprature;
    uint16_t timer;
    uint8_t pollActive;
} SM_RiceCookerStatus;

/* state enumeration order must match the order of state */
typedef enum 
{
    /* user defined states */
    STATE_IDLE,
    STATE_COMPLETED,
    STATE_CANCELED,
    STATE_STARTCOOK,
    STATE_HEATING,
    STATE_BOILED,
    STATE_STEAMING,
    STATE_KEEPWARM,
    /* DO NOT modify following enums */
    STATE_MAX_STATES,
    STATE_EVENT_IGNORED = 0xFE,
    STATE_EVENT_INVALID = 0xFF
} SM_States;

/* event enumeration */
typedef enum
{
    /* user defined events */
    Start,
    Cancel,
    PollCookStatus,
    /* DO NOT modify following enums */
    EVENT_MAX_EVENTS
} SM_Events;

/* declare the state machine object*/
extern SM_StateMachine SM_riceCooker;

/* state machine event functions */
void RiceCooker_Start(SM_StateMachine *self);
void RiceCooker_Cancel(SM_StateMachine *self);
void RiceCooker_Poll(SM_StateMachine *self);

uint8_t RiceCooker_IsPollActive(void);

#endif /* STATE_MACHINE_CFG_H */