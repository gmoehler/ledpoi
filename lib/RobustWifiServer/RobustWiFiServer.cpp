#include "RobustWiFiServer.h"

RobustWiFiServer::RobustWiFiServer():
  _currentTransition(DISCONNECTED,DISCONNECTED),
  _currentState(DISCONNECTED),
  _targetState(DISCONNECTED),
  _condition(NO_ERROR)
{};

void RobustWiFiServer::init(IPAddress ip, IPAddress gateway, IPAddress subnet, 
  uint16_t serverPort, String ssid, String wifiPassword) {  

  _ip = ip;
  _gateway = gateway;
  _subnet = subnet;
  _serverPort = serverPort;
  _ssid = ssid;
  _wifiPassword = wifiPassword;
  _targetState = DISCONNECTED; 
  _currentState = DISCONNECTED;

  _server = WiFiServer(serverPort);
  }

void RobustWiFiServer::connect(){
  LOGI(RWIFIS, "Connection request received...");
  _targetState = DATA_AVAILABLE;
}

void RobustWiFiServer::connect(IPAddress ip){
	// does not switch to new ip unless disconnected
	_ip = ip;
   connect();
}
  
void RobustWiFiServer:: disconnect(){
  LOGI(RWIFIS, "Disconnect request received...");
  _targetState = DISCONNECTED;
}

void RobustWiFiServer::clientDisconnect(){
	LOGI(RWIFIS, "Client disconnect request received...");
  _targetState = SERVER_LISTENING;
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
  if (_targetState == DATA_AVAILABLE){
    return _determineNextConnectTransition();
  }
  return _determineNextDisconnectTransition();
}
  

Transition RobustWiFiServer::_determineNextConnectTransition(){
  switch(_currentState) {
    case DISCONNECTED:
    return Transition(DISCONNECTED, CONNECTED);

    case ERR_SSID_NOT_AVAIL:
    return Transition(ERR_SSID_NOT_AVAIL, DISCONNECTED);

    case CONNECTED:
    return Transition(CONNECTED,SERVER_LISTENING);

    case SERVER_LISTENING:
    return Transition(SERVER_LISTENING, CLIENT_CONNECTED);

    case CLIENT_CONNECTED:
    return Transition(CLIENT_CONNECTED, DATA_AVAILABLE);

    default: // no action when we are in DATA_AVAILABLE state
    return Transition(_currentState,_currentState);
  }
}

Transition RobustWiFiServer::_determineNextDisconnectTransition(){
  switch(_currentState) {
    case DATA_AVAILABLE:
    return Transition(DATA_AVAILABLE, CLIENT_CONNECTED);

    case CLIENT_CONNECTED:
    return Transition(CLIENT_CONNECTED, SERVER_LISTENING);

    case SERVER_LISTENING:
    return Transition(SERVER_LISTENING, CONNECTED);

    case CONNECTED:
    return Transition(CONNECTED, DISCONNECTED);

    default: // no action when we are in DISCONNECTED state
    return Transition( _currentState, _currentState);
  }
}

Transition RobustWiFiServer::_getRevertTransition(Transition trans){
  return Transition(trans.to, trans.from);
}

void RobustWiFiServer::_invokeAction(Transition& trans){

  if (trans._invokeAction) {
    if (!trans.withAction()){
      LOGV(RWIFIS, "No action in transition...");
      // do nothing
    }
    // connecting actions...
    else if (Transition(DISCONNECTED, CONNECTED) == trans){
      LOGI(RWIFIS, "Connecting to Wifi...");
      WiFi.mode(WIFI_STA);                  // "station" mode
      // WiFi.disconnect();                    // causes auth error on following connect
      WiFi.config(_ip, _gateway, _subnet);  // set specific ip...
     WiFi.begin(_ssid.c_str(), _wifiPassword.c_str());     // connect to router
    }
    else if (Transition(ERR_SSID_NOT_AVAIL, DISCONNECTED) == trans){
      // nothing to be done
    }
//    else if (Transition(DISCONNECTED, ERR_SSID_NOT_AVAIL) == trans){
//      // nothing to be done
//    }
    else if (Transition(CONNECTED, SERVER_LISTENING) == trans){
      LOGI(RWIFIS, "Starting server...");
      delay(500);
      _server.begin();                      // bind and listen
    }
    else if (Transition(SERVER_LISTENING, CLIENT_CONNECTED) == trans){
      LOGI(RWIFIS, "Listening for clients...");
      _client = _server.available();        // accept - also checked at checkState()
    }   
    else if (Transition(CLIENT_CONNECTED, DATA_AVAILABLE) == trans){
      LOGI(RWIFIS, "Waiting for data...");
    } 

    // disconnecting actions...
    else if (Transition(DATA_AVAILABLE, CLIENT_CONNECTED) == trans){
      LOGI(RWIFIS, "Stop receiving data...");
      // nothing to be done
    }    
    else if (Transition(CLIENT_CONNECTED, SERVER_LISTENING) == trans){
      LOGI(RWIFIS, "Disconnect clients...");
      _client.stop();
    }
    else if (Transition(SERVER_LISTENING, CONNECTED) == trans){
      LOGI(RWIFIS, "Stopping server...");
      _server.end();
    }
    else if (Transition(CONNECTED, DISCONNECTED) == trans){
      LOGI(RWIFIS, "Disconnecting Wifi...");
      WiFi.disconnect();
    }
    else {
      LOGE(RWIFIS, "ERROR. Unknown transition requested: %s", trans.toString().c_str());
    }

    // remember invocation time for timeout
    trans.setLastInvocationTime();
    // invoke action only once
    trans._invokeAction = false;
  }
}

