#include <util/delay.h>
#include "StateMachine_cfg.h"

/* state machine state functions */
static void Entry_Idle(SM_StateMachine *self);
static void StateAction_Idle(SM_StateMachine *self);
static void Exit_Idle(SM_StateMachine *self);
static void StateAction_Completed(SM_StateMachine *self);
static void StateAction_Canceled(SM_StateMachine *self);
static void StateAction_StartCook(SM_StateMachine *self);
static uint8_t Guard_StartCook(SM_StateMachine *self);
static void Entry_Heating(SM_StateMachine *self);
static void StateAction_Heating(SM_StateMachine *self);
static void Exit_Heating(SM_StateMachine *self);
static void StateAction_Boiled(SM_StateMachine *self);
static void Entry_Steaming(SM_StateMachine *self);
static void StateAction_Steaming(SM_StateMachine *self);
static void Exit_Steaming(SM_StateMachine *self);
static void Entry_KeepWarm(SM_StateMachine *self);
static void StateAction_KeepWarm(SM_StateMachine *self);
static void Exit_KeepWarm(SM_StateMachine *self);

/* rice cooker status */
RiceCookerStatus riceCookerStatus = 
{
    25,     /* temprature */
    0,      /* timer */
    false,  /* pollActive */
    '\0'    /* userCommand */
};

/* state map */
static const SM_StateMap SM_stateMap[] = 
{
    /* stateFunc */                      /* guardFunc */     /* entryFunc */     /* exitFunc */
    {StateAction_Idle                    , NULL              , Entry_Idle        , Exit_Idle         }, /* STATE_IDLE */
    {StateAction_Completed               , NULL              , NULL              , NULL              }, /* STATE_COMPLETED */
    {StateAction_Canceled                , NULL              , NULL              , NULL              }, /* STATE_CANCELED */
    {StateAction_StartCook               , Guard_StartCook   , NULL              , NULL              }, /* STATE_STARTCOOK */
    {StateAction_Heating                 , NULL              , Entry_Heating     , Exit_Heating      }, /* STATE_HEATING */
    {StateAction_Boiled                  , NULL              , NULL              , NULL              }, /* STATE_BOILED */
    {StateAction_Steaming                , NULL              , Entry_Steaming    , Exit_Steaming     }, /* STATE_STEAMING */
    {StateAction_KeepWarm                , NULL              , Entry_KeepWarm    , Exit_KeepWarm     }, /* STATE_KEEPWARM */
}; 

/* check state map validity */
STATIC_ASSERT((sizeof(SM_stateMap)/sizeof(SM_stateMap[0])) == STATE_MAX_STATES);

/* event map */
static const SM_EventMap SM_eventMap[] =
{
    /* eventId */             /* pEventEmitFunc */
    {EVENT_START,             RiceCooker_Start   },
    {EVENT_CANCEL,            RiceCooker_Cancel  },
    {EVENT_POLL_COOK_STATUS,  RiceCooker_Poll    }
};

/* check event map validity */
STATIC_ASSERT((sizeof(SM_eventMap)/sizeof(SM_eventMap[0])) == EVENT_MAX_EVENTS);

/* initialize state machine object with initial state */
SM_StateMachine SM_riceCooker = 
{ 
    "Rice Cooker State Machine",                    /* name */
    STATE_MAX_STATES,                               /* maxStates */
    STATE_IDLE,                                     /* newState */
    STATE_IDLE,                                     /* currentState */
    false,                                          /* eventGenerated */
    SM_stateMap,                                    /* stateMap */
    SM_eventMap                                     /* eventMap */
};

void RiceCooker_Start(SM_StateMachine *self)
{
    static const SM_States TRANSITIONS[] = 
    {                                    /* - Current State - */
        STATE_STARTCOOK,                 /* STATE_IDLE */
        STATE_EVENT_INVALID,             /* STATE_COMPLETED */
        STATE_EVENT_INVALID,             /* STATE_CANCELED */
        STATE_EVENT_INVALID,             /* STATE_STARTCOOK */
        STATE_EVENT_IGNORED,             /* STATE_HEATING */
        STATE_EVENT_INVALID,             /* STATE_BOILED */
        STATE_EVENT_IGNORED,             /* STATE_STEAMING */
        STATE_EVENT_IGNORED,             /* STATE_KEEPWARM */
    }; 
    SM_ExternalEvent(self, TRANSITIONS[self->currentState]);
    STATIC_ASSERT((sizeof(TRANSITIONS)/sizeof(SM_States)) == STATE_MAX_STATES);
}

void RiceCooker_Cancel(SM_StateMachine *self)
{
    static const SM_States TRANSITIONS[] = 
    {                                    /* - Current State - */
        STATE_EVENT_IGNORED,             /* STATE_IDLE */
        STATE_EVENT_INVALID,             /* STATE_COMPLETED */
        STATE_EVENT_INVALID,             /* STATE_CANCELED */
        STATE_CANCELED,                  /* STATE_STARTCOOK */
        STATE_CANCELED,                  /* STATE_HEATING */
        STATE_CANCELED,                  /* STATE_BOILED */
        STATE_CANCELED,                  /* STATE_STEAMING */
        STATE_COMPLETED,                 /* STATE_KEEPWARM */
    }; 
    SM_ExternalEvent(self, TRANSITIONS[self->currentState]);
    STATIC_ASSERT((sizeof(TRANSITIONS)/sizeof(SM_States)) == STATE_MAX_STATES);
}

