#include "PoiCommand.h"

#define NO_CONNECTION_IPINCR 254

extern xQueueHandle dispatchQueue;

enum PoiState {
    NO_INTERACTION,
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

    void _triggerStateTransition(PoiCommand cmd);
    void _sendRawCommand(RawPoiCommand cmd);
    void _incrementIp();
};