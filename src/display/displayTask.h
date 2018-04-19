/******************************************************************
 * 
 * Task that drives the ws2812 led chain
 * 
 ******************************************************************/

#ifndef DISPLAY_TASK
#define DISPLAY_TASK

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiTimer.h"

extern xQueueHandle displayQueue;

String pixelFrameToString(PixelFrame frame, int idx, bool withHeader=true);

void display_setup(uint8_t queueSize); // setup display task
void display_start(uint8_t prio);      // start display task and timer

#endif