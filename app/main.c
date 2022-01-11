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

#define SLIDING 0
#define SPINNING 1

extern char * eventEnumToText[];
extern char * stateEnumToText[];

event_t event;
state_t state;

int gateStatus = OPEN;
//====== Change this!!!

// Local function prototypes State related
void S_State1_onEntry(void);
void S_State1_onExit(void);

void S_State2_onEntry(void);
void S_State2_onExit(void);


// User-defined state functions:
void S_start_onEntry(void);
void S_start_onExit(void);

 void S_init_onEntry(void);
 void S_init_onExit(void);

 void S_error_onEntry(void);
 void S_error_onExit(void);

 void S_open_onEntry(void);
 void S_open_onExit(void);


 void S_closed_onEntry(void);
 void S_closed_onExit(void);

 void S_locked_onEntry(void);
 void S_locked_onExit(void);


void S_Shutdown_onEntry(void);
void S_Shutdown_onExit(void);

///Subsystem2 (simulation) functions
event_t InitialisationSimulation(void);

///Subsystem2 (simulation) functions
event_t SubSystem2EventSimulation(void);

event_t SlidingOpenSimulation(void);
event_t SpinningOpenSimulation(void);

event_t ClosedSimulation(void);


///Helper function example
void delay_us(uint32_t d);

int open = CLOSED;
int operationMode = SLIDING;

// Main
int main(void)
{
   /// Define the state machine model
   /// First the state and the pointer to the onEntry and onExit functions
   //           State                           onEntry()               onExit()
   FSM_AddState(S_START,      &(state_funcs_t){  S_start_onEntry, S_start_onExit});
   FSM_AddState(S_INIT,     &(state_funcs_t){  S_init_onEntry, S_init_onExit });
   FSM_AddState(S_CLOSED,     &(state_funcs_t){  S_closed_onEntry, S_closed_onExit});
   FSM_AddState(S_ERROR,   &(state_funcs_t){  S_error_onEntry, S_error_onExit});
   FSM_AddState(S_O_SLIDING, &(state_funcs_t){ S_open_onEntry, S_open_onExit});
   FSM_AddState(S_O_SPINNING, &(state_funcs_t){ S_open_onEntry, S_open_onExit});
   FSM_AddState(S_LOCKED, &(state_funcs_t){ S_locked_onEntry, S_locked_onExit});

   /// Second the transitions
   //                                 From      Event   To
   // Init Transitions
   FSM_AddTransition(&(transition_t){ S_START, E_START, S_START });
   FSM_AddTransition(&(transition_t){ S_START, E_INIT, S_INIT });
   FSM_AddTransition(&(transition_t){ S_INIT, E_O_SLIDING, S_O_SLIDING });
//   FSM_AddTransition(&(transition_t){ S_INIT, E_O_SPINNING, S_O_SPINNING });
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
   // Shutdown
   FSM_AddTransition(&(transition_t){ S_CLOSED, E_SHUTDOWN, S_SHUTDOWN});
   FSM_AddTransition(&(transition_t){ S_ERROR, E_SHUTDOWN, S_SHUTDOWN });



   // Should unexpected events in a state be flushed or not?
   FSM_FlushEnexpectedEvents(false);

   /// Start the state machine
   FSM_RunStateMachine(S_START, E_START);

   /// Use this test function to test your model
   FSM_RevertModel();

   return 0;
}

/// Local function prototypes State related
// START STATE
void S_start_onEntry(void)
{
   event_t nextevent;

   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);

   /// Simulate the initialisation
   nextevent = E_INIT;

   FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_start_onExit(void)
{
   DCSdebugSystemInfo("Nothing to in this onexit function");
}



// INIT STATE
void S_init_onEntry(void)
{
   event_t nextevent;

   /// Simulate the initialisation
   nextevent = InitialisationSimulation();

   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);

   FSM_AddEvent(nextevent);           /// Internal generated event
}

void S_init_onExit(void)
{
    DCSdebugSystemInfo("Successful Initialisation");
}

// FOR INITIALISING IN CERTAIN WAYS
event_t InitialisationSimulation(void)
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
         DCSdebugSystemInfo("System is initialising into default mode: sliding");
         DCSdebugSystemInfo("Return with event E_O_SLIDING");
         return(E_O_SLIDING);
         break;
      case '2':
         DSPshow(2,"System Initialized error! System Shutdown");
         //DCSdebugSystemInfo("Example debug message going to next state");
         DCSdebugSystemInfo("Return with event E_ERROR");
         return(E_ERROR);
         break;
      default:
         DCSdebugSystemInfo("Undefined this should not happen");
         DCSdebugSystemInfo("Return with event E_ERROR");
         return(E_ERROR);
   }
}


