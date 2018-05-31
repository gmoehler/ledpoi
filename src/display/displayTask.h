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
#include "selftest/selftestTask.h"

extern xQueueHandle displayQueue; // for reading only

String pixelFrameToString(PixelFrame frame, int idx, bool withHeader=true);

void display_setup(uint8_t queueSize);  // setup display task
void display_start(uint8_t prio);       // start display task and timer
void display_pause();				    // pause the display action
void display_resume();			        // resume the display action
bool display_isPaused();			    // is display in pause
void display_skipFrames();              // skip all frames until display is resumed
void setSelftestMode(bool active);      // allow verification of selftest results

#endif