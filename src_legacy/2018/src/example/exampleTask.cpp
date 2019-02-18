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

void playerExampleStart(){
  RawPoiCommand rawCmd[] = {
    {ANIMATE,     0,  0,  0,  0,  20}, // animate with sample action
    {DISPLAY_IP,  0,  0,  0,  1, 254}, // display ip
    {DISPLAY_IP,  1,  0,  0,  1, 254}, 
    {DISPLAY_IP,  2,  0,  0,  1, 254},
    {DISPLAY_IP,  3,  0,  0,  1, 254}, 
    {DISPLAY_IP,  4,  1,  0, 10, 254}, // display ip with background
    {DISPLAY_IP,  5,  1,  0, 10, 254},
    {DISPLAY_IP,  6,  1,  0, 10, 254}, 
    {DISPLAY_IP,  7,  1,  0, 10, 254}
  };

  uint8_t numCommands = 9;
  sendCommandsTo(dispatchQueue, rawCmd, numCommands);
}

void wormExampleStart(){
  RawPoiCommand rawCmd[] = {
   {ANIMATE,      8,  1,  14, 0, 100}  // animate rainbow worm
  };

  uint8_t numCommands = 1;
  sendCommandsTo(dispatchQueue, rawCmd, numCommands);
}

void setPixelExampleStart1(uint8_t scene){

  // start set_pixels
  RawPoiCommand rawCmd0 =
    {HEAD_SCENE, scene, 0, 0, 0, 0}; // head scene 0
  sendCommandTo(dispatchQueue, rawCmd0);

  RawPoiCommand rawCmd;
  rawCmd.field[2]=0; // (scene)

  for (int f=0; f<2; f++){
    rawCmd.field[1]=f; // frame

    for (int i=0; i<30; i++){
      rawCmd.field[0]=i; // pixel

      if (f == 0){
        rawCmd.field[3]=32;
        rawCmd.field[4]=32;
        rawCmd.field[5]=32;
      }
      else { //f==1
        if (i<10){
          rawCmd.field[3]=32; // r
          rawCmd.field[4]=0;  // g
          rawCmd.field[5]=0;  // b
        }
        else if (i<20){
          rawCmd.field[3]=0;
          rawCmd.field[4]=32;
          rawCmd.field[5]=0;
        }
        else{
          rawCmd.field[3]=0;
          rawCmd.field[4]=0;
          rawCmd.field[5]=32;
        }
      }

      sendCommandTo(dispatchQueue, rawCmd);
    }
  }

  // end of set pixels
  RawPoiCommand rawCmd9 =
    {TAIL_SCENE, 0, 0, 0, 0, 0}; 
  sendCommandTo(dispatchQueue, rawCmd9);
}

void setPixelExampleStart2(uint8_t scene){

  // start set_pixels
  RawPoiCommand rawCmd0 =
    {HEAD_SCENE, scene, 0, 0, 0, 0}; // head scene 2
  sendCommandTo(dispatchQueue, rawCmd0);

  RawPoiCommand rawCmd;
  rawCmd.field[2]=2; // (scene)

  for (int f=0; f<2; f++){
    rawCmd.field[1]=f; // frame

    for (int i=0; i<30; i++){
      rawCmd.field[0]=i; // pixel

      if (f == 0){
        rawCmd.field[3]=32;
        rawCmd.field[4]=0;
        rawCmd.field[5]=0;
      }
      else { 
      	rawCmd.field[3]=0;
        rawCmd.field[4]=0;
        rawCmd.field[5]=32;
      }

      sendCommandTo(dispatchQueue, rawCmd);
    }
  }

  // end of set pixels
  RawPoiCommand rawCmd9 =
    {TAIL_SCENE, 0, 0, 0, 0, 0}; 
  sendCommandTo(dispatchQueue, rawCmd9);
}

void playFramesExampleStart(uint8_t scene){

  RawPoiCommand rawCmd =
    {PLAY_FRAMES,  scene,  1,  2,  4, 254}; // play frames

  sendCommandTo(dispatchQueue, rawCmd);
}

void setPixelSaveAndPlayStart(){
  setPixelExampleStart1(0);
  RawPoiCommand rawCmd =
    {SAVE_SCENE,  0,  0,  0,  0,  0}; // save current Scene (0)
  sendCommandTo(dispatchQueue, rawCmd);
  LOGI(EXPL_T, "Playing scene 0");
  playFramesExampleStart(0);
  delay(1000);

  setPixelExampleStart2(2);
  rawCmd =
    {SAVE_SCENE,  0,  0,  0,  0,  0}; // save current Scene (2)
  sendCommandTo(dispatchQueue, rawCmd);
  LOGI(EXPL_T, "Playing scene 2");
  playFramesExampleStart(2);

  delay(1000); // delay because we need to wait until things are saved
  
  rawCmd =
    {LOAD_SCENE,  0,  0,  0,  0,  0}; // load scene 0
  sendCommandTo(dispatchQueue, rawCmd);
  LOGI(EXPL_T, "Playing scene 0");
  playFramesExampleStart(0);
}

void setProgramStart(){
  setPixelExampleStart1(0);
  RawPoiCommand rawCmd[] = {
    {HEAD_PROG,    0,  0,  0,  0,   0},  // start transmitting a program
    {LOOP_START,   1,  0,  0,  0,   2},  // loop 1 start, 2 loops
    {PLAY_FRAMES,  0,  1,  0,  4, 254},  // play frame 0-1
    {LOOP_END,     1,  0,  0,  0,   0},  // loop 1 end
    {ANIMATE,      8,  2,  14, 0, 100},  // animate worm
    {TAIL_PROG,    0,  0,  0,  0,   0},  // end of program
    {SAVE_PROG,    0,  0,  0,  0,   0}  // save prog to flash
  };

  uint8_t numCommands = 7;
  sendCommandsTo(dispatchQueue, rawCmd, numCommands);

  RawPoiCommand rawCmd1 =
    {START_PROG,  0,  0,  0,  0,  0}; // run pro
  sendCommandTo(dispatchQueue, rawCmd1);
  LOGI(EXPL_T, "Running program");
}

static void exampleTask(void* arg){

  switch (exampleToRun){

    case DISPLAY_EXAMPLE:
    LOGI(EXPL_T, "Starting display example");
    displayExampleStart();
    break;

    case PLAYER_EXAMPLE:
    LOGI(EXPL_T, "Starting player example");
    playerExampleStart();
    break;

    case SET_PIXEL_AND_PLAY_EXAMPLE:
    LOGI(EXPL_T, "Starting set pixel and play example");
    setPixelExampleStart1(0);
    playFramesExampleStart(0);
    break;

    case SET_PIXEL_SAVE_AND_PLAY_EXAMPLE:
    LOGI(EXPL_T, "Starting set pixel, save and play example");
    setPixelSaveAndPlayStart();
    break;

    case SET_PROGRAM_EXAMPLE:
    LOGI(EXPL_T, "Sending a program example");
    setProgramStart();
    break;

    case WORM_EXAMPLE:
    LOGI(EXPL_T, "Starting player example");
    wormExampleStart();
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