// ERROR STATE
void S_error_onEntry(void)
{
    int response;
   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);
   DSPshow(5, "An Error has occured");
   DCSdebugSystemInfo("System has encountered an exception - please re-initialise or shutdow the system");
   //exit(0);
   response = DCSsimulationSystemInputChar("Attempt to re-initialise?\nYes: 1\nNo: 2","12");

   switch (response)
   {
        case '1':
            DCSdebugSystemInfo("System will attempt to reinitialise");
            DCSdebugSystemInfo("Returning E_INIT event");
            FSM_AddEvent(E_INIT);
            break;
        case '2':
            DCSdebugSystemInfo("System Shutting down");
            DCSdebugSystemInfo("Returning with E_SHUTDOWN");
            FSM_AddEvent(E_SHUTDOWN);
            break;
        default:
            FSM_AddEvent(E_SHUTDOWN);
   }
}
void S_error_onExit(void)
{
   //DCSdebugSystemInfo("Nothing to in this onexit function");
   DSPshow(5,"");
}



// OPEN EVENT
void S_open_onEntry(void)
{
   open = OPEN;
//   DCSdebugSystemInfo("Doors are opening in sliding mode");
   switch (operationMode)
   {
    case SLIDING:
       FSM_AddEvent(SlidingOpenSimulation());
       break;
    case SPINNING:
       FSM_AddEvent(SpinningOpenSimulation());
       break;
   }

}

void S_open_onExit(void)
{
    DCSdebugSystemInfo("Closing doors");
    // Switches global variable to closed
    open = CLOSED;
}

// OPEN SLIDING SIMULATION
event_t SlidingOpenSimulation(void)
{
   int response;
   DCSdebugSystemInfo("Opening the Door in sliding-mode");
   response = DCSsimulationSystemInputChar(
                 "\n\tSimulate an Event:\n\tClose Door: 1\n\tRaise Error 2\n", "12");

   switch (response)
   {
      case '1':
         DSPshow(3,"Doors will now close");
         return(E_CLOSE);
         break;
      case '2':
         DSPshow(3,"");
         return(E_ERROR);
         break;
      default:
         DCSdebugSystemInfo("Undefined this should not happen");
         DCSdebugSystemInfo("Return with event E_ERROR");
         return(E_ERROR);
   }

}


// SPINNING OPEN FUNCTIONS
event_t SpinningOpenSimulation(void)
{
   int response;
   DCSdebugSystemInfo("Opening the Door in spinning-mode");
   response = DCSsimulationSystemInputChar(
                 "\n\tSimulate an Event:\n\tClose Door: 1\n\tRaise Error 2\n", "12");

   switch (response)
   {
      case '1':
         DSPshow(3,"Doors will stop spinning");
         return(E_CLOSE);
         break;
      case '2':
         DSPshow(3,"");
         return(E_ERROR);
         break;
      default:
         DCSdebugSystemInfo("Undefined this should not happen");
         DCSdebugSystemInfo("Return with event E_ERROR");
         return(E_ERROR);
   }

}



// CLOSED STATE
void S_closed_onEntry(void)
{
   DCSdebugSystemInfo("Current state: %s\n", stateEnumToText[state]);
   DSPshow(3, "Doors are now closed");

   FSM_AddEvent(ClosedSimulation());
}
void S_closed_onExit(void)
{
   DCSdebugSystemInfo("Nothing to in this onexit function");

}

event_t ClosedSimulation(void){
    int response;
    while(1)
    {
        response = DCSsimulationSystemInputChar("Simulate an input:\n--Open(in Selected mode): 1\n--Sliding: 2\n--Spinning: 3\n--Lock Door: 4\n--Raise Error: 5\n--Shut Down: 6\n","123456");
        switch (response)
        {
            case '1':
                switch(open)
                {
                DCSdebugSystemInfo("Doors will now open");
                    case 0:
                        return(E_O_SLIDING);
                        break;
                    case 1:
                        return(E_O_SPINNING);
                        break;
                }

                break;
            case '2':
                DCSdebugSystemInfo("Setting operation-mode to Sliding");
                DCSdebugSystemInfo("operationMode <= SLIDING");
                operationMode = SLIDING;
                break;

            case '3':
                DCSdebugSystemInfo("Setting operation-mode to Spinning");
                DCSdebugSystemInfo("operationMode <= SPINNING");
                operationMode = SPINNING;
                break;

            case '4':
                return(E_LOCK);
                break;
            case '5':
                return(E_ERROR);
                break;
            case '6':
                return(E_SHUTDOWN);
                break;
            default:
                break;
        }
    }

    return(E_ERROR);
}




void S_locked_onEntry(){
    int response;
    while (1)
    {
        response = DCSsimulationSystemInputChar("Enter 1 to unlock Doors: ","1");
        if (response == '1'){
            FSM_AddEvent(E_UNLOCK);
            return;
        }
        else {
            DCSdebugSystemInfo("Doors will remain closed");
        }
    }
}
void S_locked_onExit(){
    DCSdebugSystemInfo("Doors are unlocking, they will return to their last operating state");
}


void S_Shutdown_onEntry(void)
{
    DSPshow(3,"SHUTTING DOWN");
    DCSdebugSystemInfo("Performing checks and shutting down");
    DCSdebugSystemInfo("For safety reasons the doors will remain open");
}
void S_Shutdown_onExit(void){
    DSPshowDelete(5,"---SYSTEM OFFLINE---");
    exit(0);
}




//void soundAlarm(void)
//{
//   DSPshow(6, "System alarm \a\a\a\a\a\a");
//}


//// simulate delay in microseconds
void delay_us(uint32_t d)
{
   DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
   sleep(d/10000);
}
