#include "RobustWiFiServer.h"

RobustWiFiServer::RobustWiFiServer():
  _currentTransition(UNCONFIGURED,UNCONFIGURED),
  _currentState(UNCONFIGURED),
  _targetState(UNCONFIGURED),
  _condition(NO_ERROR),
  _targetUpdated(false)
{};

void RobustWiFiServer::init(IPAddress ip, IPAddress gateway, IPAddress subnet, 
  uint16_t serverPort, String ssid, String wifiPassword, 
  uint8_t serverPortIncrOnError, uint8_t numPortVarOnError) {  

  _ip = ip;
  _gateway = gateway;
  _subnet = subnet;
  _serverPort = serverPort;
  _initialServerPort = serverPort;
  _ssid = ssid;
  _wifiPassword = wifiPassword;
  _serverPortIncrOnError = serverPortIncrOnError;
  _numPortVarOnError = numPortVarOnError;
  _targetState = UNCONFIGURED; 
  _targetState2 = UNCONFIGURED;
  _currentState = UNCONFIGURED;
  _lastDataAvailableCount = 0;

  wifi_init();
  }

void RobustWiFiServer::connect(){
  LOGI(RWIFIS, "Connection request received...");
  _targetState = DATA_AVAILABLE;
  _targetState2 = _targetState; // no second target
  _targetUpdated = true;
}

void RobustWiFiServer::connect(IPAddress ip, uint16_t port){
  if (ip == _ip && port == _serverPort) {
    LOGI(RWIFIS, "Connection request with ip %s:%d received...", 
      ip.toString().c_str(), port);
    _targetState = DATA_AVAILABLE;
    _targetState2 = _targetState; // no second target
  }
  else {
    LOGI(RWIFIS, "Connection request with new ip %s:%d received...", 
      ip.toString().c_str(), port);
    _targetState = UNCONFIGURED;
	  _ip = ip;
    _serverPort = port;
    _initialServerPort = port;
    _targetState2 = DATA_AVAILABLE;
  }
  _targetUpdated = true;
}

void RobustWiFiServer:: disconnect(){
  LOGI(RWIFIS, "Disconnect request received...");
  _targetState = UNCONFIGURED;
  _targetState2 = _targetState; // no second target
  _targetUpdated = true;
}

void RobustWiFiServer::clientDisconnect(){
	LOGI(RWIFIS, "Client disconnect request received...");
  _targetState = SERVER_LISTENING;
  _targetState2 = DATA_AVAILABLE;
  _targetUpdated = true;
}

ServerState RobustWiFiServer::getState() {
  return _currentState;
}

ServerCondition RobustWiFiServer::getCondition(){
  return _condition;
}

char RobustWiFiServer::readData(){
  return _client.read();             // read a byte
}

size_t RobustWiFiServer::writeData(uint8_t data){
  return _client.write(data);        // write data
}

size_t RobustWiFiServer::writeData(const uint8_t *buf, size_t size){
  return _client.write(buf, size);   // write data
}

Transition RobustWiFiServer::_determineNextTransition(){
  if (_targetState >= _currentState){
    return Transition(_currentState, getNextServerStateUp(_currentState));
  }
  return Transition(_currentState, getNextServerStateDown(_currentState));
}

Transition RobustWiFiServer::_getStepBackTransition(){
  // always step back from the "lowest" order state towards disconnect
  ServerState fromState = (_currentTransition.from < _currentTransition.to) ? 
    _currentTransition.from : _currentTransition.to;
  ServerState toState = getNextServerStateDown(fromState) ;
  return Transition(fromState, toState);
}

