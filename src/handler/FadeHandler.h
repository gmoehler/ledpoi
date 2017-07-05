#ifndef FRAME_FADER_H
#define FRAME_FADER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif
#include "ledpoi.h"
#include "AbstractHandler.h"
#include "ImageCache.h"

#define N_FADE_STEPS_DEFAULT 50
#define MIN_FADE_TIME 20

/**
 * Holds the information for a fade action on a given frame in the scene
 **/


class FadeHandler : public AbstractHandler
{
public:
  FadeHandler(ImageCache imageCache);
  void init(uint16_t fadeTime);

  const char* getActionName();

  void next();
  bool isActive();
  
  uint16_t getDelayMs();
  rgbVal* getDisplayFrame();
  
  void printInfo();
  void printState();
  
#ifdef WITHIN_UNITTEST
  float __getCurrentFadeFactor();
#endif

private:
  float _getCurrentFadeFactor();
  
  uint16_t _fadeTime;
  uint16_t _numFadeSteps;
  uint16_t _delayMs;

  uint8_t _currentFadeStep;
  bool _active;

  ImageCache _imageCache;
  
  

};
#endif
