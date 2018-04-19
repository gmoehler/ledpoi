#include "wifiTask.h"

const String ssid     = "MY_SSID";
const String password = "my_password";

uint8_t baseIp[] = {
	192, 168, 1, 127};

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
uint8_t ipIncr = 0;

uint16_t port = 1110;

RobustWiFiServer wifiServer;
bool withinProgram = false;

// queue to receive wifi control commands
xQueueHandle wifiControlQueue = NULL;

void wifiSendCommand(PoiCommand cmd){

  PoiCommandType type = cmd.getType();
  RawPoiCommand rawCmd = cmd.getRawPoiCommand();

  // send command to central dispatch queue
  if (xQueueSendToBack(dispatchQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
    LOGE(WIFI_T, "Could not add command to dispatchQueue: %s", cmd.toString().c_str());
  }
}

void connect() {
	IPAddress myIP (baseIp[0], baseIp[1], baseIp[2], baseIp[3] + ipIncr);
  LOGD(WIFI_T, "Connecting to %s...", myIP.toString().c_str());
	wifiServer.connect(myIP);
};

void realizeControlCommand(PoiCommand cmd){
	PoiCommandType type = cmd.getType();
	uint8_t field1 = cmd.getField(1);

    switch(type){

      case CONNECT:
        ipIncr  = field1;
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

void wifiTask(void* arg) {
  RawPoiCommand rawCmd;
  int i = 0;

  for(;;) {
    wifiServer.loop();

    if (wifiServer.getState() == DATA_AVAILABLE) {
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
          if (cmd.getType() != NO_COMMAND){
            wifiSendCommand(cmd);
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
      delay(500);
      
    } // else
  } // loop
}

void wifi_setup(uint8_t queueSize){
  wifiControlQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
  IPAddress myIP (baseIp[0], baseIp[1], baseIp[2], baseIp[3] + ipIncr);
  wifiServer.init(myIP, gateway, subnet, port, ssid, password);
}

void wifi_start(uint8_t prio){ 
  xTaskCreate(wifiTask, "wifiTask", 4096, NULL, prio, NULL);
}
