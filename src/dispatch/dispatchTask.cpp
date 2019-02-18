#include "dispatchTask.h"

// state kept within dispatcher
bool programTransmitting = false;

xQueueHandle dispatchQueue = NULL;
InteractionState uiState;

void dispatchCommand(PoiCommand cmd){

  // choose correct queue based on cmd type
  if (cmd.isSpiffsControlCommand()) {
      sendToQueue(SPIFFS_QUEUE, cmd, DSPCH_T);
   }
    
  else {
   	LOGE(MEM_T, "Cannot dispatch command: %s", cmd.toString().c_str());
  }
}

void dispatchTask(void* arg) {
  RawPoiCommand rawCmd;
  for(;;) {
    if(xQueueReceive(dispatchQueue, &( rawCmd ), portMAX_DELAY)) {
      PoiCommand cmd(rawCmd);
      PoiCommandType type = cmd.getType();

      if (type == SET_PIXEL) {
        LOGV(DSPCH_T, "Receiving cmd: %s", cmd.toString().c_str());
      } else {
        LOGD(DSPCH_T, "Receiving cmd: %s", cmd.toString().c_str());
      }
      
      if (uiState.commandFilter(cmd)) {
        dispatchCommand(cmd);
      }
    }
  }
}


void dispatch_setup(uint8_t queueSize){
  dispatchQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
}

void dispatch_start(uint8_t prio){ 
  xTaskCreate(dispatchTask, "dispatchTask", 4096, NULL, prio, NULL);
}