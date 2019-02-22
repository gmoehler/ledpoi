#include "InteractionState.h"

void IRAM_ATTR onIpConfigTimeout(){

    // timeout for wifi configuration
    sendRawToDispatch( {TIMEOUT_IP_CONFIG, 0, 0, 0, 0, 0}, INTS );
}

InteractionState::InteractionState() :
	_state(WAIT_FOR_PROGSTART),
	_ipIncr(NO_CONNECTION_IPINCR)
{
    // todo: use PoiTimer
    _timeoutTimer = timerBegin(0, 80, true);                  //timer 0, div 80
    timerAttachInterrupt(_timeoutTimer, &onIpConfigTimeout, true);  //attach callback
    timerAlarmWrite(_timeoutTimer, IP_CONFIG_TIMEOUT * 1000 * 1000, false); //set time in us
}

bool InteractionState::commandFilter(PoiCommand cmd) {
	
    // look whether state changes
    // internal commands only trigger state changes
    _triggerStateTransition(cmd);

    // realize the command when it is not an internal command
    return ! cmd.isInternalCommand();
}

void InteractionState::_triggerStateTransition(PoiCommand cmd) {
	
	PoiCommandType type = cmd.getType();
    PoiState nextState = _state;

    // state changes based on click events and current state
	switch(type) {
        case START_IP_CONFIG:
        // not implemented without wifi
        break;

		case BUTTON0_LONGCLICK:
        if (_state == IP_CONFIG) {
            // end of ip conf: connect or do nothing
            if (_ipIncr == NO_CONNECTION_IPINCR) {
                sendRawToDispatch( {SET_IP, _ipIncr, 0, 0, 0, 0}, INTS );
                sendRawToDispatch( {DISCONNECT, 0, 0, 0, 0, 0}, INTS );
            }
            else {
                sendRawToDispatch( {SET_IP, _ipIncr, 0, 0, 0, 0}, INTS );
                sendRawToDispatch( {CONNECT, 0, 0, 0, 0, 0}, INTS );
            }
            sendRawToDispatch( {ANIMATE, PALE_WHITE, 1, 10, 0, 50}, INTS );
            timerAlarmDisable(_timeoutTimer); 
            nextState = WAIT_FOR_PROGSTART;
        }
        else {
            // stop processing
            sendRawToDispatch( {STOP_PROC, 0, 0, 0, 0, 0}, INTS); 

            // continue waiting for program start
            nextState = WAIT_FOR_PROGSTART;
        }
		break;
		
		case BUTTON0_CLICK:
        if (_state == IP_CONFIG) {
    	    // _incrementIp();
   	        sendRawToDispatch( {DISPLAY_IP, _ipIncr, 1, 0, 0, 0}, INTS );
            timerWrite(_timeoutTimer, 0); //reset timer (feed watchdog)
     	   // state does not change
        }
        // at any other time a button click starts the program
        else {
        	sendRawToDispatch( {PLAY_SPIFFS_IMAGE, 0, 0, 0, 0, 0}, INTS );
            nextState = WAIT_FOR_PROGSTART;
        }
        break;

        case TIMEOUT_IP_CONFIG:
        if (_state == IP_CONFIG) {
            LOGI(INTS, "Timeout for IP config reached - stay disconnected.");
            sendRawToDispatch({SHOW_RGB, 0, 0, 0, 0, 200}, MAIN_T); // black
            sendRawToDispatch( {SHOW_RGB, 5, 5, 5, 2, 254}, INTS );
            sendRawToDispatch({SHOW_RGB, 0, 0, 0, 0, 200}, MAIN_T); // black
            nextState = WAIT_FOR_PROGSTART;
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
            sendRawToDispatch( {ANIMATE, PALE_WHITE, 1, 15, 0, 50}, INTS );
        } 
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

/*
void InteractionState::_incrementIp() {
	
	if (_ipIncr == NO_CONNECTION_IPINCR) {
		_ipIncr = 0;
	}
	else if (_ipIncr >= N_POIS-1) {
        // after last ip we show option for no connection
		_ipIncr = NO_CONNECTION_IPINCR;
	}
	else {
		_ipIncr++;
	}
} */

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
