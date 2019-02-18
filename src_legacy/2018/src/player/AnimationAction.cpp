#include "AnimationAction.h"

AnimationAction::AnimationAction() :
  _numLoops(0), _registerLength(0), _currentStep(0), _currentLoop(0),
  _active(false), _delay(100)
{}


void AnimationAction::init(PoiCommand cmd, PixelFrame* pframe, ActionOptions option) {

  // only allow ANIMATION action
  if (cmd.getType() != ANIMATE){
    LOGE(ANIM_A, "ERROR. %s cannot play action of type %s.", 
      getActionName(), cmd.commandTypeToString().c_str());
    return;
  }

  _pframe = pframe;

  // determine delay
  _delay = cmd.getCombinedField(4, 5);
  _pframe->delay = _delay;
    
  Color color =  static_cast<Color>(cmd.getField(1));
  _numLoops = cmd.getField(2);
  _registerLength = cmd.getField(3);
  _currentStep = 0;
  _currentLoop = 0;
  
  // initialize with color
  _black();
  if (color == RAINBOW) {
    rgbVal red =  makeRGBValue(RED);
    rgbVal green =  makeRGBValue(GREEN);
    rgbVal blue =  makeRGBValue(BLUE);
    rgbVal yellow =  makeRGBValue(YELLOW);
    rgbVal lila =  makeRGBValue(LILA);
    rgbVal cyan =  makeRGBValue(CYAN);

    rgbVal rainbow[6] = {lila, blue, cyan, green, red, yellow};
    for (int i=0; i<6; i++){
     _pframe->pixel[i] = rainbow[i];
    }
  }
  else {
    _pframe->pixel[0] = makeRGBValue(color);
  }

  _pframe->idx = 0;
  // declare action to be started
  _active = true;
}

void AnimationAction::_black(){
  for (int i=0; i< N_PIXELS; i++){
    _pframe->pixel[i] = makeRGBVal(0,0,0);
  }
}

void AnimationAction::next(){
	if (_numLoops > 0){
    if (_currentStep >= _registerLength) {
      if (_currentLoop + 1 >= _numLoops){
        _active = false;
      }
      else {
        _currentLoop++;
        _currentStep = 0;
      }
    }
    else {
      _currentStep++;
    }
  }

  if (_active){
      // be cyclic except in last loop and on first step on last loop
      bool cyclic = !_isLastLoop() || _currentStep == 0;
      shiftPixelframe(_pframe, _registerLength, cyclic);
    }
  else {
    _black();
  }

  _pframe->idx++;
  _pframe->isLastFrame = !_active;
}

bool AnimationAction::_isLastLoop(){
  return (_numLoops > 0 && _currentLoop + 1 >= _numLoops && _active);
}

bool AnimationAction::isActive(){
  return _active;
}

void AnimationAction::printInfo(const char* prefix){
  LOGI(ANIM_A, "%sAnimationAction - delay: %d num loops: %d len: %d", 
    prefix, _delay, _numLoops, _registerLength);
}

void AnimationAction::printState(){
  LOGD(ANIM_A, "AnimationAction - active: %d step: %d loop: %d.",
    _active, _currentStep, _currentLoop);
}

const char* AnimationAction::getActionName(){
  return "Sample Action";
}