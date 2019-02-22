#ifndef DISPATCH_TASK_H
#define DISPATCH_TASK_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"
#include "dispatch/InteractionState.h"
#include "dispatch/PoiMonitor.h"

#include "display/displayTask.h"
#include "player/playerTask.h"

extern xQueueHandle dispatchQueue; // for reading only

void dispatch_setup(uint8_t queueSize);
void dispatch_start(uint8_t prio);

#endif