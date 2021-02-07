#ifndef STATE_MACHINE_ENUMS_H
#define STATE_MACHINE_ENUMS_H

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
    EVENT_START,
    EVENT_CANCEL,
    EVENT_POLL_COOK_STATUS,
    /* DO NOT modify following enums */
    EVENT_MAX_EVENTS
} SM_Events;

#endif /* STATE_MACHINE_ENUMS_H */