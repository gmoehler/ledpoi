#ifndef ANIMATION_HANDLER_H
#define ANIMATION_HANDLER_H

#include <Arduino.h>
#include "ledpoi.h"

enum AnimationType {
  ANIMATIONTYPE_WORM
};

/**
 * Holds the information for a n animation action (currently only "Worm" animation supported)
 **/

class AnimationHandler
{
public:
  AnimationHandler();
  void init(AnimationType animation, uint8_t registerLength, uint8_t numLoops);

  void next();
  bool isActive();
  bool isLastLoop();
  uint8_t getRegisterLength();
  uint8_t getCurrentLoop();

  void printInfo();
  void printState();

private:
  AnimationType _animation;
  uint8_t _registerLength;
  uint8_t _numLoops;
  uint8_t _currentStep;
  uint8_t _currentLoop;
  bool _active;
};

#endif
