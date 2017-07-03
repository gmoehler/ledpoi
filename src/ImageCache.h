#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
  #include <ws2812.h>
#else
  #include "../test/mock_Arduino.h"
  #include "../test/mock_ws2812.h"
#endif
#include "ledpoi.h"

/**
 * Holds the a chache for the image of the current scene and 2 registers
 **/

class ImageCache
{
public:
  ImageCache(uint32_t size, LogLevel loglevel);
  uint8_t* getRawImageData();

  rgbVal makeRGBValue(Color color, uint8_t brightness=255);
  rgbVal makeRGBValue(uint8_t rgb_array[3]);

  rgbVal* getRegister(uint8_t i);
  void printRegister(uint8_t i);
  void fillRegister(uint8_t registerId, rgbVal rgb, uint8_t nLeds=N_PIXELS);
  void clearRegister(uint8_t registerId); // fill with black
  void shiftRegister(uint8_t registerId1, uint8_t shiftRegisterLength, bool cyclic=false);

  void setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b);
  void _setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal getPixel(uint8_t frame_idx, uint8_t pixel_idx);

  void clearImageMap();
  void fillImageMap(rgbVal rgb);
  void copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor=1);
  void copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor=1);

private:
  // data stores
  // after each action the last frame is stored in _pixelRegister[0]
  rgbVal **_pixelRegister;
  uint8_t *_pixelMap;

  LogLevel _logLevel;
};



#endif