#include "dispatchTask.h"

// state kept within dispatcher
bool programTransmitting = false;

xQueueHandle dispatchQueue = NULL;
InteractionState uiState;

void dispatchCommand(PoiCommand cmd){

  PoiCommandType type = cmd.getType();
  RawPoiCommand rawCmd = cmd.getRawPoiCommand();
  xQueueHandle *targetQueue;

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

      if (type == SET_PIXEL) {
        // less verbose for pixels
        LOGV(DSPCH_T,  "Sending cmd to memory: %s", cmd.toString().c_str());
      } else {
        LOGD(DSPCH_T,  "Sending cmd to memory: %s", cmd.toString().c_str());
      }

      if (xQueueSendToBack(memoryQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
        LOGE(DSPCH_T, "Could not add command to memoryQueue.");
      }
  }
  
  else if (cmd.isProgramControlCommand()) {
      if (programTransmitting) {
        LOGW(DSPCH_T,  "Program control command received while program transmission active.");
      }
      LOGD(DSPCH_T,  "Sending cmd to program: %s", cmd.toString().c_str());
      if (xQueueSendToBack(programQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
        LOGE(DSPCH_T, "Could not add program to programQueue.");
      }
  }
  
  else if (cmd.isWifiControlCommand()) {
  	LOGD(DSPCH_T,  "Sending cmd to wifi: %s", cmd.toString().c_str());
      if (xQueueSendToBack(wifiControlQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
        LOGE(DSPCH_T, "Could not add cmd to wifiControlQueue.");
      }
  }
  
  else if (programTransmitting && cmd.isProgramStatement()) {
      // everything is a program line and goes to the memory
      LOGD(DSPCH_T,  "Sending cmd to memory: %s", cmd.toString().c_str());
      if (xQueueSendToBack(memoryQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
        LOGE(DSPCH_T, "Could not add program line to memoryQueue.");
      }
   } 
   
   else if (!programTransmitting && cmd.isPlayableCommand()) {
      LOGD(DSPCH_T,  "Sending cmd to player: %s", cmd.toString().c_str());
      if (xQueueSendToBack(playerQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
        LOGE(DSPCH_T, "Could not add command to playerQueue.");
      }
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