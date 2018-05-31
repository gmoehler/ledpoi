#ifndef DISPATCH_TASK_H
#define DISPATCH_TASK_H

#include "ledpoi.h"
#include "PoiCommand.h"
#include "dispatch/InteractionState.h"

extern xQueueHandle dispatchQueue; // for reading only

void dispatch_setup(uint8_t queueSize);
void dispatch_start(uint8_t prio);

#endif