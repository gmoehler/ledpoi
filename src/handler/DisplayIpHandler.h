#ifndef DISPLAYIP_HANDLER_H
#define DISPLAYIP_HANDLER_H

#include "ledpoi.h"
#include "AbstractStaticHandler.h"
#include "ImageCache.h"

/**
 * Holds the information for a static rgb action 
 **/

class DisplayIpHandler: public AbstractStaticHandler
{
public:
  DisplayIpHandler(ImageCache imageCache);
  void init(uint8_t ipOffset, bool withStaticBackground);

  const char* getActionName();
  
  rgbVal* getDisplayFrame();

  void printInfo();

private:
  ImageCache _imageCache;
  uint8_t _ipOffset;
  bool _withStaticBackground;
};
#endif
