#ifndef PLAY_SPIFFS_IMAGE_ACTION_H
#define PLAY_SPIFFS_IMAGE_ACTION_H

#include "ledpoi.h"
#include "AbstractAction.h"
#include "spiffsUtils.h"

/**
 * Holds the information for a play action of an image in spiffs
 **/

class PlaySpiffsImageAction: public AbstractAction
{
public:
  PlaySpiffsImageAction();
  const char* getActionName();

  void setup();
  
  void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options);

  void next();
  bool isActive();
  
  void printInfo(const char* prefix);
  void printState();

#ifdef WITHIN_UNITTEST
  uint8_t __getCurrentFrame();
#endif

private:
  PixelFrame* _pframe;

  uint16_t _delayMs;
  uint16_t _currentFrame;
  
  bool _active;
  bool _isLastFrame();
};
#endif
