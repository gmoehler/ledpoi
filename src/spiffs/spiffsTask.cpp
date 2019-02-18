#include "spiffsTask.h"

SpiffsUtils spiffsUtils;

xQueueHandle spiffsQueue = NULL;
PixelFrame imageFrame;

void _loadChannelFromSpiffs() {
  spiffsUtils.openFile("/channel.poi");
  int numFrames=0;
  imageFrame.delay = 10; // in ms
  while(spiffsUtils.getNextFrame(&imageFrame)) {
    numFrames++;
    sendFrameToDisplay(&imageFrame, portMAX_DELAY);
  }

  LOGI(MEM_T,  "Channel with %d frames loaded from spiffs.", numFrames);
}

void spiffsTask(void* arg)
{
  RawPoiCommand rawCmd;

  for(;;) {
    // grab next command
    if(xQueueReceive(spiffsQueue, &( rawCmd ), portMAX_DELAY)) {
      PoiCommand cmd(rawCmd);

      if (!cmd.isSpiffsControlCommand()) {
          LOGE(MEM_T, "Error. Non-spiffs command sent to spiffs task: %s", cmd.toString().c_str());
      }
      else {
        // carry out control commands
        PoiCommandType type = cmd.getType();
        switch(type){
          case LOAD_CHAN:
          LOGI(SPIF_T, "Loading channel from SPIFFS");
          _loadChannelFromSpiffs();
          break;

          default:
          LOGE(SPIF_T, "Spiffs control command is not implemented: %s", cmd.toString().c_str());
          break;
        }
      }
    }
  }
}

void spiffs_setup(uint8_t queueSize){
  // setup spiffs
  spiffsUtils.setup();
  // setup queue
  spiffsQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
}

void spiffs_start(uint8_t prio){ 
  xTaskCreate(spiffsTask, "spiffsTask", 4096, NULL, prio, NULL);
}
