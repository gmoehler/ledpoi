#ifndef FRAME_PLAYER_H
#define FRAME_PLAYER_H

#include <Arduino.h>
#include "ledpoi.h"

/**
 * Holds the information for a play action for frames in a scene
 **/

class FramePlayer
{
public:
  FramePlayer();
  void init(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops);

  void next();

  bool isActive();
  uint8_t getDelayMs();
  uint8_t getCurrentScene();
  uint8_t getCurrentFrame();
  uint8_t getCurrentLoop();

  void printInfo();
  void printState();

private:
  uint8_t _scene;
  uint8_t _startFrame;
  uint8_t _endFrame;
  uint16_t _delayMs;
  uint8_t _numLoops;

  uint32_t _currentFrame;
  uint32_t _currentLoop;
  bool _active;

};
#endif
