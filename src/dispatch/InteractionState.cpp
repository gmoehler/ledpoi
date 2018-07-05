#include "InteractionState.h"

InteractionState::InteractionState() :
	_state(NO_INTERACTION),
	_ipIncr(NO_CONNECTION_IPINCR)
{}

void InteractionState::_triggerStateTransition(PoiCommand cmd) {
	
	PoiCommandType type = cmd.getType();
    PoiState nextState = _state;
    bool isDisplayActive = false;

    // state changes based on click events and current state
	switch(type) {
		case BUTTON0_LONGCLICK:
		if (_state == NO_INTERACTION) {

            isDisplayActive = _monitor.isDisplayActive();

            // stop processing
			sendRawToDispatch( {STOP_PROC, 0, 0, 0, 0, 0}, INTS); 

            // ip config only when no display was active
            // start of ip conf: done on STOP_PROC_COMPLETED signal
            if (isDisplayActive) {
			    nextState = NO_INTERACTION;
            }
#ifdef DISABLE_WIFI
            else {
                nextState = WAIT_FOR_PROGSTART;
            }
#else 
            else {
                nextState = IP_CONFIG;
                _ipIncr = getIpIncrement(); // sync initially
            }
#endif
		}
		else if (_state == IP_CONFIG) {
			// end of ip conf
			 if (_ipIncr == NO_CONNECTION_IPINCR) {
                sendRawToDispatch( {SET_IP, _ipIncr, 0, 0, 0, 0}, INTS );
                sendRawToDispatch( {DISCONNECT, 0, 0, 0, 0, 0}, INTS );
            }
            else {
                sendRawToDispatch( {SET_IP, _ipIncr, 0, 0, 0, 0}, INTS );
                sendRawToDispatch( {CONNECT, 0, 0, 0, 0, 0}, INTS );
            }
            sendRawToDispatch( {ANIMATE, PALE_WHITE, 1, 10, 0, 50}, INTS ); 
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
        
        case POISTATUS:
        _monitor.logStatus();
        break;

        case SELFTEST:
        selftest_start(5);
        break;

        case STOP_PROC_COMPLETED:
        sendRawToDispatch( {SHOW_RGB, 0, 0, 0, 0, 0}, PROG_T ); // black
        if (_state == IP_CONFIG) {
             sendRawToDispatch( {DISPLAY_IP, _ipIncr, 1, 0, 0, 0}, INTS );
        }
        else {
            // program interrupted or prepare for start
            sendRawToDispatch( {ANIMATE, PALE_WHITE, 1, 15, 0, 50}, PROG_T );
        } 
        break;

        case START_PROG:
        // reset interaction state when program is started
        nextState = NO_INTERACTION;
        break;
		
		default:
        // let pass most commands
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
