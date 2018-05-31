#ifndef LEDPOI_UTILS_H
#define LEDPOI_UTILS_H

#ifndef WITHIN_UNITTEST
  #include "ws2812.h"
#else
  #include "../test/mock_ws2812.h"
	#include "../test/mock_esp.h"
#endif

#include "ledpoi.h"
#include "PoiCommand.h"

// a frame displayed at the poi
struct PixelFrame {
  uint8_t   idx;
	rgbVal    pixel[ N_PIXELS ];
  uint16_t  delay;
  bool      isLastFrame; // whether this is the last frame of an action - currently not needed
}; 

extern xQueueHandle dispatchQueue;
extern xQueueHandle memoryQueue;                                        
extern xQueueHandle playerQueue;
extern xQueueHandle wifiControlQueue;
extern xQueueHandle programQueue;
extern xQueueHandle displayQueue;

enum QueueType { 
	DISPATCH_QUEUE,
	MEMORY_QUEUE,
	PLAYER_QUEUE,
	WIFI_CONTROL_QUEUE,
	DISPLAY_QUEUE,
	PROGRAM_QUEUE,
};

// some utils
rgbVal makeRGBValue(uint8_t *rgb_array);
rgbVal makeRGBValue(Color color, uint8_t brightness=255);
rgbVal fadeColor(Color color, float factor);
// utils and demo setup
void fillFrame(PixelFrame* pFrame, uint8_t idx, uint16_t delay, uint8_t r, uint8_t g, uint8_t b);
void shiftPixelframe(PixelFrame *pFrame, uint8_t shiftRegisterLength, bool cyclic);

bool sendToQueue(QueueType q, PoiCommand& cmd, const char* logtag, bool debugPrintout=true);
bool sendToDispatch(PoiCommand& cmd, const char* logtag, bool debugPrintout=true);
bool sendRawToDispatch(RawPoiCommand rawCmd, const char* logtag, bool debugPrintout=true);
void sendFrameToDisplay(PixelFrame* pFrame, uint32_t timeout);
void clearQueue(QueueType q);
uint8_t getQueueSize(QueueType q);

#endif