#include "AnimationHandler.h"

AnimationHandler::AnimationHandler(ImageCache imageCache)  :
  _currentLoop(0), _delayMs(0), _active(false),
  _imageCache(imageCache) {}

void AnimationHandler::init(AnimationType animation, uint8_t registerLength,
  uint8_t numLoops, Color color, uint16_t delay) {
  _animation = animation;
  _registerLength = registerLength;
  _numLoops = numLoops;
  _currentLoop = 0;
  _currentStep = 0;
  _active = true;
  _delayMs = delay;

  _imageCache.clearRegister(0);
  rgbVal* reg0 =  _imageCache.getRegister(0);
  if  (color == RAINBOW){
    rgbVal red =  makeRGBValue(RED);
    rgbVal green =  makeRGBValue(GREEN);
    rgbVal blue =  makeRGBValue(BLUE);
    rgbVal yellow =  makeRGBValue(YELLOW);
    rgbVal lila =  makeRGBValue(LILA);
    rgbVal cyan =  makeRGBValue(CYAN);

    // initialize register 0 with rainbow
    rgbVal rainbow[6] = {lila, blue, cyan, green, red, yellow};
    for (int i=0; i<6; i++){
      reg0[i] = rainbow[i];
    }
  }
  else {
   reg0[0] = makeRGBValue( color );
  }
}


void AnimationHandler::next(){

  // numLoops=0 means eternal
  if (_numLoops > 0){
    if (_currentStep + 2 > _registerLength) {
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
    _imageCache.shiftRegister(0, _registerLength, cyclic);
  }
  else {
    _imageCache.clearRegister(0);
  }

}

bool AnimationHandler::isActive(){
  return _active;
}

rgbVal* AnimationHandler::getDisplayFrame(){
	return _imageCache.getRegister(0);
}

bool AnimationHandler::_isLastLoop(){
  return (_numLoops > 0 && _currentLoop + 1 >= _numLoops && _active);
}

uint16_t AnimationHandler::getDelayMs(){
  return _delayMs;
}

#ifdef WITHIN_UNITTEST
uint8_t AnimationHandler::__getCurrentLoop(){
  return _currentLoop;
}
#endif

void AnimationHandler::printInfo(){
  printf("AnimationHandler: NumLoops: %d\n", _numLoops);
}

void AnimationHandler::printState(){
  printf("AnimationHandler: Active: %d Current step: %d Current loop: %d \n",
    _active, _currentStep, _currentLoop);
}
