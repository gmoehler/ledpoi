#ifndef PLAY_HANDLER_H
#define PLAY_HANDLER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
#else
  #include "../test/mock_Arduino.h"
#endif

#include "ledpoi.h"
#include "ImageCache.h"

/**
 * Holds the information for a play action for frames in a scene
 **/

class PlayHandler
{
public:
  PlayHandler(ImageCache imageCache);
  void init(uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops);
  void setActiveScene(uint8_t scene);

  void next();

  bool isActive();
  bool isLastStep();

  uint16_t getDelayMs();
  uint8_t getCurrentFrame();
  uint16_t getCurrentLoop();

  void printInfo();
  void printState();

private:
  uint8_t _startFrame;
  uint8_t _endFrame;
  uint16_t _delayMs;
  uint16_t _numLoops;

  uint8_t _currentFrame;
  uint16_t _currentLoop;
  bool _active;
  bool _forward; // false for backward

  ImageCache _imageCache;
};
#endif
