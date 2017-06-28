#ifndef WS2812_DRIVER_H
#define WS2812_DRIVER_H

#include <stdint.h>

typedef union {
  struct __attribute__ ((packed)) {
    uint8_t r, g, b;
  };
  uint32_t num;
} rgbVal;

#endif