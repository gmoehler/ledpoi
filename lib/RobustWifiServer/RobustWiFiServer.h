#ifndef WIFI_HANDLER
#define WIFI_HANDLER

/**
 * Robust server
 * 
 * Can handle the following wifi situations:
 * 1. client connects
 * 2. client sends data
 * 3. client disconnects when idle
 * 4. client is interrupted during transmission
 * 5. wifi stops and is restarted when no client is connected
 * 6. wifi stops and is restarted when client is connected 
 * 7. wifi stops during client transmission 
 * 
 **/

#include <utility>
#ifndef WITHIN_UNITTEST
  #include "WiFiServer2.h"
#else
  #include "../test/mock_Arduino.h"
  #include "../test/mock_wifi.h"
#endif

#include "wifi_utils.h"

#define TRANSITION_TIMEOUT 10000
#define TRANSITION_TIMEOUT_SMALL 3000

class RobustWiFiServer
{
public:
  RobustWiFiServer();
  void init(IPAddress gateway, IPAddress subnet, 
    String ssid, String wifiPassword, 
    uint8_t serverPortIncrOnError, uint8_t numPortVarOnError); // call once

  void connect();
  // connect to a different ip (only when being disconnected)
  void connect(IPAddress ip, uint16_t port);
  void disconnect();
  
  void clientDisconnect();

  ServerState getState();
  ServerCondition getCondition();

  char readData();  // read data (in state DATA_AVAILABLE)
  size_t writeData(uint8_t data);                     // write (in state CLIENT_CONNECTED)
  size_t writeData(const uint8_t *buf, size_t size);  // write (in state CLIENT_CONNECTED)
 
  void loop();      // to be called in loop()

private:
  Transition _currentTransition; // current state transition
  ServerState _currentState; // current state
  ServerState _targetState;  // first target
  ServerState _targetState2; // second target
  ServerCondition _condition;
  bool _targetUpdated;
  int _lastDataAvailableCount;

  IPAddress _ip; 
  IPAddress _gateway; 
  IPAddress _subnet; 
  uint16_t _serverPort;
  uint16_t _initialServerPort;
  uint8_t _serverPortVarsOnError;
  uint8_t _numPortVarsOnError;
  String _ssid; 
  String _wifiPassword;

  WiFiServer2 _server;
  WiFiClient _client;
  
  Transition _determineNextTransition();
  Transition _getStepBackTransition();
  
  void _invokeAction(Transition& trans);
  bool _wasTransitionSuccessful(Transition trans);
  bool _checkState(ServerState state, bool debug=false);
  bool _timeoutReached();

#ifdef WITHIN_UNITTEST  
public:  
#endif
  WiFiClient& _getClient(){return _client;}
  WiFiServer2& _getServer(){return _server;}
  
  void _printInternalState();
};

#endif