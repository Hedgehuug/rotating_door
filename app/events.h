#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
    E_NO,                ///< Used for initialisation of an event variable
    E_START,
    E_INIT,
    E_O_SLIDING,
    E_O_SPINNING,
    E_CLOSE,
    E_ERROR,
    E_LOCK,
    E_UNLOCK,
    E_SHUTDOWN
} event_t;

#endif
