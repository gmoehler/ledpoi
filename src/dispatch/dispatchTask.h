#ifndef DISPATCH_TASK_H
#define DISPATCH_TASK_H

#include "ledpoi.h"
#include "PoiCommand.h"
#include "dispatch/InteractionState.h"
#include "player/playerTask.h"
#include "memory/memoryTask.h"
#include "program/programTask.h"
#include "wifi/wifiTask.h"

extern xQueueHandle dispatchQueue;

void dispatch_setup(uint8_t queueSize);
void dispatch_start(uint8_t prio);

#endif