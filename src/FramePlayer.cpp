#include "FramePlayer.h"

FramePlayer::FramePlayer() :
_scene(0), _numLoops(0), _startFrame(0), _endFrame(0), _delayMs(100),
_currentFrame (0), _currentLoop(0), _active(false) {}

void FramePlayer::init(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops) {
  _scene          = constrain(scene,0,N_SCENES-1);
  init(startFrame, endFrame, speed, loops);
}

void FramePlayer::init(uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops) {

  _startFrame     = constrain(startFrame,0,N_FRAMES-1);
  _endFrame       = constrain(endFrame,0,N_FRAMES-1);
  _delayMs        = speed;
  _numLoops       = loops;

  _currentFrame = _startFrame;
  _currentLoop = 0;
  _active = true;
}

  void FramePlayer::setActiveScene(uint8_t scene){
    _scene = scene;
  }

void FramePlayer::next(){

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

bool FramePlayer::isActive(){
  return _active;
}

uint8_t FramePlayer::getDelayMs(){
  return _delayMs;
}

uint8_t FramePlayer::getCurrentScene(){
  return _scene;
}
uint8_t FramePlayer::getCurrentFrame(){
  return _currentFrame;
}
uint8_t FramePlayer::getCurrentLoop(){
  return _currentLoop;
}

void FramePlayer::printInfo(){
  printf("FramePlayer: Scene: %d frames: [%d,%d] delay: %d loops:%d \n", _scene, _startFrame, _endFrame, _delayMs, _numLoops);
}

  void FramePlayer::printState(){
    printf("FramePlayer: Active: %d Current Scene: %d current frame: %d current loop: %d \n", _active, _scene, _currentFrame, _currentLoop);
  }
