#include "dispatchTask.h"

// state kept within dispatcher
bool programTransmitting = false;

xQueueHandle dispatchQueue = NULL;
InteractionState uiState;

void dispatchCommand(PoiCommand cmd){

  PoiCommandType type = cmd.getType();

  // choose correct queue based on cmd type
  if (cmd.isMemoryControlCommand()) {
  	if (type == HEAD_PROG){
        LOGD(DSPCH_T,  "Head of program.");
        programTransmitting = true;
      }
      else if (type == TAIL_PROG){
        LOGD(DSPCH_T,  "Tail of program.");
        programTransmitting = false;
      }

      // less verbose for pixels
      sendToQueue(MEMORY_QUEUE, cmd, DSPCH_T, (type != SET_PIXEL));
  }
  
  else if (cmd.isProgramControlCommand()) {
    if (programTransmitting) {
        LOGW(DSPCH_T,  "Program control command received while program transmission active.");
    }
    sendToQueue(PROGRAM_QUEUE, cmd, DSPCH_T);
  }
  
  else if (cmd.isWifiControlCommand()) {
#ifndef DISABLE_WIFI
  	sendToQueue(WIFI_CONTROL_QUEUE, cmd, DSPCH_T);
#endif
  }
  
  else if (programTransmitting && cmd.isProgramStatement()) {
      // everything is a program line and goes to the memory
      sendToQueue(MEMORY_QUEUE, cmd, DSPCH_T);
   } 
   
   else if (!programTransmitting && cmd.isPlayableCommand()) {
      sendToQueue(PLAYER_QUEUE, cmd, DSPCH_T);
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