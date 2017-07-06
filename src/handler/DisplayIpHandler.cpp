#include "DisplayIpHandler.h"

DisplayIpHandler::DisplayIpHandler(ImageCache imageCache) :
 _imageCache(imageCache) {}

void DisplayIpHandler::init(uint8_t ipOffset, bool withStaticBackground){
  _ipOffset = ipOffset;

 // set back the ip led to black
  _imageCache.clearRegister(0);
  if (withStaticBackground){
    rgbVal paleWhite = makeRGBVal(8,8,8);
    _imageCache.fillRegister(0, paleWhite, N_POIS);
  }
  rgbVal* reg0 =  _imageCache.getRegister(0);
    // display colored led (first one less bright for each)
  uint8_t b = 64;
  if (ipOffset %2 == 0){
    b=8;
  }
  rgbVal color =  makeRGBValue(RED, b);
  switch(ipOffset/2){
    case 1:
    color =  makeRGBValue(GREEN, b);
    break;

    case 2:
    color =  makeRGBValue(BLUE, b);
    break;

    case 3:
    color =  makeRGBValue(YELLOW, b);
    break;

    case 4:
    color =  makeRGBValue(LILA, b);
    break;
  }
  reg0[ipOffset]= color;
}

rgbVal* DisplayIpHandler::getDisplayFrame(){
	return _imageCache.getRegister(0);
}

void DisplayIpHandler::printInfo(){
  printf("DisplayIpHandler: Ip Offset: %d\n", _ipOffset);
}

const char* DisplayIpHandler::getActionName(){
  return "Display IP";
}

