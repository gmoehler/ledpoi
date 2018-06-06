#ifndef SELFTEST_TASK_H
#define SELFTEST_TASK_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"
#include "SelftestHelper.h"

void selftest_start(uint8_t prio); 
bool isSelftestActive();  
void selftestValidateFrame(PixelFrame& rframe);

#endif