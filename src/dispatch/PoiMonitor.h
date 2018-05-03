#ifndef POI_MONITOR
#define POI_MONITOR

extern xQueueHandle dispatchQueue;
extern xQueueHandle displayQueue;
extern xQueueHandle memoryQueue;
extern xQueueHandle playerQueue;
extern xQueueHandle programQueue;
extern xQueueHandle wifiControlQueue;

class PoiMonitor {
	public:
		logStatus();

}

#endif