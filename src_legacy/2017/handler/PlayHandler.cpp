#include "PlayHandler.h"

PlayHandler::PlayHandler(ImageCache imageCache) :
 _startFrame(0), _endFrame(0), _delayMs(0),
 _numLoops(0),
 _currentFrame (0), _currentLoop(0),
 _active(false), _forward(true), _actionPaused(false),
 _dimFactor(1.0),
 _imageCache(imageCache) {}

void PlayHandler::init(uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops) {

  _startFrame     = constrain(startFrame,0,N_FRAMES-1);
  _endFrame       = constrain(endFrame,0,N_FRAMES-1);
  _delayMs        = delay;
  _numLoops       = loops;

  _currentFrame = _startFrame;
  _currentLoop = 0;
  _active = true;
  _forward = endFrame >= startFrame;
  _imageCache.copyFrameToRegister(0, _currentFrame, _dimFactor);
}

void PlayHandler::next(){

  if (_forward){
    if (_currentFrame + 1  >  _endFrame){
      // last frame reached
      if (_currentLoop + 1 >= _numLoops){
        // end of final loop reached
        _active = false;
        return;
      }
      // next loop starts
      _currentLoop++;
      _currentFrame = _startFrame;
    }
    else {
      // normal operation: next frame
      if (!_actionPaused){
        _currentFrame++;
      }
    }
  }
  // backward
  else {
    if (_currentFrame - 1  <  _endFrame){
      // last frame reached
      if (_currentLoop + 1 >= _numLoops){
        // end of final loop reached
        _active = false;
        return;
      }
      // next loop starts
      _currentLoop++;
      _currentFrame = _startFrame;
    }
    else {
      // normal operation: next frame
      if (!_actionPaused) {
        _currentFrame--;
      }
    }
  }
  if (isActive()){
    _imageCache.copyFrameToRegister(0, _currentFrame, _dimFactor);
  }
}

bool PlayHandler::isActive(){
  return _active;
}

void PlayHandler::pauseAction(){
  _actionPaused = true;
}

void PlayHandler::continueAction(){
  _actionPaused = false;
}

rgbVal* PlayHandler::getDisplayFrame(){
	return _imageCache.getRegister(0);
}

uint16_t PlayHandler::getDelayMs(){
  return _delayMs;
}


void PlayHandler::printInfo(){
  if (_startFrame == _endFrame && _numLoops == 1){
    printf("PlayHandler: Static Frame %d timeout: %d \n", _startFrame, _delayMs);
  }
  else {
    printf("PlayHandler: Frames [%d,%d] delay: %d loops:%d \n", _startFrame, _endFrame, _delayMs, _numLoops);
  }
}

void PlayHandler::printState(){
  printf("PlayHandler: Active: %d Current frame: %d current loop: %d dim: %.2f", 
    _active, _currentFrame, _currentLoop, _dimFactor);
  if (_actionPaused){
    printf("... pausing.\n");
  }
  else {
    printf("\n");
  }
}

const char* PlayHandler::getActionName(){
  return "Play Frame";
}

void PlayHandler::setDimFactor(float factor){
  _dimFactor = factor;
  // update register with current dim level
  _imageCache.copyFrameToRegister(0, _currentFrame, _dimFactor);
}

#ifdef WITHIN_UNITTEST
uint8_t PlayHandler::__getCurrentFrame(){
  return _currentFrame;
}

uint16_t PlayHandler::__getCurrentLoop(){
  return _currentLoop;
}
#endif
