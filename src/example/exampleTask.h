#ifndef EXAMPLE_TASK_H
#define EXAMPLE_TASK_H

#include "dispatch/dispatchTask.h"
#include "display/displayTask.h"

enum Example {    DISPLAY_EXAMPLE,   
              };

 // start example task
void example_start(Example example, uint8_t prio);     

#endif