#include "displayTask.h"

xQueueHandle displayQueue = NULL;
PoiTimer ptimer;
uint16_t currentDelay=1000;
bool doPause = false;
bool skipFrames = false;

volatile SemaphoreHandle_t displayTimerSemaphore;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

String pixelFrameToString(PixelFrame f, int idx, bool withHeader){
  char buffer[40];
  if (withHeader) {
  snprintf(buffer, 40, "Frame %d (delay %d): %3d %3d %3d", 
    f.idx, f.delay, 
    f.pixel[idx].r, f.pixel[idx].g, f.pixel[idx].b);
  }
  else {
  snprintf(buffer, 40, "%3d %3d %3d", 
    f.pixel[idx].r, f.pixel[idx].g, f.pixel[idx].b);    
  }
  return String(buffer);
}

void IRAM_ATTR onDisplayTimer(){

  //portENTER_CRITICAL_ISR(&timerMux);
  // nothing critical for now
  //portEXIT_CRITICAL_ISR(&timerMux);

  // Give a semaphore that triggers the displayTask
  xSemaphoreGiveFromISR(displayTimerSemaphore, NULL);
}

static void displayTask(void* arg)
{
  struct PixelFrame rframe;

  for(;;) {
    // wait for timer
    if (xSemaphoreTake(displayTimerSemaphore, portMAX_DELAY)) {
      LOGV(DISP_T, "After semaphore - before queue - delay: %d", currentDelay);
      // grab next frame
      if(xQueueReceive(displayQueue, &( rframe ), portMAX_DELAY)) {
        if (skipFrames) {
          LOGD(DISP_T, "Skipping display frame...");
        }
        else {
          // change delay asap to make interval as exact as possible
          // need to make sure that next semaphore is not given before this step
          if (rframe.delay != currentDelay){
            ptimer.setInterval(rframe.delay);
            currentDelay = rframe.delay;
          }
          int idx=0;
          LOGD(DISP_T, "Received: %s %s %s ...", 
            pixelFrameToString(rframe, 0, true).c_str(),
            pixelFrameToString(rframe, 1, false).c_str(),
            pixelFrameToString(rframe, 2, false).c_str());
          ws2812_setColors(N_PIXELS, rframe.pixel);
        }
      }
    }
  }
}

void display_setup(uint8_t queueSize){
  if(ws2812_init(WS2812_PIN, LED_WS2812B)){
    LOGE(DISP_T, "LED Pixel init error.");
  }

  displayQueue = xQueueCreate(queueSize, sizeof( PixelFrame ));
  displayTimerSemaphore = xSemaphoreCreateBinary();
  ptimer.init(onDisplayTimer);
}

void display_start(uint8_t prio){ 
  doPause = false;
  doAction = true;
  xTaskCreate(displayTask, "displayTask", 4096, NULL, prio, NULL);
  ptimer.setInterval(currentDelay);
}

void display_pause(){ 
  LOGI(DISP_T, "Display pausing...");
  ptimer.disable();
  doPause = true;
}

void display_skipFrames(){ 
  LOGI(DISP_T, "Display stopped...");
  skipFrames = true;
}

void display_resume(){ 
  LOGI(DISP_T, "Display resuming...");
  ptimer.enable();
  doPause = false;
  skipFrames = false;
}

bool display_isPaused() {
  return doPause;
}