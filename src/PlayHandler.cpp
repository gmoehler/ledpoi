#include "PlayHandler.h"

PlayHandler::PlayHandler() :
 _startFrame(0), _endFrame(0), _delayMs(0),
 _numLoops(0),
 _currentFrame (0), _currentLoop(0),
 _active(false), _forward(true) {}

void PlayHandler::init(uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops) {

  _startFrame     = constrain(startFrame,0,N_FRAMES-1);
  _endFrame       = constrain(endFrame,0,N_FRAMES-1);
  _delayMs        = delay;
  _numLoops       = loops;

  _currentFrame = _startFrame;
  _currentLoop = 0;
  _active = true;
  _forward = endFrame >= startFrame;
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
      _currentFrame++;
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
      _currentFrame--;
    }

  }
}

bool PlayHandler::isActive(){
  return _active;
}

bool PlayHandler::isLastStep(){
  return (_currentFrame == _endFrame && _currentLoop == _numLoops - 1 && _active);
}

uint16_t PlayHandler::getDelayMs(){
  return _delayMs;
}

uint8_t PlayHandler::getCurrentFrame(){
  return _currentFrame;
}

uint16_t PlayHandler::getCurrentLoop(){
  return _currentLoop;
}

void PlayHandler::printInfo(){
  printf("PlayHandler: Frames [%d,%d] delay: %d loops:%d \n", _startFrame, _endFrame, _delayMs, _numLoops);
}

void PlayHandler::printState(){
  printf("PlayHandler: Active: %d Current frame: %d current loop: %d \n", _active, _currentFrame, _currentLoop);
}
