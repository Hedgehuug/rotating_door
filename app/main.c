#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
/// Finite State Machine Library
#include "fsm_functions/fsm.h"
/// Development Console Library
#include "console_functions/keyboard.h"
#include "console_functions/display.h"
#include "console_functions/devConsole.h"


#define OPEN 1
#define CLOSED 0

extern char * eventEnumToText[];
extern char * stateEnumToText[];

event_t event;
state_t state;

int gateStatus = CLOSED;
//====== Change this!!!

// Local function prototypes State related
void S_State1_onEntry(void);
void S_State1_onExit(void);

void S_State2_onEntry(void);
void S_State2_onExit(void);

void S_Shutdown_onEntry(void);

///Subsystem2 (simulation) functions
event_t SubSystem1InitialisationSimulation(void);

///Subsystem2 (simulation) functions
event_t SubSystem2EventSimulation(void);

///Helper function example
void delay_us(uint32_t d);

// Main
int main(void)
{

   /// Define the state machine model
   /// First the state and the pointer to the onEntry and onExit functions
   //           State                           onEntry()               onExit()
   FSM_AddState(S_START,      &(state_funcs_t){  NULL,                  NULL});
   FSM_AddState(S_INIT,     &(state_funcs_t){  S_State1_onEntry,      S_State1_onExit });
   FSM_AddState(S_CLOSED,     &(state_funcs_t){  S_State2_onEntry,      S_State2_onExit });
   FSM_AddState(S_ERROR,   &(state_funcs_t){  S_Shutdown_onEntry,    NULL            });
   FSM_AddState(S_O_SPINNING, &(state_funcs_t){ NULL, NULL});
   FSM_AddState(S_O_SLIDING, &(state_funcs_t){ NULL, NULL});
   FSM_AddState(S_LOCKED, &(state_funcs_t){ NULL, NULL});

   /// Second the transitions
   //                                 From            Event                To
   // Init Transitions
   FSM_AddTransition(&(transition_t){ S_START, E_INIT, S_INIT });
   FSM_AddTransition(&(transition_t){ S_INIT, E_O_SLIDING, S_O_SLIDING });
   FSM_AddTransition(&(transition_t){ S_INIT, E_O_SPINNING, S_O_SPINNING });
   // Closing Transitions
   FSM_AddTransition(&(transition_t){ S_O_SLIDING, E_CLOSE, S_CLOSED });
   FSM_AddTransition(&(transition_t){ S_O_SPINNING, E_CLOSE, S_CLOSED });
   // Opening Transitions
   FSM_AddTransition(&(transition_t){ S_CLOSED, E_O_SLIDING, S_O_SLIDING });
   FSM_AddTransition(&(transition_t){ S_CLOSED, E_O_SPINNING, S_O_SPINNING });
   // Error Transitions
   FSM_AddTransition(&(transition_t){ S_CLOSED, E_ERROR, S_ERROR });
   FSM_AddTransition(&(transition_t){ S_O_SLIDING, E_ERROR, S_ERROR });
   FSM_AddTransition(&(transition_t){ S_O_SPINNING, E_ERROR, S_ERROR });
   FSM_AddTransition(&(transition_t){ S_INIT, E_ERROR, S_ERROR });
   FSM_AddTransition(&(transition_t){ S_ERROR, E_INIT, S_INIT });
   // Locking and Unlocking transitions
   FSM_AddTransition(&(transition_t){ S_LOCKED, E_UNLOCK, S_CLOSED });
   FSM_AddTransition(&(transition_t){ S_CLOSED, E_LOCK, S_LOCKED });


   // Should unexpected events in a state be flushed or not?
   FSM_FlushEnexpectedEvents(true);

   /// Start the state machine
///   FSM_RunStateMachine(S_START, E_START);

   /// Use this test function to test your model
   FSM_RevertModel();

   return 0;
}



/// Local function prototypes State related

void S_State1_onEntry(void)
{
   event_t nextevent;

   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);

   /// Simulate the initialisation
   nextevent = SubSystem1InitialisationSimulation();

   FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_State1_onExit(void)
{
   DCSdebugSystemInfo("Nothing to in this onexit function");
}

void S_State2_onEntry(void)
{
   event_t nextevent;

   /// Simulate the initialisation
   nextevent = SubSystem2EventSimulation();

   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);

   FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_State2_onExit(void)
{
   DCSdebugSystemInfo("Nothing to in this onexit function");
}

void S_Shutdown_onEntry(void)
{
   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);
   DSPshow(5, "Shutting down (Event3) selected ");
   DCSdebugSystemInfo("Shutting down the system");
   exit(0);
}

///Subsystem (simulation) functions
event_t SubSystem1InitialisationSimulation(void)
{
   int response;
   DSPinitialise();
   DSPshowDisplay();
   KYBinitialise();

   response = DCSsimulationSystemInputChar("Initialize result: 1 no error, 2 error", "12");

   switch (response)
   {
      case '1':
         DSPshow(2,"System Initialized No errors");
         DCSdebugSystemInfo("Example debug message going to next state");
         DCSdebugSystemInfo("Return with event E_EVENT1");
         return(E_EVENT1);
         break;
      case '2':
         DSPshow(2,"System Initialized error! System Shutdown");
         DCSdebugSystemInfo("Example debug message going to next state");
         DCSdebugSystemInfo("Return with event E_EVENT2");
         return(E_EVENT2);
         break;
      default:
         DCSdebugSystemInfo("Undefined this should not happen");
         DCSdebugSystemInfo("Return with event E_EVENT3");
         return(E_EVENT3);
   }
}

event_t SubSystem2EventSimulation(void)
{
   int response;
   response = DCSsimulationSystemInputChar(\
                 "Example subsystem A: Select Event2, B: Select Event3", "AB");
   switch (response)
   {
      case 'A':
         DSPshow(3,"Event2 is selected");
         return(E_EVENT2);
         break;
      case 'B':
         DSPshow(3,"Event3 is selected");
         return(E_EVENT3);
         break;
      default:
         DCSdebugSystemInfo("Undefined this should not happen");
         DCSdebugSystemInfo("Return with event E_EVENT3");
         return(E_EVENT3);
   }
}

void soundAlarm(void)
{
   DSPshow(6, "System alarm \a\a\a\a\a\a");
}

// simulate delay in microseconds
void delay_us(uint32_t d)
{
   DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
   sleep(d/10000);
}
