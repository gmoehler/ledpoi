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

#include "display/displayTask.h"
#include "memory/memoryTask.h" // for scene loading

extern xQueueHandle playerQueue;

void player_setup(uint8_t queueSize);
void player_start(uint8_t prio);

#endif