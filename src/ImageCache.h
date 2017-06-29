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



class ImageCache
{
public:
  ImageCache(uint32_t size, LogLevel loglevel);
  rgbVal _makeRGBValue(uint8_t rgb_array[3]);
  rgbVal _makeRGBValue(Color color, uint8_t brightness=255);
  rgbVal* getRegister(uint8_t i);
  void printRegister(uint8_t i);
  void _setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b);
  void _setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal _getPixel(uint8_t frame_idx, uint8_t pixel_idx);

  void _copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor=1);
  void _copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor=1);
  void _fillRegister(uint8_t registerId, rgbVal rgb);
  void _clearRegister(uint8_t registerId); // fill with black
  void _shiftRegister(uint8_t registerId1, uint8_t shiftRegisterLength, bool cyclic=false);
  void _fillMap(rgbVal rgb);
  void clearImageMap();
private:
  // data stores
  // after each action the last frame is stored in _pixelRegister[0]
  rgbVal **_pixelRegister;
  uint8_t *_pixelMap;

  LogLevel _logLevel;

};



#endif