#ifndef PLAYER_TASK
#define PLAYER_TASK

#include "ledpoi.h"
#include "PoiCommand.h"

#include "player/AbstractAction.h"
#include "player/NoAction.h"
#include "player/PlayFramesAction.h"
#include "player/ShowRgbAction.h"
#include "player/DisplayIpAction.h"
#include "player/AnimationAction.h"

#include "memory/memoryTask.h" // for scene loading

extern xQueueHandle playerQueue;
extern xQueueHandle displayQueue;

void player_setup(uint8_t queueSize);
void player_start(uint8_t prio);
void player_skipActions();     // skip all commands until player is resumed
void player_resume();   // resume player

#endif