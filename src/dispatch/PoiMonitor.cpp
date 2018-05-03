#include "PoiMonitor.h"

extern xQueueHandle dispatchQueue;
extern xQueueHandle displayQueue;
extern xQueueHandle memoryQueue;
extern xQueueHandle playerQueue;
extern xQueueHandle programQueue;
extern xQueueHandle wifiControlQueue;

void PoiMonitor::logStatus() {
	uint8_t numDispatchMsg = uxQueueMessagesWaiting(dispatchQueue);
	uint8_t numDisplayMsg = uxQueueMessagesWaiting(displayQueue);
	uint8_t numMemoryMsg = uxQueueMessagesWaiting(memoryQueue);
	uint8_t numPlayerMsg = uxQueueMessagesWaiting(playerQueue);
	uint8_t numProgramMsg = uxQueueMessagesWaiting(programQueue);
	uint8_t numWifiControlMsg = uxQueueMessagesWaiting(wifiControlQueue);
	
	LOGD(DSPCH_T,  "Queues: DIP:%d DIS:%d MEM:%d PLA:%d PRG:%d WIF:%d", 
		numDispatchMsg,
		numDisplayMsg,
		numMemoryMsg,
		numPlayerMsg,
		numProgramMsg,
		numWifiControlMsg);
}