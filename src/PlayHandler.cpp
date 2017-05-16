#include "PlayHandler.h"

PlayHandler::PlayHandler() :
_scene(0), _numLoops(0), _startFrame(0), _endFrame(0), _delayMs(100),
_currentFrame (0), _currentLoop(0), _active(false) {}

void PlayHandler::init(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops) {
  _scene          = constrain(scene,0,N_SCENES-1);
  init(startFrame, endFrame, delay, loops);
}

void PlayHandler::init(uint8_t startFrame, uint8_t endFrame, uint16_t delay, uint16_t loops) {

  _startFrame     = constrain(startFrame,0,N_FRAMES-1);
  _endFrame       = constrain(endFrame,0,N_FRAMES-1);
  _delayMs        = delay;
  _numLoops       = loops;

  _currentFrame = _startFrame;
  _currentLoop = 0;
  _active = true;
}

  void PlayHandler::setActiveScene(uint8_t scene){
    _scene = scene;
  }

void PlayHandler::next(){

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

bool PlayHandler::isActive(){
  return _active;
}

uint16_t PlayHandler::getDelayMs(){
  return _delayMs;
}

uint8_t PlayHandler::getCurrentScene(){
  return _scene;
}

uint8_t PlayHandler::getCurrentFrame(){
  return _currentFrame;
}

uint16_t PlayHandler::getCurrentLoop(){
  return _currentLoop;
}

void PlayHandler::printInfo(){
  printf("PlayHandler: Scene: %d frames: [%d,%d] delay: %d loops:%d \n", _scene, _startFrame, _endFrame, _delayMs, _numLoops);
}

void PlayHandler::printState(){
  printf("PlayHandler: Active: %d Current Scene: %d current frame: %d current loop: %d \n", _active, _scene, _currentFrame, _currentLoop);
}
