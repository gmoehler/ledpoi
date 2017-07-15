#ifndef PLAY_HANDLER_H
#define PLAY_HANDLER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif

#include "ledpoi.h"
#include "AbstractHandler.h"
#include "ImageCache.h"

/**
 * Holds the information for a play action for frames in a scene
 **/

class PlayHandler: public AbstractHandler
{
public:
  PlayHandler(ImageCache imageCache);
  void init(uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops);

  const char* getActionName();

  void next();
  bool isActive();
  
  uint16_t getDelayMs();
  rgbVal* getDisplayFrame();

  void printInfo();
  void printState();

  void setDimFactor(float factor);

#ifdef WITHIN_UNITTEST
  uint8_t __getCurrentFrame();
  uint16_t __getCurrentLoop();
#endif

private:
  uint8_t _startFrame;
  uint8_t _endFrame;
  uint16_t _delayMs;
  uint16_t _numLoops;

  uint8_t _currentFrame;
  uint16_t _currentLoop;
  bool _active;
  bool _forward; // false for backward

  float _dimFactor;

  ImageCache _imageCache;
};
#endif
