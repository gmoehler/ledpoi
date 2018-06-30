#include "wifiTask.h"

const String ssid     = "MY_SSID7";
const String password = "my_password";

uint8_t baseIp[] = {
	192, 168, 1, 127};

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

uint16_t basePort = 1110;
const int CONTROL_LED_PIN = 2;

RobustWiFiServer wifiServer;
TaskHandle_t controlLedBlinkTaskHandle = NULL;
bool withinProgram = false;
bool controlLedBlinkActive = false;

// queue to receive wifi control commands
xQueueHandle wifiControlQueue = NULL;

void connect() {
  uint16_t ipIncr = getIpIncrement();
  if (ipIncr == NO_CONNECTION_IPINCR || ipIncr > N_POIS-1 ) {
    LOGE(WIFI_T, "Cannot connect with ip increment of %d.", ipIncr);
    return;
  }
	IPAddress myIP (baseIp[0], baseIp[1], baseIp[2], baseIp[3] + ipIncr);
  LOGD(WIFI_T, "Connecting to %s:%d...", myIP.toString().c_str(), basePort + ipIncr);
	wifiServer.connect(myIP, basePort + ipIncr);
};

void realizeControlCommand(PoiCommand cmd){
	PoiCommandType type = cmd.getType();

    switch(type){

      case CONNECT:
        connect();
      break;
      
      case DISCONNECT:
        wifiServer.disconnect();
      break;
      
      case CLIENT_DISCON:
        wifiServer.clientDisconnect();
      break;

      default:
          LOGW(WIFI_T, "Wifi control command is not implemented: %s", cmd.toString().c_str());
      break;
      
    }
}

void controlLedBlinkTask(void* arg) {
  bool on = false;
  while(controlLedBlinkActive) {

    uint8_t level = on ? HIGH : LOW;
    digitalWrite(CONTROL_LED_PIN, level);
    on = !on;
    delay(500);
  }
  vTaskDelete(NULL);
}

void setControlLedState(ServerState state) {

  static ControlLedState prevControlLedState = CONTROL_LED_OFF;

  switch(state) {
    case UNCONFIGURED:
      if (prevControlLedState != CONTROL_LED_OFF) { // state changed
        controlLedBlinkActive = false;              // end blink task
        prevControlLedState = CONTROL_LED_OFF;      // remember state
        digitalWrite(CONTROL_LED_PIN, LOW);
      }
    break;

    case CONNECTED:
    case SERVER_LISTENING:
      if (prevControlLedState != CONTROL_LED_BLINK) { // state changed
        prevControlLedState = CONTROL_LED_BLINK;      // remember state
        controlLedBlinkActive = true;                 // enable blinking
        // start blink task
        xTaskCreate(controlLedBlinkTask, "controlLedBlinkTask", 1024, NULL, 3, &controlLedBlinkTaskHandle);
      }
    break;

    case CLIENT_CONNECTED:
    case DATA_AVAILABLE:
    if (prevControlLedState != CONTROL_LED_ON) {  // state chanted
        controlLedBlinkActive = false;            // end blink task
        prevControlLedState = CONTROL_LED_ON;     // remember state
        digitalWrite(CONTROL_LED_PIN, HIGH);
      }
     break;

    default:
      // should not happen
    break;
  }
}

void wifiTask(void* arg) {

  wifiServer.init(gateway, subnet, ssid, password, N_POIS, N_PORT_VARS_ON_ERROR);

  RawPoiCommand rawCmd;
  int i = 0;

  for(;;) {
    wifiServer.loop();

    ServerState currentState = wifiServer.getState();
    setControlLedState(currentState);

    if (currentState == DATA_AVAILABLE) {
      char c = wifiServer.readData();       // read one byte
      uint8_t b = static_cast<uint8_t> (c);
      LOGV(WIFI_T, "%d ", b);               
      
      // initial flag 255
      if (b==255){
        i=0;
      }
      else {
        i++;
      }

      // read fields
      if (i>0 && i<N_CMD_FIELDS+1){
        rawCmd.field[i-1] = b;
        // cmd complete
        if (i==N_CMD_FIELDS){
          PoiCommand cmd(rawCmd);
          if (cmd.getType() == SET_PIXEL){  
            sendToDispatch(cmd, WIFI_T, false);
          }
          else if (cmd.getType() != NO_COMMAND){
            sendToDispatch(cmd, WIFI_T);
          }
        }
      }
    }
    
    // no data available: check control queue, no blocking
    else {
      if(xQueueReceive(wifiControlQueue, &( rawCmd ), 0)) {
        PoiCommand cmd(rawCmd);
        
        if (cmd.isWifiControlCommand()){
          LOGD(WIFI_T, "Receiving cmd: %s", cmd.toString().c_str());
          realizeControlCommand(cmd);
        }
        else {
      	  LOGE(WIFI_T, "Error. Non-wifi control command sent to wifi task: %s", cmd.toString().c_str());
        }
      }	

    delay(200); // delay only when no data is available
    } // else
  } // loop
}

void wifi_setup(uint8_t queueSize){
  pinMode(CONTROL_LED_PIN, OUTPUT);
  wifiControlQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
  
}

void wifi_start(uint8_t prio){ 
  xTaskCreate(wifiTask, "wifiTask", 4096, NULL, prio, NULL);
}
