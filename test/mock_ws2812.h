#ifndef MOCK_WS2812_DRIVER_H
#define MOCK_WS2812_DRIVER_H

#include <stdint.h>

typedef union {
  struct __attribute__ ((packed)) {
    uint8_t r, g, b;
  };
  uint32_t num;
} rgbVal;

inline rgbVal makeRGBVal(uint8_t r, uint8_t g, uint8_t b)
{
  rgbVal v;
  v.r = r;
  v.g = g;
  v.b = b;
  return v;
}

#endif
