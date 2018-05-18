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
			_sendRawCommand( {STOP_PROC, 0, 0, 0, 0, 0} ); 
            // TODO base next actions on events
            // otherwise animation below is swallowed by stop proc
            delay(1000); 
            _sendRawCommand( {SHOW_RGB, 0, 0, 0, 0, 0} ); // black
            _sendRawCommand( {ANIMATE, PALE_WHITE, 1, 15, 0, 50} ); 

            // start of ip conf
            //_sendRawCommand( {DISPLAY_IP, _ipIncr, 1, 0, 0, 0} ); // only pale white background
			//nextState = IP_CONFIG;

            // skip ip conf for now
            nextState = WAIT_FOR_PROGSTART;
		}
		else if (_state == IP_CONFIG) {
			// end of ip conf
			 if (_ipIncr == NO_CONNECTION_IPINCR) {
                _sendRawCommand( {DISCONNECT, 0, 0, 0, 0, 0} );
            }
            else {
                _sendRawCommand( {CONNECT, _ipIncr, 0, 0, 0, 0} );
            }
            _sendRawCommand( {ANIMATE, PALE_WHITE, 1, 15, 0, 50} ); 
			nextState = WAIT_FOR_PROGSTART;
		}
		else {
			nextState = NO_INTERACTION;
		}
		break;
		
		case BUTTON0_CLICK:
        if (_state == IP_CONFIG) {
    	    _incrementIp();
   	        _sendRawCommand( {DISPLAY_IP, _ipIncr, 1, 0, 0, 0} );
     	   // state does not change
        }
        else if (_state == WAIT_FOR_PROGSTART) {
        	_sendRawCommand( {START_PROG, 0, 0, 0, 0, 0} );
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

void InteractionState::_sendRawCommand(RawPoiCommand rawCmd) {

    PoiCommand cmd = PoiCommand(rawCmd);
	LOGD(INTS,  "Sending cmd to dispatch: %s", cmd.toString().c_str());
    
      if (xQueueSendToBack(dispatchQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
        LOGE(INTS, "Could not add cmd to dispatchQueue.");
      }
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
