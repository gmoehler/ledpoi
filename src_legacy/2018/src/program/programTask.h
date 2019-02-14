/******************************************************************
 * 
 * Task that drives the ws2812 led chain
 * 
 ******************************************************************/

#ifndef PROGRAM_TASK_H
#define PROGRAM_TASK_H

#include "ledpoi.h"
#include "PoiCommand.h"

#include "program/PoiProgramHandler.h"
#include "player/playerTask.h"       // for cleaning methods
#include "display/displayTask.h"     // for cleaning methods
#include "dispatch/PoiMonitor.h"

extern xQueueHandle programQueue;    // for reading only

void program_setup(uint8_t queueSize); // setup program task
void program_start(uint8_t prio, uint8_t prioPrgExecTask);      // start program task and timer

#endif