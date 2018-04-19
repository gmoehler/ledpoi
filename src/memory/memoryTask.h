#ifndef MEMORY_TASK_H
#define MEMORY_TASK_H

#include "ledpoi.h"
#include <esp_system.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <nvs.h>


#include "PoiCommand.h"
#include "ImageCache.h"
#include "ProgramCache.h"
#include "PoiFlashMemory.h"

extern xQueueHandle memoryQueue;
extern ImageCache imageCache;
extern ProgramCache programCache;

void memory_setup(uint8_t queueSize);
void memory_start(uint8_t prio);

// required to do synchronous scene loading from the player task
void loadSceneFromFlash(uint8_t scene);

#endif