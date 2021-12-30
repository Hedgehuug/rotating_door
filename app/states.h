#ifndef STATES_H
#define STATES_H

typedef enum {
    S_NO,               ///< Used for initialisation if state is not yet known
    S_START,
    S_STATE1,           ///< Initial state
    S_STATE2,           ///< Initialised subsystems
    S_SHUTDOWN,         ///< System shutdown
    S_INIT,
    S_O_SLIDING,
    S_O_SPINNING,
    S_CLOSED,
    S_LOCKED,
    S_ERROR
    //end
} state_t;

#endif
