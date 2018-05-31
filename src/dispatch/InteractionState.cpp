#include "InteractionState.h"

InteractionState::InteractionState() :
	_state(NO_INTERACTION),
	_ipIncr(NO_CONNECTION_IPINCR)
{}

void InteractionState::_triggerStateTransition(PoiCommand cmd) {
	
	PoiCommandType type = cmd.getType();
    PoiState nextState = _state;

    // state changes based on click events and current state
	switch(type) {
		case BUTTON0_LONGCLICK:
		if (_state == NO_INTERACTION) {

            // stop processing
			sendRawToDispatch( {STOP_PROC, 0, 0, 0, 0, 0}, INTS); 

            // start of ip conf
            //sendRawToDispatch( {DISPLAY_IP, _ipIncr, 1, 0, 0, 0}, INTS ); // only pale white background
			//nextState = IP_CONFIG;

            // skip ip conf for now
            nextState = WAIT_FOR_PROGSTART;
		}
		else if (_state == IP_CONFIG) {
			// end of ip conf
			 if (_ipIncr == NO_CONNECTION_IPINCR) {
                sendRawToDispatch( {DISCONNECT, 0, 0, 0, 0, 0}, INTS );
            }
            else {
                sendRawToDispatch( {CONNECT, _ipIncr, 0, 0, 0, 0}, INTS );
            }
            sendRawToDispatch( {ANIMATE, PALE_WHITE, 1, 15, 0, 50}, INTS ); 
			nextState = WAIT_FOR_PROGSTART;
		}
		else {
			nextState = NO_INTERACTION;
		}
		break;
		
		case BUTTON0_CLICK:
        if (_state == IP_CONFIG) {
    	    _incrementIp();
   	        sendRawToDispatch( {DISPLAY_IP, _ipIncr, 1, 0, 0, 0}, INTS );
     	   // state does not change
        }
        else if (_state == WAIT_FOR_PROGSTART) {
        	sendRawToDispatch( {START_PROG, 0, 0, 0, 0, 0}, INTS );
            nextState = NO_INTERACTION;
        }
        break;

        
        case BUTTON0_RELEASE:
        // no action defined
        break;
        
        case POISTATUS:
        _monitor.logStatus();
        break;

        case SELFTEST:
        selftest_start(5);
        break;

		
		default:
		break;
	}

    if (nextState != _state) {
        _state = nextState;
        LOGI(INTS, "New State: %s", toString().c_str());
    }

}

void InteractionState::_incrementIp() {
	
	if (_ipIncr == NO_CONNECTION_IPINCR) {
		_ipIncr = 0;
	}
	else if (_ipIncr > N_POIS-2) {
		_ipIncr = NO_CONNECTION_IPINCR;
	}
	else {
		_ipIncr++;
	}
}

bool InteractionState::commandFilter(PoiCommand cmd) {
	
    _triggerStateTransition(cmd);
		
    return ! cmd.isInternalCommand();
}

String InteractionState::toString(){

    String stateAsString;

    switch(_state) {
        case NO_INTERACTION:
        stateAsString = String("NO_INTERACTION");
        break;

        case IP_CONFIG:
        stateAsString = String("IP_CONFIG");
        break;

        case WAIT_FOR_PROGSTART:
        stateAsString = String("WAIT_FOR_PROGSTART");
        break;

        default:
        stateAsString = String("UNKNOWN_STATE");
        break;
    }

    return stateAsString;
}
