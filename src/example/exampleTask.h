#ifndef EXAMPLE_TASK_H
#define EXAMPLE_TASK_H

#include "dispatch/dispatchTask.h"
#include "display/displayTask.h"

enum Example {    DISPLAY_EXAMPLE,   
                  PLAYER_EXAMPLE,    
                  SET_PIXEL_AND_PLAY_EXAMPLE,
                  SET_PIXEL_SAVE_AND_PLAY_EXAMPLE,
                  SET_PROGRAM_EXAMPLE,
                  WORM_EXAMPLE
              };

 // start example task
void example_start(Example example, uint8_t prio);     

#endif