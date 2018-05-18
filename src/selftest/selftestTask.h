#ifndef SELFTEST_TASK_H
#define SELFTEST_TASK_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"
#include "display/displayTask.h"


extern xQueueHandle dispatchQueue;

void selftest_start(uint8_t prio); 
void validateSelftest(PixelFrame& rframe);    

#endif