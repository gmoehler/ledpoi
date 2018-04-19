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
  _color = color;
  _numWummerSteps = 20;
  _wummerBrightnessFactor = 0.05;

  _imageCache.clearRegister(0);
  
  switch(_animation){
    case ANIMATIONTYPE_WORM:
    _init_worm();
    break;

    case ANIMATIONTYPE_WUMMER:
    _init_wummer();
    break;

    default:
    break;
  }

}

void AnimationHandler::_init_worm(){
  rgbVal* reg0 =  _imageCache.getRegister(0);
  if  (_color == RAINBOW){
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
   reg0[0] = makeRGBValue( _color );
  }
}

void AnimationHandler::_init_wummer(){

}

void AnimationHandler::next(){

  // numLoops=0 means eternal
  switch(_animation){

    case ANIMATIONTYPE_WORM: {
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
  break;  

  case ANIMATIONTYPE_WUMMER: {
    _currentStep++;
    if (_currentStep == _numWummerSteps){
      _currentStep = 0;
      // reached end of animation (numloops == 0: run forever)
      if (_numLoops > 0 && _currentLoop >= _numLoops-1){
        _active = false;
        // finally clean leds
        rgbVal* reg0 = _imageCache.getRegister(0);
        rgbVal c =  makeRGBValue(BLACK);
        for (int i=0; i<_registerLength; i++){
          reg0[i] = c;
          reg0[N_PIXELS-1-i] = c;
        }
        return;
      }
      _currentLoop++;
    }
    // not to full brightness
    float factor = _wummerBrightnessFactor *  _currentStep / _numWummerSteps;
    rgbVal* reg0 = _imageCache.getRegister(0);
    rgbVal c1 =  fadeColor(_color, factor);
    rgbVal c2 =  fadeColor(_color, _wummerBrightnessFactor - factor);
    for (int i=0; i<_registerLength; i++){
      rgbVal c = (i%2==0) ? c1 : c2;
      reg0[i] = c;
      reg0[N_PIXELS-1-i] = c;
    }
  }
  break;

  default:
  break;
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

void AnimationHandler::printInfo(){
  if (_animation == ANIMATIONTYPE_WORM){
    printf("AnimationHandler: Worm with color: %d len: %d numLoops: %d delay: %d\n", 
      _color, _registerLength, _numLoops, _delayMs);
  }
  else {
    printf("AnimationHandler: Wummer with color: %d len: %d numLoops: %d delay: %d\n", 
      _color, _registerLength, _numLoops, _delayMs);
  }
}

void AnimationHandler::printState(){
  printf("AnimationHandler: Active: %d Current step: %d Current loop: %d \n",
    _active, _currentStep, _currentLoop);
}

const char* AnimationHandler::getActionName(){
  return "Worm Animation";
}

#ifdef WITHIN_UNITTEST
uint8_t AnimationHandler::__getCurrentLoop(){
  return _currentLoop;
}
#endif
