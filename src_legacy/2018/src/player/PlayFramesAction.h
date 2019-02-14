#ifndef FRAME_PLAY_ACTION_H
#define FRAME_PLAY_ACTION_H

#include "ledpoi.h"
#include "AbstractAction.h"
#include "memory/ImageCache.h"

extern ImageCache imageCache;

/**
 * Holds the information for a play action for frames in a scene
 **/

class PlayFramesAction: public AbstractAction
{
public:
  PlayFramesAction();
  const char* getActionName();
  
  void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options);

  void next();
  bool isActive();
  
  void printInfo(const char* prefix);
  void printState();

  void setDimFactor(float factor); // not part of AbstractAction
#ifdef WITHIN_UNITTEST
  uint8_t __getCurrentFrame();
  uint16_t __getCurrentLoop();
#endif

private:
  PixelFrame* _pframe;

  uint8_t _startFrame;
  uint8_t _endFrame;
  uint16_t _delayMs;
  uint16_t _numLoops;

  uint8_t _currentFrame;
  uint16_t _currentLoop;
  bool _active;
  bool _forward; // false for backward

  float _dimFactor;
  bool _isLastFrame();
};
#endif
