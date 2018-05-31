#include "PoiMonitor.h"

extern xQueueHandle dispatchQueue;
extern xQueueHandle displayQueue;
extern xQueueHandle memoryQueue;
extern xQueueHandle playerQueue;
extern xQueueHandle programQueue;
extern xQueueHandle wifiControlQueue;

void PoiMonitor::logStatus() {
	uint8_t numDispatchMsg = getQueueSize(DISPATCH_QUEUE);
	uint8_t numDisplayMsg = getQueueSize(DISPLAY_QUEUE);
	uint8_t numMemoryMsg = getQueueSize(MEMORY_QUEUE);
	uint8_t numPlayerMsg = getQueueSize(PLAYER_QUEUE);
	uint8_t numProgramMsg = getQueueSize(PROGRAM_QUEUE);
	uint8_t numWifiControlMsg = getQueueSize(WIFI_CONTROL_QUEUE);
	
	LOGD(DSPCH_T,  "Queues: DIP:%d DIS:%d MEM:%d PLA:%d PRG:%d WIF:%d", 
		numDispatchMsg,
		numDisplayMsg,
		numMemoryMsg,
		numPlayerMsg,
		numProgramMsg,
		numWifiControlMsg);
}