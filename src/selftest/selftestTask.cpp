#include "selftestTask.h"

TaskHandle_t selftestTaskHandle = NULL;
bool testResult = true;
rgbVal red = makeRGBVal(254, 0, 0);
rgbVal green = makeRGBVal(0, 254, 0);
uint16_t expectedNumFrames = 0;

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
	expectedNumFrames = 4 + N_FRAMES * 2;
}

void startProgram() {
	sendToDispatch({START_PROG,  0,  0,  0,  0,  0});
}

bool isRgbEqual(rgbVal v1, rgbVal v2) {
	return (v1.r == v2.r && v1.g == v2.g && v1.b == v2.b);
}

void validateSelftest(PixelFrame& rframe) {

	LOGD(SELF_T, "Validating display frame %d", rframe.idx);
	
	// 4 SHOW_RGB commands
	if (rframe.idx<4) {
		rgbVal expectedColor = rframe.idx%2 == 0 ? 
			red : green;

		for (uint8_t i=0; i<N_PIXELS; i++) {
				if (rframe.idx%2 == 0){
					if (!isRgbEqual(rframe.pixel[i], expectedColor)) {
						LOGD(SELF_T, "Wrong color (%d %d %d) at display frame %d",
							rframe.pixel[i].r, rframe.pixel[i].g, rframe.pixel[i].b, rframe.idx);
							testResult = false;
					}
				}
		}
	}
	// image frames
	else {
		uint8_t frameIdx = rframe.idx > N_FRAMES ? rframe.idx - N_FRAMES : rframe.idx;
		rgbVal expectedColor = getTestPixelColor(frameIdx-4);
		uint8_t i = getTestPixelIndex(frameIdx-4);
		if (!isRgbEqual(rframe.pixel[i], expectedColor)) {
			LOGD(SELF_T, "Wrong color (%d %d %d) at display frame %d",
				rframe.pixel[i].r, rframe.pixel[i].g, rframe.pixel[i].b, rframe.idx);
			testResult = false;
		}
		if (rframe.idx >= expectedNumFrames - 1) {
				LOGI(SELF_T, "Self test finished. Result: %d.",testResult);
				LOGI(SELF_T, "Disabling result verification...");
				setSelftestMode(false);
		}
	}
}

static void selftestTask(void* arg){

	// reset result
	testResult = true;

	LOGI(SELF_T, "Sending image...");
	sendImage();
	LOGI(SELF_T, "Sending program...");
	sendProgram();
	LOGI(SELF_T, "Enabling result verification...");
	setSelftestMode(true);
	LOGI(SELF_T, "Starting program...");
	startProgram();
	LOGI(SELF_T, "Program started...");
	// LOGI(SELF_T, "Disabling result verification...");
	// setSelftestMode(false);
  // end task
  vTaskDelete( selftestTaskHandle );
}

void selftest_start(uint8_t prio){
  LOGI(SELF_T, "Starting selftest task");
  xTaskCreate(selftestTask, "selftestTask", 4096, NULL, prio, &selftestTaskHandle);
}
