#include "StaticRgbHandler.h"

StaticRgbHandler::StaticRgbHandler(ImageCache imageCache) :
 _imageCache(imageCache) {}

void StaticRgbHandler::init(uint8_t r, uint8_t g, uint16_t b, uint8_t nLeds){
  // directly "play" out of register
  _r = r;
  _g = g;
  _b = b;
  _imageCache.fillRegister(0, makeRGBVal(r,g,b), nLeds);
}


rgbVal* StaticRgbHandler::getDisplayFrame(){
	return _imageCache.getRegister(0);
}

void StaticRgbHandler::printInfo(){
  printf("StaticRgbHandler: Rgb [%d,%d,%d]\n", _r, _g, _b);
}

const char* StaticRgbHandler::getActionName(){
  return "Show Static RGB";
}

