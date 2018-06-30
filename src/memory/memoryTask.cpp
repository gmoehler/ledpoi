#include "memoryTask.h"

ImageCache imageCache(N_PIXELS, N_FRAMES, 
  N_NUM_FLASH_SECTIONS_PER_IMAGE * SPI_FLASH_SEC_SIZE, 0);
ProgramCache programCache;
PoiFlashMemory flashMemory;
uint8_t currentScene = 0;
uint8_t ipIncr = 0;

xQueueHandle memoryQueue = NULL;

void _setPixel(PoiCommand cmd) {
  uint8_t pixel_idx = cmd.getField(0);
  uint8_t frame_idx = cmd.getField(1);
  // uint8_t scene     = cmd.getField(2);        
  uint8_t r = cmd.getField(3);
  uint8_t g = cmd.getField(4);
  uint8_t b = cmd.getField(5);
  imageCache.setPixel(frame_idx, pixel_idx, r, g, b);
}

// save scene from memory to flash
void _saveSceneToFlash(){
  if (flashMemory.saveImage(currentScene, imageCache.getRawImageData())){
    LOGI(MEM_T, "Image of scene %d saved to flash.", currentScene);
  }
  else {
    LOGE(MEM_T, "Error saving scene %d to flash.", currentScene);
  }
}

// load scene from flash into memory (used synchronously in player task)
void loadSceneFromFlash(uint8_t scene){
  if (flashMemory.loadImage(scene, imageCache.getRawImageData())){
    currentScene = scene;
   LOGI(MEM_T,  "Scene %d loaded from flash.", currentScene);
  }
  else{
    LOGE(MEM_T, "Error. Cannot load scene %d", scene);
  }
}

// load ip increment from flash into memory 
void _loadIpIncrFromFlash(){
  if (flashMemory.loadIpIncrement(&ipIncr)) {
    LOGI(MEM_T,  "IP incr %d loaded from flash.", ipIncr);
    if (ipIncr == NO_CONNECTION_IPINCR) {
      LOGI(MEM_T,  "IP incr means no connection.");
    }
  }
  else{
    LOGE(MEM_T, "Error. Cannot load IP incr");
  }
}


uint8_t getIpIncrement() {
  return ipIncr;
}

// save program from memory to flash
void _saveProgToFlash(){
  if (flashMemory.saveProgram(programCache.getRawProgramDataPrt())){
    uint8_t numProgSteps = programCache.getNumProgLines();
	  if (flashMemory.saveNumProgramSteps(numProgSteps)){
			LOGD(MEM_T, "Program saved to flash.");
		}
		else {
			LOGE(MEM_T, "Error saving number of program steps to flash.");
		}
	}
	else {
			LOGE(MEM_T, "Error saving program to flash.");
	}
}

void _loadProgFromFlash(){
  bool loadingSuccess = false;
	uint16_t numProgLines = 0;
  if (flashMemory.loadNumProgramSteps(&numProgLines)){
		if (numProgLines > 0){
			if (flashMemory.loadProgram(programCache.getRawProgramDataPrt())){
        programCache.parseProgram(numProgLines);
        loadingSuccess = true;

        LOGI(MEM_T, "Program loaded from flash.");
        LOGI(MEM_T, "Program read (%d lines, %d labels, %d sync points):", 
          programCache.getNumProgLines(),  
          programCache.getNumLabels(),
          programCache.getNumSyncPoints() );
        programCache.printProgram();
			}
		}
		else {
      loadingSuccess = true;
      LOGW(MEM_T, "No program on flash.");
		}
	}

  if (!loadingSuccess){
		LOGE(MEM_T, "Error loading program from flash.");
		programCache.clearProgram();
	}
}

void memoryTask(void* arg)
{
  RawPoiCommand rawCmd;
  // uint8_t scene = 0;
  bool duringPrograming = false;

  for(;;) {
    // grab next command
    if(xQueueReceive(memoryQueue, &( rawCmd ), portMAX_DELAY)) {
      PoiCommand cmd(rawCmd);

      if (cmd.getType() == SET_PIXEL) {
        LOGV(MEM_T, "Receiving cmd: %s", cmd.toString().c_str());
      }
      else {
        LOGD(MEM_T, "Receiving cmd: %s", cmd.toString().c_str());
      }
      
      if (duringPrograming && cmd.isProgramStatement()) {
        // add program commands to program cache
        programCache.addCommand(cmd);
      }
      else if (!cmd.isMemoryControlCommand()) {
      	LOGE(MEM_T, "Error. Non-memory command sent to memory task: %s", cmd.toString().c_str());
      }
      else {
        // carry out control commands
        PoiCommandType type = cmd.getType();
        uint8_t field;
        switch(type){
          case HEAD_SCENE:
          currentScene = cmd.getField(1);
          imageCache.clearImageMap();
          LOGI(MEM_T, "Start writing image %d to memory...", currentScene);
          break;

          case TAIL_SCENE:
          LOGI(MEM_T, "Stop writing image %d to memory.", currentScene);
          LOGV(MEM_T, "First 2 frames:");
          imageCache.printImageFrame(0, ESP_LOG_VERBOSE);
          imageCache.printImageFrame(1, ESP_LOG_VERBOSE);
          break;

          case SAVE_SCENE:
          LOGI(MEM_T, "Saving image of scene %d to flash.", currentScene);
          _saveSceneToFlash();
          break;

          case SET_PIXEL:
          _setPixel(cmd);
          break;

          case HEAD_PROG:
          LOGI(MEM_T, "Start writing program to memory...");
          programCache.clearProgram();
          duringPrograming = true;
          break;

          case TAIL_PROG:
          LOGI(MEM_T, "Stop writing program to memory.");
          duringPrograming = false;
          LOGD(MEM_T, "Program (%d lines, %d lables, %d syncpoints):", 
            programCache.getNumProgLines(), programCache.getNumLabels(), programCache.getNumSyncPoints());
          programCache.printProgram();
          break;

          case SAVE_PROG:
          LOGI(MEM_T, "Saving program to flash.");
          _saveProgToFlash();
          break;

          case INIT_FLASH:
          LOGI(MEM_T, "Initializing flash...");
          imageCache.clearImageMap();
          flashMemory.initializeFlash(imageCache.getRawImageData());
          break;     
          
          case SET_IP:
          field = cmd.getField(1);
          if (field != ipIncr) {
            ipIncr = field;
            flashMemory.saveIpIncrement(ipIncr);
            LOGI(MEM_T, "Start writing IP increment %d to memory...", ipIncr);
          }
          break;

          default:
          LOGE(MEM_T, "Memory control command is not implemented: %s", cmd.toString().c_str());
          break;
        }
      }
    }
  }
}

void memory_setup(uint8_t queueSize){
  // setup memory & flash
  imageCache.clearImageMap();
  flashMemory.setup(imageCache.getRawImageData());
  _loadIpIncrFromFlash();
  loadSceneFromFlash(0);
  _loadProgFromFlash();
  // setup queue
  memoryQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
}

void memory_start(uint8_t prio){ 
  xTaskCreate(memoryTask, "memoryTask", 4096, NULL, prio, NULL);
}
