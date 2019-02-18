#ifndef ANIMATION_ACTION_H
#define ANIMATION_ACTION_H

#include "ledpoi.h"
#include "AbstractAction.h"

/**
 * Animation action with various animationd
 **/


class AnimationAction : public AbstractAction
{
public:
  AnimationAction();
  const char* getActionName();
  
  void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options);

  void next();
  bool isActive();
  
  void printInfo(const char* prefix);
  void printState();
  
private:  
  PixelFrame* _pframe;
  
  uint8_t _numLoops;
  uint8_t _registerLength;
  uint8_t _currentStep;
  uint8_t _currentLoop;
  
  bool _active;
  uint16_t _delay;
  
  void _black();
  bool _isLastLoop();
  bool _isLastFrame();

};
#endif
