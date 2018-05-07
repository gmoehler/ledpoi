#include "programTask.h"

xQueueHandle programQueue = NULL;
PoiProgramHandler handler;
uint8_t prioProgramExecTask;
TaskHandle_t programExecTaskHandle = NULL;

// program state 
bool programActive = false;

// the task that actually runs the program thru the handlet
// arg: program line to start program from
void programExecTask(void* arg){
  programActive = true;
  if (arg == NULL){
    handler.init();
  }
  else {
    uint8_t *progLine = (uint8_t*) arg;
    handler.init(*progLine);
  }
  
  // first next() does not advance prg
  while(handler.next()){
		PoiCommand cmd = handler.getCurrentCommand();
		RawPoiCommand rawCmd = cmd.getRawPoiCommand();
	    LOGD(PROG_T,  "Sending cmd to player: %s", cmd.toString().c_str());
         if (xQueueSendToBack(playerQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
           LOGE(PROG_T, "Could not add command to playerQueue.");
        }
  }
  LOGI(PROG_T, "End of program...");
  programActive = false;
  vTaskDelete(NULL);
}

void stopProgramExecTask(){
  if (programActive && programExecTaskHandle != NULL){
    LOGI(PROG_T, "Stopping program...");
    vTaskDelete(programExecTaskHandle);
  }
  else {
    LOGI(PROG_T, "No program is active that could be finished.");
  }
  programActive = false;
  
  LOGD(PROG_T, "Emptying player and display queue");
  xQueueReset(playerQueue);
  xQueueReset(displayQueue);
}

// task that schedules the program
void programTask(void* arg)
{
  RawPoiCommand rawCmd;

  for(;;) {
    // grab next command
    if(xQueueReceive(programQueue, &( rawCmd ), portMAX_DELAY)) {
      PoiCommand cmd(rawCmd);

      LOGD(PROG_T, "Receiving cmd: %s", cmd.toString().c_str());

      // and carry out command
      uint8_t syncPoint;
      uint8_t progLine;
      PoiCommandType type = cmd.getType();
      switch(type){
        case START_PROG:
        if (programActive){
        	// need to stop prev program first
        	stopProgramExecTask();
        }
        LOGI(PROG_T, "Starting program...");
        xTaskCreate(programExecTask, "programExecTask", 4096, NULL, prioProgramExecTask, &programExecTaskHandle);
        break;

        case STOP_PROC:
        stopProgramExecTask();
        break;

        case JUMP2SYNC:
        // get line of sync point, restart program from that line
        // TODO think about alternative implementation:
        // directly move to new line using a handler call
        // (faster? also need to clean play and display buffer?)

        syncPoint = cmd.getField(1);
        progLine = programCache.getLineOfSyncPoint(syncPoint);
        LOGI(PROG_T, "Jumping to sync point %d", syncPoint);

        stopProgramExecTask();

        LOGD(PROG_T, "Starting program at sync point %d / line %d", syncPoint, progLine);
        xTaskCreate(programExecTask, "programExecTask", 4096, (void*) &progLine, prioProgramExecTask, &programExecTaskHandle);
        break;

        default:
        LOGE(PROG_T, "Error unknown command sent to program task: %s", cmd.toString().c_str());
        break;
      }
    }
  }
}

void program_setup(uint8_t queueSize){
  programQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
}

void program_start(uint8_t prio, uint8_t prioPrgExecTask){ 
  prioProgramExecTask = prioPrgExecTask;
  xTaskCreate(programTask, "programTask", 4096, NULL, prio, NULL);
  
}
