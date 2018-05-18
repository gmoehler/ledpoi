#include "programTask.h"

// program task will run start a program exec task to run the program

xQueueHandle programQueue = NULL;
PoiProgramHandler handler;
uint8_t prioProgramExecTask;
TaskHandle_t programExecTaskHandle = NULL;
PoiMonitor monitor;

// program state 
bool programActive = false;

// the task that actually runs the program thru the handler
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
  // stop program when programActive is set to false
  while(programActive && handler.next()){
		PoiCommand cmd = handler.getCurrentCommand();
		RawPoiCommand rawCmd = cmd.getRawPoiCommand();
	    LOGD(PROG_T,  "Sending cmd to player: %s", cmd.toString().c_str());
         if (xQueueSendToBack(playerQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
           LOGE(PROG_T, "Could not add command to playerQueue.");
        }
  }
  if (programActive) {
    LOGI(PROG_T, "End of program.");
  } 
  else {
    LOGI(PROG_T, "Program stopped.");
  }

  vTaskDelete(NULL);
  programActive = false;
}

void stopProgramExecTask(){
  if (programActive){
    LOGI(PROG_T, "Stopping program...");
    programActive = false;
  }
  else {
    LOGI(PROG_T, "No program is active that could be finished.");
  }
  monitor.logStatus();

  LOGD(PROG_T, "Skipping all remaining display frames");
  display_skipFrames();
  
  LOGD(PROG_T, "Skipping all remaining player actions");
  player_skipActions();
  
  LOGD(PROG_T, "Emptying player queue");
  xQueueReset(playerQueue);

  LOGD(PROG_T, "Emptying display queue");
  xQueueReset(displayQueue);

  delay(100); // just a bit to allow stopping player
  LOGD(PROG_T, "Resuming display and player");
  monitor.logStatus();
  display_resume();
  player_resume();
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
        display_resume();
        xTaskCreate(programExecTask, "programExecTask", 4096, NULL, prioProgramExecTask, &programExecTaskHandle);
        break;

        case STOP_PROC:
        stopProgramExecTask();
        break;

        case PAUSE_PROC:
        if (display_isPaused()) {
          display_resume();
        }
        else {
          display_pause();
        }
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