void RobustWiFiServer::_invokeAction(Transition& trans){

    // actions are only invoked once
  if (!trans.wasActionInvoked()) {
    if (trans.isEmptyTransition()){
      LOGV(RWIFIS, "No action in transition...");
      // do nothing
    }
    // connecting actions...
    else if (Transition(UNCONFIGURED, CONNECTED) == trans){
      LOGI(RWIFIS, "-> Connecting to wifi with SSID %s & ip %s:%d...", 
        _ssid.c_str(), _ip.toString().c_str(), _serverPort);
      wifi_start_sta(_ssid, _wifiPassword, _ip, _gateway, _subnet);
    }
    else if (Transition(CONNECTED, SERVER_LISTENING) == trans){
      LOGI(RWIFIS, "-> Starting server...");
      _server = WiFiServer(_serverPort);
      _server.begin();                      // bind and listen
    }
    else if (Transition(SERVER_LISTENING, CLIENT_CONNECTED) == trans){
      LOGI(RWIFIS, "-> Listening for clients...");
      _client = _server.available();        // accept - also checked at checkState()
    }   
    else if (Transition(CLIENT_CONNECTED, DATA_AVAILABLE) == trans){
      // dont show message for keepAlive signal
      if (_lastDataAvailableCount != 1) {
        LOGI(RWIFIS, "-> Waiting for data...");
      }
      // nothing to be done
    } 

    // disconnecting actions...
    else if (Transition(DATA_AVAILABLE, CLIENT_CONNECTED) == trans){
      LOGD(RWIFIS, "-> Stop receiving data...");
      // nothing to be done
    }    
    else if (Transition(CLIENT_CONNECTED, SERVER_LISTENING) == trans){
      LOGI(RWIFIS, "-> Disconnect clients...");
      _client.stop();
    }
    else if (Transition(SERVER_LISTENING, CONNECTED) == trans){
      LOGI(RWIFIS, "-> Stopping server...");
      _server.end();
    }
    else if (Transition(CONNECTED, UNCONFIGURED) == trans){
      LOGI(RWIFIS, "-> Stopping Wifi...");
      wifi_disconnect();
    }
    else {
      LOGE(RWIFIS, "ERROR. Unknown transition requested: %s", trans.toString().c_str());
    }

    // remember invocation time for timeout
    trans.setLastInvocationTime();
    // invoke action only once
    trans.setActionInvoked(true);
  }
}

bool RobustWiFiServer::_wasTransitionSuccessful(Transition trans){
  // transition with same from and to state are never successfull
  return trans.from != trans.to 
    && _checkState(trans.to);
}

// check whether a state that happen asynchronously
bool RobustWiFiServer::_checkState(ServerState state, bool debug){

  bool stateok = false;
  int dataCount = 0;
  LOGV(RWIFIS, "Checking wifistate %s", wiFiStateToString().c_str());
  switch(state){
    
    case UNCONFIGURED:
    stateok = (wifiState == WIFI_DISCONNECTED);
    break;

    case CONNECTED:
    stateok = (wifiState == WIFI_CONNECTED);
    break;

    case SERVER_LISTENING:
    // need to also check wifi status since server would not notice failing wifi
    stateok = (wifiState == WIFI_CONNECTED) && _server; // equals to is_listening()
    break;

    case CLIENT_CONNECTED:
    if (!_client.connected()) {
      _client = _server.available();
    }
    stateok =  (wifiState == WIFI_CONNECTED) && _client.connected();
    break;

    case DATA_AVAILABLE:
    dataCount = _client.available();
    if (dataCount != 0) {
      _lastDataAvailableCount = dataCount;
    }
    stateok =  (wifiState == WIFI_CONNECTED) && dataCount;
    break;

    default: // unknown state
    stateok = false;
    break;
  } 

  // reset to show waiting data msg again
  if (state < CLIENT_CONNECTED) {
    _lastDataAvailableCount = 0; 
  }

  if (!stateok && debug) {
    _printInternalState();
  }
  return stateok;
}

void RobustWiFiServer::_printInternalState(){
	LOGD(RWIFIS, "WiFi state: %s", wiFiStateToString().c_str());
  LOGD(RWIFIS, " Server: %s %s", 
    _server ? "conn " : "nc ", _server.available() ? "avail" : "na");
  LOGD(RWIFIS, " Client: %s %s", 
    _client.connected() ? "conn " : "nc ", _client.available() ? "avail" : "na");
}

