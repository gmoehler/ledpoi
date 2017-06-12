#ifndef FRAME_FADER_H
#define FRAME_FADER_H

#include <Arduino.h>
#include "ledpoi.h"

#define N_FADE_STEPS_DEFAULT 50
#define MIN_FADE_TIME 20

/**
 * Holds the information for a fade action on a given frame in the scene
 **/


class FadeHandler
{
public:
  FadeHandler();
  void init(uint16_t fadeTime);

  void next();

  bool isActive();
  uint8_t getDelayMs();
  float getCurrentFadeFactor();

  void printInfo();
  void printState();

private:
  uint16_t _fadeTime;
  uint16_t _numFadeSteps;
  uint16_t _delayMs;

  bool _active;
  uint8_t _currentFadeStep;
};
#endif
