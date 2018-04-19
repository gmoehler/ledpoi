
#include "wifi_utils.h"

ServerCondition::ServerCondition(ServerError err){
  error = err;
  numberOfTimeouts = 0;
}

void ServerCondition::resetError(){
  error = NO_ERROR;
  numberOfTimeouts = 0;
}

String wiFiStateToString(){
  
  wl_status_t wifiState = WiFi.status();

  switch(wifiState){
    case WL_NO_SHIELD:
    return String("WL_NO_SHIELD");

    case WL_IDLE_STATUS:
    return String("WL_IDLE_STATUS");
    
    case WL_NO_SSID_AVAIL:
    return String("WL_NO_SSID_AVAIL");

    case WL_SCAN_COMPLETED:
    return String("WL_SCAN_COMPLETED");

    case WL_CONNECTED:
    return String("WL_CONNECTED");

    case WL_CONNECT_FAILED:
    return String("WL_CONNECT_FAILED");

    case WL_CONNECTION_LOST:
    return String("WL_CONNECTION_LOST");

    case WL_DISCONNECTED:
    return String("WL_DISCONNECTED");
  } 
  return String("UNKNOWN WiFi STATE");
}


String serverStateToString(ServerState state){
	switch(state){
    case DISCONNECTED:
      return String("DISCONNECTED");
    case ERR_SSID_NOT_AVAIL:
      return String( "ERR_SSID_NOT_AVAIL");
    case CONNECTED:
      return String("CONNECTED");
    case SERVER_LISTENING:
      return String("SERVER_LISTENING");
    case CLIENT_CONNECTED:
      return String("CLIENT_CONNECTED");
    case DATA_AVAILABLE:
      return String("DATA_AVAILABLE");
  } 
  return String("UNKNOWN_STATE");
}

String Transition::toString(){
  
  if (withAction()) {
    char buffer[40];
    snprintf(buffer, 40, "%s --> %s", 
		  serverStateToString(from).c_str(), serverStateToString(to).c_str());
		return String(buffer);
  }
  else {
    return String("No action");
  }
}
