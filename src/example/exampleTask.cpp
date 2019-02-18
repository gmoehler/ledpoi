#include "exampleTask.h"

TaskHandle_t exampleTaskHandle = NULL;
Example exampleToRun = DISPLAY_EXAMPLE;

const uint8_t DEMO_BRIGHTNESS = 32; // Limits brightness

void sendCommandsTo(xQueueHandle queue, RawPoiCommand rawCmd[], uint8_t numCmds ) {

  for (int i=0; i<numCmds; i++){
    PoiCommand cmd(rawCmd[i]);
    LOGD(EXPL_T, "Sending cmd:   %s", cmd.toString().c_str());
    if (xQueueSendToBack(queue, &(rawCmd[i]),  portMAX_DELAY) != pdTRUE){
       LOGE(EXPL_T, "Could not add command %d to playerQueue.", i);
    }
  }
}

void sendCommandTo(xQueueHandle queue, RawPoiCommand rawCmd ) {

  PoiCommand cmd(rawCmd);
  LOGD(EXPL_T, "Sending cmd:   %s", cmd.toString().c_str());
  if (xQueueSendToBack(queue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
    LOGE(EXPL_T, "Could not add single command to playerQueue.");
  }
}

void frameToDisplayQueue(PixelFrame* pFrame, uint32_t timeout) {
  if (xQueueSendToBack(displayQueue, pFrame, timeout/portTICK_PERIOD_MS) != pdTRUE){
    LOGE(PLAY_T, "Could not add frame %d to queue.", pFrame->idx);
  }
}

static void displayExampleStart(){
    PixelFrame sframe;
      // send frames to the play queue
    fillFrame(&sframe, 0, 1000, DEMO_BRIGHTNESS, 0, 0);
    LOGD(EXPL_T, "%s", pixelFrameToString(sframe, 0).c_str());
    frameToDisplayQueue(&sframe, portMAX_DELAY);

    fillFrame(&sframe, 1, 3000, 0, DEMO_BRIGHTNESS, 0);
    LOGD(EXPL_T, "%s", pixelFrameToString(sframe, 0).c_str());
    frameToDisplayQueue(&sframe, portMAX_DELAY);

    fillFrame(&sframe, 2, 3000, 0, 0, DEMO_BRIGHTNESS);
    LOGD(EXPL_T, "%s", pixelFrameToString(sframe, 0).c_str());
    frameToDisplayQueue(&sframe, portMAX_DELAY);

    fillFrame(&sframe, 3, 3000, DEMO_BRIGHTNESS, 0, DEMO_BRIGHTNESS);
    LOGD(EXPL_T, "%s", pixelFrameToString(sframe, 0).c_str());
    frameToDisplayQueue(&sframe, portMAX_DELAY);
}

static void exampleTask(void* arg){

  switch (exampleToRun){

    case DISPLAY_EXAMPLE:
    LOGI(EXPL_T, "Starting display example");
    displayExampleStart();
    break;

    default:
    LOGE(EXPL_T, "No example to start");
    break;
  }

  // just filling queues then end
  vTaskDelete( exampleTaskHandle );
}

void example_start(Example example, uint8_t prio){
  LOGI(EXPL_T, "Starting example");
  exampleToRun = example;
  xTaskCreate(exampleTask, "exampleTask", 4096, NULL, prio, &exampleTaskHandle);
}
