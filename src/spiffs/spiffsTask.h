#ifndef SPIFFS_TASK_H
#define SPIFFS_TASK_H

#include <esp_system.h>
#include <esp_partition.h>

#include "FS.h"
#include "SPIFFS.h"
#include "spiffsUtils.h"

#include "ledpoi.h"
#include "PoiCommand.h"
#include "ledpoi_utils.h"

extern xQueueHandle spiffsQueue; // for reading only

void spiffs_setup(uint8_t queueSize);
void spiffs_start(uint8_t prio);

#endif