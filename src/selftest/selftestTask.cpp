#include "selftestTask.h"

TaskHandle_t selftestTaskHandle = NULL;

void sendToDispatch( RawPoiCommand rawCmd ) {

  PoiCommand cmd(rawCmd);
  LOGD(SELF_T, "Sending to dispatch:  %s", cmd.toString().c_str());
  if (xQueueSendToBack(dispatchQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
    LOGE(SELF_T, "Could not add  command to dispatch queue.");
  }
}

// generate a color for a pixel
rgbVal getTestPixelColor(uint8_t i) {
	return makeRGBVal(i,0,254-i);
}

// generate an index for a pixel
uint8_t getTestPixelIndex(uint8_t i) {
	return i % N_PIXELS;
}

/// generate an image with max size
void sendImage() {
  sendToDispatch({HEAD_SCENE,  N_SCENES-1,  0,  0,  0,  0});
	
	for (uint8_t i=0; i<N_FRAMES; i++) {
		rgbVal c = getTestPixelColor(i);
		uint8_t idx = getTestPixelIndex(i);
		sendToDispatch({idx,  i,  0,  c.r,  c.g,  c.b});
	}
	
	sendToDispatch({TAIL_SCENE,  0,  0,  0,  0,  0});
	sendToDispatch({SAVE_SCENE,  0,  0,  0,  0,  0});
}

void sendProgram() {
	
	uint8_t d0 = 10;  // short delay
	uint8_t d1 = 250; // long delay
	sendToDispatch({HEAD_PROG,  0,  0,  0,  0,  0});
	
	// scene 0 first to wipe out mem
	sendToDispatch({LOAD_SCENE,  0,  0,  0,  0,  0});
	sendToDispatch({LOAD_SCENE,  N_SCENES-1,  0,  0,  0,  0});
	
	// twice red-green
	sendToDispatch({LOOP_START,  42,  0,  0,  0,  2});
	sendToDispatch({SHOW_RGB, 254,  0,  0,  0,  d1});
	sendToDispatch({SHOW_RGB,  0, 254,  0,  0,  d1});
	sendToDispatch({LOOP_END, 42,  0,  0,  0,  0});
	
	// twice complete image
	sendToDispatch({PLAY_FRAMES,  0,  N_FRAMES-1,  2,  0,  d0});
	
	sendToDispatch({TAIL_PROG,  0,  0,  0,  0,  0});
	sendToDispatch({SAVE_PROG,  0,  0,  0,  0,  0});
}

void startProgram() {
	sendToDispatch({START_PROG,  0,  0,  0,  0,  0});
}

static void selftestTask(void* arg){

	LOGI(SELF_T, "Sending image...");
	sendImage();
	LOGI(SELF_T, "Sending program...");
	sendProgram();
	LOGI(SELF_T, "Starting program...");
	startProgram();
	LOGI(SELF_T, "Program started...");

  // end task
  vTaskDelete( selftestTaskHandle );
}

void selftest_start(uint8_t prio){
  LOGI(SELF_T, "Starting selftest task");
  xTaskCreate(selftestTask, "selftestTask", 4096, NULL, prio, &selftestTaskHandle);
}
