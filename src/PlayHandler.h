#ifndef FRAME_PLAYER_H
#define FRAME_PLAYER_H

#include <Arduino.h>
#include "ledpoi.h"

/**
 * Holds the information for a play action for frames in a scene
 **/

class PlayHandler
{
public:
  PlayHandler();
  void init(uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops);
  void init(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops);
  void setActiveScene(uint8_t scene);

  void next();

  bool isActive();
  uint16_t getDelayMs();
  uint8_t getCurrentScene();
  uint8_t getCurrentFrame();
  uint16_t getCurrentLoop();

  void printInfo();
  void printState();

private:
  uint8_t _scene;
  uint8_t _startFrame;
  uint8_t _endFrame;
  uint16_t _delayMs;
  uint16_t _numLoops;

  uint8_t _currentFrame;
  uint16_t _currentLoop;
  bool _active;

};
#endif
