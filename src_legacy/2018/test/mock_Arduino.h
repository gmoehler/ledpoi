#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "WString.h"

#define SPI_FLASH_SEC_SIZE 4096

// functions mocking Arduino.h to be used

#define LOW 0
#define HIGH 1

unsigned long millis();
void delay(unsigned long ms);

void pinMode(int, int);
int digitalRead(int pin);

uint8_t constrain(uint8_t value, uint8_t lowerBound, uint8_t upperBound);

// functions controling the behaviour of the mocks

void mock_increaseTime(uint32_t ms);


#endif
