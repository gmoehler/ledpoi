#ifndef WIFI_TASK_H
#define WIFI_TASK_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"
#include "RobustWiFiServer.h"

extern xQueueHandle wifiControlQueue;

void wifi_setup(uint8_t queueSize);
void wifi_start(uint8_t prio);

#endif