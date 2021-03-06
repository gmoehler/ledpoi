#ifndef UART_TASK_H
#define UART_TASK_H

#include "driver/uart.h"
#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"

void uart_setup();
void uart_start(uint8_t prio);

#endif