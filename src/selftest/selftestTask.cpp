#include "selftestTask.h"

TaskHandle_t selftestTaskHandle = NULL;
SelftestHelper stHelper;
bool _selftestActive = false;
bool _selftestFailed = false;
uint16_t numFramesChecked = 0;

volatile SemaphoreHandle_t _endofTestSemaphore;

bool isSelftestActive() {
	return _selftestActive;
}

void selftestValidateFrame(PixelFrame& rframe) {
	if (!_selftestActive) {
		LOGE(SELF_T, "Cannot validate frame when selftest is inactive" );
		return;
	}
	bool success = stHelper.validateFrame(rframe, numFramesChecked);
	if (success) {
		numFramesChecked++;
		if (numFramesChecked >= stHelper.getExpectedNumFrames()) {
			// last frame
			_selftestActive = false;
		}
	}
	else {
		_selftestFailed = true;
		_selftestActive = false;
 	}
 	
	if (!_selftestActive) {
		// selftest finished
		xSemaphoreGive( _endofTestSemaphore );
	}
}

static void selftestTask(void* arg){

	_selftestActive = true;
	_selftestFailed = false;
	numFramesChecked = 0;
	_endofTestSemaphore = xSemaphoreCreateBinary();

	LOGI(SELF_T, "Sending image...");
	stHelper.sendImage();
	LOGI(SELF_T, "Sending program...");
	stHelper.sendProgram();
	LOGI(SELF_T, "Starting program...");
	stHelper.startProgram();
	LOGI(SELF_T, "Program started...");

	// wait for success, error or timeout (1.2 times the expected runtime)
	uint32_t timeoutMs =  12 * stHelper.getExpectedRuntime() / 10;
	const TickType_t xDelay = timeoutMs / portTICK_PERIOD_MS;
	if (!xSemaphoreTake(_endofTestSemaphore, xDelay)) {
		// timeout reached
		LOGE(SELF_T, "Selftest timeout (%d) reached at frame %d.", timeoutMs, numFramesChecked);
		_selftestFailed = true;
	}

	// finish processing (mainly required for failed test)
	sendRawToDispatch({STOP_PROC, 0, 0, 0, 0, 0}, SELF_T);
	
	// communicate test result
	if (_selftestFailed) {
		LOGE(SELF_T, "Selftest FAILED at frame %d (out of %d frames).", numFramesChecked, stHelper.getExpectedNumFrames());
		delay(200);
		sendRawToDispatch( {ANIMATE, RED, 1, 30, 0, 50}, INTS ); 
	}
	else {
		LOGI(SELF_T, "Selftest SUCCEEDED (%d frames validated).", numFramesChecked);
		delay(200);
		sendRawToDispatch( {ANIMATE, GREEN, 1, 30, 0, 50}, INTS );
	}

	_selftestActive = false;
	vTaskDelete( selftestTaskHandle );
}

void selftest_start(uint8_t prio){
  LOGI(SELF_T, "Starting selftest task");
  xTaskCreate(selftestTask, "selftestTask", 4096, NULL, prio, &selftestTaskHandle);
}
