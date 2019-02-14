#ifndef ANIMATION_HANDLER_H
#define ANIMATION_HANDLER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif
#include "ledpoi.h"
#include "AbstractHandler.h"
#include "ImageCache.h"

enum AnimationType {
  ANIMATIONTYPE_WORM,
  ANIMATIONTYPE_WUMMER
};

/**
 * Holds the information for a n animation action (currently only "Worm" animation supported)
 **/

class AnimationHandler : public AbstractHandler
{
public:
  AnimationHandler(ImageCache imageCache);
  void init(AnimationType animation, uint8_t registerLength, uint8_t numLoops, Color color, uint16_t delay=0);

  const char* getActionName();

  void next();
  bool isActive();
  
  uint16_t getDelayMs();
  rgbVal* getDisplayFrame();

  void finish() { _active = false;};

  void printInfo();
  void printState();
  
#ifdef WITHIN_UNITTEST
  uint8_t __getCurrentLoop();
#endif

private:
  bool _isLastLoop();
  void _init_worm();
  void _init_wummer();

  AnimationType _animation;
  uint8_t _registerLength;
  uint8_t _numLoops;
  uint8_t _currentStep;
  uint8_t _currentLoop;
  uint16_t _delayMs;
  Color _color;
  bool _active;
  uint8_t _numWummerSteps;
  float _wummerBrightnessFactor;

  ImageCache _imageCache;
};

#endif
