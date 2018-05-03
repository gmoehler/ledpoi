#include "PoiMonitor.h"

extern xQueueHandle dispatchQueue;
extern xQueueHandle displayQueue;
extern xQueueHandle memoryQueue;
extern xQueueHandle playerQueue;
extern xQueueHandle programQueue;
extern xQueueHandle wifiControlQueue;

PoiMonitor::logStatus() {
	uint8_t numDispatchMsg = dispatchQueue.uxQueueMessagesWaiting();
	uint8_t numDisplayMsg = displayQueue.uxQueueMessagesWaiting();
	uint8_t numMemoryMsg = memoryQueue.uxQueueMessagesWaiting();
	uint8_t numPlayerMsg = playerQueue.uxQueueMessagesWaiting();
	uint8_t numProgramMsg = programQueue.uxQueueMessagesWaiting();
	uint8_t numWifiControlMsg = wifiControlQueue.uxQueueMessagesWaiting();
	
	LOGD(DSPCH_T,  "Queues: DIP:%d DIS:%d MEM:%d PLA:%d PRG:%d WIF:%d", 
		numDispatchMsg,
		numDisplayMsg,
		numMemoryMsg,
		numPlayerMsg,
		numProgramMsg,
		numWifiControlMsg);

}