void RiceCooker_Poll(SM_StateMachine *self)
{
    static const SM_States TRANSITIONS[] = 
    {                                    /* - Current State - */
        STATE_IDLE,                      /* STATE_IDLE */
        STATE_EVENT_IGNORED,             /* STATE_COMPLETED */
        STATE_EVENT_IGNORED,             /* STATE_CANCELED */
        STATE_EVENT_IGNORED,             /* STATE_STARTCOOK */
        STATE_HEATING,                   /* STATE_HEATING */
        STATE_EVENT_IGNORED,             /* STATE_BOILED */
        STATE_STEAMING,                  /* STATE_STEAMING */
        STATE_KEEPWARM,                  /* STATE_KEEPWARM */
    }; 
    SM_ExternalEvent(self, TRANSITIONS[self->currentState]);
    STATIC_ASSERT((sizeof(TRANSITIONS)/sizeof(SM_States)) == STATE_MAX_STATES);
}

void StartPoll()
{
    riceCookerStatus.pollActive = true;
}

void StopPoll()
{
    riceCookerStatus.pollActive = false;
}

uint8_t RiceCooker_IsPollActive() 
{ 
    return riceCookerStatus.pollActive;
}

static void Entry_Idle(SM_StateMachine *self)
{
    printf("%s Entry_Idle\n", self->name);
    /* reset timer */
    riceCookerStatus.timer = 0;
    StartPoll();
}

static void StateAction_Idle(SM_StateMachine *self)
{
    /* cool down until temperature reaches 25 */
    if(riceCookerStatus.temprature > 25)
    {
        _delay_ms(10);
        riceCookerStatus.temprature--;
        printf("%s StateAction_Idle -> Temperature: %d\n", self->name, riceCookerStatus.temprature);
    }
    else
    {
        StopPoll();
    }
    
}

static void Exit_Idle(SM_StateMachine *self)
{
    printf("%s Exit_Idle\n", self->name);
    /* reset timer */
    riceCookerStatus.timer = 0;
    StopPoll();
}

static void StateAction_Completed(SM_StateMachine *self)
{
    printf("%s StateAction_Completed\n", self->name);
    SM_InternalEvent(self, STATE_IDLE);
}

static void StateAction_Canceled(SM_StateMachine *self)
{
    printf("%s StateAction_Canceled\n", self->name);
    StopPoll();
    SM_InternalEvent(self, STATE_IDLE);
}

static void StateAction_StartCook(SM_StateMachine *self)
{
    printf("%s StateAction_StartCook\n", self->name);    
    SM_InternalEvent(self, STATE_HEATING);
}

static uint8_t Guard_StartCook(SM_StateMachine *self)
{
    printf("%s Guard_StartTest\n", self->name);
    /* if temperature is too hot, rice cooker refuses to start cooking */
    if (riceCookerStatus.temprature < 100)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void Entry_Heating(SM_StateMachine *self)
{
    printf("%s Entry_Heating\n", self->name);
    StartPoll();
}

static void StateAction_Heating(SM_StateMachine *self)
{
    printf("%s StateAction_Heating -> Temperature: %d\n", self->name, riceCookerStatus.temprature);
    /* heat until temperature reaches 100 */
    _delay_ms(10);
    if(riceCookerStatus.temprature >= 100)
    {
        SM_InternalEvent(self, STATE_BOILED);
    }
    riceCookerStatus.temprature++;
}

static void Exit_Heating(SM_StateMachine *self)
{
    printf("%s Exit_Heating\n", self->name);
    StopPoll();
}

static void StateAction_Boiled(SM_StateMachine *self)
{
    printf("%s StateAction_Boiled\n", self->name);
    SM_InternalEvent(self, STATE_STEAMING);
}

static void Entry_Steaming(SM_StateMachine *self)
{
    printf("%s Entry_Steaming\n", self->name);
    StartPoll();
}

static void StateAction_Steaming(SM_StateMachine *self)
{
    printf("%s StateAction_Steaming -> Timer: %d\n", self->name, riceCookerStatus.timer);
    /* steam until timer reaches 10 */
    _delay_ms(10);
    if(riceCookerStatus.timer >= 10)
    {
        SM_InternalEvent(self, STATE_KEEPWARM);
    }
    riceCookerStatus.timer++;
}

static void Exit_Steaming(SM_StateMachine *self)
{
    printf("%s Exit_Steaming\n", self->name);
    StopPoll();
}

static void Entry_KeepWarm(SM_StateMachine *self)
{
    printf("%s Entry_KeepWarm\n", self->name);
    StartPoll();
}

static void StateAction_KeepWarm(SM_StateMachine *self)
{
    /* keep temerature at 80 */
    if(riceCookerStatus.temprature > 80)
    {
        _delay_ms(10);
        riceCookerStatus.temprature--;
        printf("%s StateAction_KeepWarm -> Temperature: %d\n", self->name, riceCookerStatus.temprature);
    }
}

static void Exit_KeepWarm(SM_StateMachine *self)
{
    printf("%s Exit_KeepWarm\n", self->name);
    StopPoll();
}