#ifndef UART_TASK_H
#define UART_TASK_H

#include "driver/uart.h"
#include "ledpoi.h"
#include "PoiCommand.h"

extern xQueueHandle dispatchQueue;

void uart_setup();
void uart_start(uint8_t prio);

#endif