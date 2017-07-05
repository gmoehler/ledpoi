#ifndef FRAME_FADER_H
#define FRAME_FADER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif
#include "ledpoi.h"
#include "ImageCache.h"

#define N_FADE_STEPS_DEFAULT 50
#define MIN_FADE_TIME 20

/**
 * Holds the information for a fade action on a given frame in the scene
 **/


class FadeHandler
{
public:
  FadeHandler(ImageCache imageCache);
  void init(uint16_t fadeTime);

  void next();
  rgbVal* getDisplayFrame();

  bool isActive();
  bool isLastStep();

  uint16_t getDelayMs();

  void printInfo();
  void printState();

  float _getCurrentFadeFactor();

private:

  uint16_t _fadeTime;
  uint16_t _numFadeSteps;
  uint16_t _delayMs;

  uint8_t _currentFadeStep;
  bool _active;

  ImageCache _imageCache;

};
#endif
