#ifndef INTERACTION_STATE_H
#define INTERACTION_STATE_H

#include "PoiCommand.h"
#include "PoiMonitor.h"
#include "selftest/selftestTask.h"  // to start self test
#include "memory/memoryTask.h"      // for ip increment loading

enum PoiState {
    NO_INTERACTION, // currently not used
    IP_CONFIG,
    WAIT_FOR_PROGSTART
};

class InteractionState {
public:
    InteractionState();
    bool commandFilter(PoiCommand cmd);

    String toString();

private:
    PoiState _state;
    uint8_t _ipIncr;
    PoiMonitor _monitor;
    hw_timer_t *_timeoutTimer = NULL;

    void _triggerStateTransition(PoiCommand cmd);
    void _incrementIp();
};

#endif