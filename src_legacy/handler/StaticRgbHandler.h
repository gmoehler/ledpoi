#ifndef STATICRGB_HANDLER_H
#define STATICRGB_HANDLER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif

#include "ledpoi.h"
#include "AbstractStaticHandler.h"
#include "ImageCache.h"

/**
 * Holds the information for a static rgb action 
 **/

class StaticRgbHandler: public AbstractStaticHandler
{
public:
  StaticRgbHandler(ImageCache imageCache);
  void init(uint8_t r, uint8_t g, uint16_t b, uint8_t nLeds);

  const char* getActionName();
  
  rgbVal* getDisplayFrame();

  void printInfo();

private:
  ImageCache _imageCache;
  uint8_t _r;
  uint8_t _g;
  uint8_t _b;
};
#endif
