#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
    E_NO,                ///< Used for initialisation of an event variable
    E_START,
    E_EVENT1,
    E_EVENT2,
    E_EVENT3,
    E_INIT,
    E_O_SLIDING,
    E_O_SPINNING,
    E_CLOSE,
    E_ERROR,
    E_LOCK,
    E_UNLOCK
} event_t;

#endif
