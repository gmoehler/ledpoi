#ifndef MEMORY_TASK_H
#define MEMORY_TASK_H

#include "ledpoi.h"
#include "FS.h"
#include "SPIFFS.h"
#include <time.h> 
#include <esp_system.h>
#include <esp_partition.h>


#include "PoiCommand.h"

void spiffs_setup(uint8_t queueSize);
void spiffs_start(uint8_t prio);

#endif