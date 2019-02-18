#ifndef DISPLAYIP_HANDLER_H
#define DISPLAYIP_HANDLER_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "AbstractAction.h"
#include "PoiCommand.h"

/**
 * Displays the IPs as relative IPs (from 0 to 9) on leds
 **/

class DisplayIpAction: public AbstractAction
{
public:
  DisplayIpAction();
  const char* getActionName();
  
  void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options);

  void next();
  bool isActive();
  
  void printInfo(const char* prefix);
  void printState();

private:
  PixelFrame* _pframe;
  bool _active;

  uint8_t _ipOffset;
  bool _withStaticBackground;

  void _black();
  void _fillFrame(rgbVal rgb, uint8_t nLeds=N_PIXELS);
};
#endif
