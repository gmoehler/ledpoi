#ifndef ANIMATION_HANDLER_H
#define ANIMATION_HANDLER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif
#include "ledpoi.h"
#include "ImageCache.h"

enum AnimationType {
  ANIMATIONTYPE_WORM
};

/**
 * Holds the information for a n animation action (currently only "Worm" animation supported)
 **/

class AnimationHandler
{
public:
  AnimationHandler(ImageCache imageCache);
  void init(AnimationType animation, uint8_t registerLength, uint8_t numLoops, Color color, uint16_t delay=0);

  void next();
  rgbVal* getDisplayFrame();

  bool isActive();
  bool isLastStep();
  bool isLastLoop();

  uint16_t getDelayMs();
  uint8_t getCurrentLoop();

  void printInfo();
  void printState();

private:
  AnimationType _animation;
  uint8_t _registerLength;
  uint8_t _numLoops;
  uint8_t _currentStep;
  uint8_t _currentLoop;
  uint16_t _delayMs;
  bool _active;

  ImageCache _imageCache;
};

#endif