bool RobustWiFiServer::_timeoutReached(){
  // timeout does not make sense when we are not even configured
  if (_currentState == UNCONFIGURED) {
    return false;
  }

  uint32_t now = millis();
  if (_currentState == CLIENT_CONNECTED) {
    return (now - _currentTransition.getLastInvocationTime() > TRANSITION_TIMEOUT_DATA_AVAILABLE);
  }
  return (now - _currentTransition.getLastInvocationTime() > TRANSITION_TIMEOUT);
}

void RobustWiFiServer::loop(){

  // new target 
  if (_targetUpdated) {
    LOGD(RWIFIS, "Target updated to: %s", serverStateToString(_targetState).c_str());
    _condition.resetError();
    _currentTransition = _determineNextTransition();
    LOGD(RWIFIS, "NEW updated Transition: %s", _currentTransition.toString().c_str());
    _targetUpdated = false;
  }

  // check whether we are either in current state of 'to' state
  else if (!_checkState(_currentState, true)
      && !_checkState(_currentTransition.to, true)){

    // we are neither in 'from' nor in 'to' state
    _condition.error = STATE_CHECK_FAILED;

    // switch back one state
    _currentTransition = _getStepBackTransition();

    if (_currentState == DATA_AVAILABLE) {
      LOGD(RWIFIS, "WARNING. Checking failed for state %s", serverStateToString(_currentState).c_str());
      LOGD(RWIFIS, "Stepping back with %s", _currentTransition.toString().c_str());
    }
    else {
      LOGW(RWIFIS, "WARNING. Checking failed for state %s", serverStateToString(_currentState).c_str()); 
      LOGI(RWIFIS, "Stepping back with %s", _currentTransition.toString().c_str());     
    }
    
    _currentState = _currentTransition.from;
  }

  // check whether transition was successful
  else if ( _wasTransitionSuccessful(_currentTransition)) {
    _currentState = _currentTransition.to;
    _condition.resetError();
    if (_currentState == _targetState) { 
      // dont show message for keepAlive signal
      if (_currentState == DATA_AVAILABLE && _lastDataAvailableCount != 1) {
        LOGI(RWIFIS, "Data available: %d bytes.", _lastDataAvailableCount);
      }
      if (_targetState2 == _targetState) {
        // final target reached: create no-action transition
        LOGD(RWIFIS, "Final Target reached: %s", serverStateToString(_targetState).c_str());
        _currentTransition = Transition(_currentState,_currentState);
      }
      else {
        LOGD(RWIFIS, "Target reached: %s, new target: %s", 
          serverStateToString(_targetState).c_str(),
          serverStateToString(_targetState2).c_str());
        _targetState = _targetState2;
        _currentTransition = _determineNextTransition();
      }
    }
    else {
      _currentTransition = _determineNextTransition();
    }
    LOGD(RWIFIS, "NEW Transition: %s", _currentTransition.toString().c_str());
  }

  // we stayed too long in this state, step back one level and re-try
  else if (!_currentTransition.isEmptyTransition() && _timeoutReached()){
    _condition.error = TRANSITION_TIMEOUT_REACHED;
    _condition.numberOfTimeouts++;
    // we first revert transition to be on the safe side
    // next iteration will automatically repeat action then
    _currentTransition = _getStepBackTransition();
    LOGI(RWIFIS, "Timeout reached. Will step back from %s to %s.", 
      serverStateToString(_currentTransition.from).c_str(),
      serverStateToString(_currentTransition.to).c_str());
    // if we failed to connect we need to change the
    if (_currentState == CONNECTED) {
      // maximal number of different ports is 5, then return to initial port
      // otherwise we are hard to catch by the clients
      if (_condition.numberOfTimeouts > _numPortVarOnError) {
        _serverPort = _initialServerPort;
      }
      else {
        _serverPort+=_serverPortIncrOnError;
      }
      LOGI(RWIFIS, "Server port updated to %d.", _serverPort);
    }

  }

  _invokeAction(_currentTransition);
}
