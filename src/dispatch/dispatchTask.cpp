#include "dispatchTask.h"

// state kept within dispatcher
bool programTransmitting = false;

xQueueHandle dispatchQueue = NULL;
InteractionState uiState;
PoiMonitor monitor;

void stopProcessing(){
  // stopping the processing using skip flags,
  // a cleaner approch would be running the action
  // in a task and stop that task
  monitor.logStatus();

  LOGD(PROG_T, "Skipping all remaining display frames");
  display_skipFrames();
  
  LOGD(PROG_T, "Skipping all remaining player actions");
  player_skipActions();
  
  LOGD(PROG_T, "Emptying player queue");
  clearQueue(PLAYER_QUEUE);

  LOGD(PROG_T, "Emptying display queue");
  clearQueue(DISPLAY_QUEUE);

  delay(100); // just a bit to allow stopping player
  LOGD(PROG_T, "Resuming display and player");
  monitor.logStatus();
  display_resume();
  player_resume();

  sendRawToDispatch( {STOP_PROC_COMPLETED, 0, 0, 0, 0, 0}, PROG_T );
}

void dispatchCommand(PoiCommand cmd){

  // choose correct queue based on cmd type
  if (cmd.isPlayableCommand()) {
      sendToQueue(PLAYER_QUEUE, cmd, DSPCH_T);
  }  
  else if (cmd.getType() == STOP_PROC) {
      stopProcessing();
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