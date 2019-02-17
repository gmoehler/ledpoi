#include "PoiMonitor.h"

extern xQueueHandle dispatchQueue;
extern xQueueHandle displayQueue;
extern xQueueHandle playerQueue;

void PoiMonitor::logStatus() {
	uint8_t numDispatchMsg = getQueueSize(DISPATCH_QUEUE);
	uint8_t numDisplayMsg = getQueueSize(DISPLAY_QUEUE);
	
	LOGI(DSPCH_T,  "Queues: DIP:%d DIS:%d MEM:%d PLA:%d PRG:%d WIF:%d", 
		numDispatchMsg,
		numDisplayMsg);
}

bool PoiMonitor::isDisplayActive() {
	uint8_t numDisplayMsg = getQueueSize(DISPLAY_QUEUE);
	return numDisplayMsg > 8;
}