bool RobustWiFiServer::_wasTransitionSuccessful(Transition trans){
  // special checking when wifi connection was lost
  if (Transition(ERR_SSID_NOT_AVAIL, DISCONNECTED) == trans){
   LOGI(RWIFIS, "Scanning SSIDs...");
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
      LOGI(RWIFIS, "%s", WiFi.SSID(i).c_str());
      if (WiFi.SSID(i).equals( _ssid)){
        LOGI(RWIFIS, "specified SSID found.");
        return true;
      }
      return false;
    }
  }
  return _checkState(trans.to);
}

// check whether a state that happen asynchronously
bool RobustWiFiServer::_checkState(ServerState state, bool debug){

  bool stateok = false;
  LOGV(RWIFIS, "wifistate: %s", wiFiStateToString().c_str());
  switch(state){
    
    case DISCONNECTED:
    stateok = (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_NO_SSID_AVAIL);
    break;

    case ERR_SSID_NOT_AVAIL:
    stateok = (WiFi.status() == WL_NO_SSID_AVAIL);
    break;

    case CONNECTED:
    LOGD(RWIFIS, "%s", wiFiStateToString().c_str());
    delay(500);
    stateok = (WiFi.status() == WL_CONNECTED);
    break;

    case SERVER_LISTENING:
    // need to also check wifi status since server would not notice failing wifi
    stateok = (WiFi.status() == WL_CONNECTED) && _server; // equals to is_listening()
    break;

    case CLIENT_CONNECTED:
    if (!_client) {
      _client = _server.available();
    }
    stateok =  (WiFi.status() == WL_CONNECTED) && _client.connected();
    break;

    case DATA_AVAILABLE:
    stateok =  (WiFi.status() == WL_CONNECTED) && _client.available();
    break;

    default: // unknown state
    stateok = false;
    break;
  } 

  if (!stateok && debug) {
    _printInternalState();
  }
  return stateok;
}

void RobustWiFiServer::_printInternalState(){
	LOGD(RWIFIS, "WiFi state: %s", wiFiStateToString().c_str());
  LOGD(RWIFIS, " Server %s %s", 
    _server ? "conn " : "nc ", _server.available() ? "avail" : "na");
  LOGD(RWIFIS, " Client: %s %s", 
    _client.connected() ? "conn " : "nc ", _client.available() ? "avail" : "na");
}

bool RobustWiFiServer::_timeoutReached(){
  uint32_t now = millis();
  return (now - _currentTransition.getLastInvocationTime() > 5000);
}

void RobustWiFiServer::loop(){

  // always verify current state to detect errors
  if (!_checkState(_currentState, true)
      && !_checkState(_currentTransition.to, true)){

    // current state has an error
    _condition.error = STATE_CHECK_FAILED;
    LOGW(RWIFIS, "WARNING. Checking failed for state %s", serverStateToString(_currentState).c_str());

    if (_currentState == DISCONNECTED && WiFi.status() == WL_NO_SSID_AVAIL){
      // for SSID not found we have a special error state
      _currentState = ERR_SSID_NOT_AVAIL;
      _currentTransition = _determineNextTransition();
      LOGI(RWIFIS, "%s", _currentTransition.toString().c_str());
      delay(1000);
    }
    else {
      // switch back one state
      _currentTransition = _determineNextDisconnectTransition();
      LOGI(RWIFIS, "%s", _currentTransition.toString().c_str());
      _currentState = _currentTransition.to;
      delay(200);
    }
  }

  // check whether transition was successful
  else if (_currentTransition.withAction() 
        && _wasTransitionSuccessful(_currentTransition)) {
    _currentState = _currentTransition.to;
    _condition.resetError();
    if (_currentState == _targetState) { 
        // target reached: create no-action transition
        _currentTransition = Transition(_currentState,_currentState);
        LOGI(RWIFIS, "Target reached: %s", serverStateToString(_targetState).c_str());
      }
    else {
      _currentTransition = _determineNextTransition();
    }
    LOGD(RWIFIS, "NEW Transition: %s", _currentTransition.toString().c_str());
  }

  // was in target state (with no action), but now have a different target
  else if (!_currentTransition.withAction() 
         && _currentState != _targetState) {
    _condition.resetError();
    _currentTransition = _determineNextTransition(); 
    LOGD(RWIFIS, "NEW Transition: %s", _currentTransition.toString().c_str());
  }

  // we stayed too long in this state, repeat action
  else if (_currentTransition.withAction() && _timeoutReached()){
    _condition.error = TRANSITION_TIMEOUT_REACHED;
    _condition.numberOfTimeouts++;
    LOGD(RWIFIS, "Timeout reached. Will repeat last action.");
    // we first revert transition to be on the safe side
    // next iteration will automatically repeat action then
    _currentTransition = _getRevertTransition(_currentTransition);
  }

  // some actions are only invoked once
  _invokeAction(_currentTransition);
}
