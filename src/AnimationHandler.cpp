#include "AnimationHandler.h"

AnimationHandler::AnimationHandler()  :
  _currentLoop(0), _delayMs(0), _active(false) {}

void AnimationHandler::init(AnimationType animation, uint8_t registerLength,
  uint8_t numLoops, uint16_t delay) {
  _animation = animation;
  _registerLength = registerLength;
  _numLoops = numLoops;
  _currentLoop = 0;
  _currentStep = 0;
  _active = true;
  _delayMs = delay;
}


void AnimationHandler::next(){

  // numLoops=0 means eternal
  if (_numLoops == 0){
    return;
  }
  if (_currentStep + 2 > _registerLength) {
    if (_currentLoop + 1 >= _numLoops){
      _active = false;
      return;
    }
    _currentLoop++;
    _currentStep = 0;
  }
  else {
    _currentStep++;
  }
}

bool AnimationHandler::isActive(){
  return _active;
}

bool AnimationHandler::isLastStep(){
  return (_currentStep == _registerLength - 1 && _currentLoop == _numLoops - 1 && _active);
}

bool AnimationHandler::isLastLoop(){
  return (_numLoops > 0 && _currentLoop + 1 >= _numLoops && _active);
}

uint16_t AnimationHandler::getDelayMs(){
  return _delayMs;
}

uint8_t AnimationHandler::getCurrentLoop(){
  return _currentLoop;
}

uint8_t AnimationHandler::getRegisterLength(){
  return _registerLength;
}

void AnimationHandler::printInfo(){
  printf("AnimationHandler: NumLoops: %d\n", _numLoops);
}

void AnimationHandler::printState(){
  printf("AnimationHandler: Active: %d Current step: %d Current loop: %d \n",
    _active, _currentStep, _currentLoop);
}
