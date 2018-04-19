#ifndef BUTTON_H
#define BUTTON_H

#include "ledpoi.h"
#include "PoiCommand.h"
#include "dispatch/dispatchTask.h"

#define GPIO_INPUT_IO_0       GPIO_NUM_0
#define GPIO_INPUT_PIN_SEL    (1<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

void IRAM_ATTR buttonIsrHandler(void* arg);
void button_setup();

#endif