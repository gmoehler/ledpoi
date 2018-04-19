#include "PlayFramesAction.h"

PlayFramesAction::PlayFramesAction() :
 _startFrame(0), _endFrame(0), _delayMs(0),
 _numLoops(0), _currentFrame (0), _currentLoop(0),
 _active(false), _forward(true),
 _dimFactor(1.0) { }

void PlayFramesAction::init(PoiCommand cmd, PixelFrame* pframe, ActionOptions options) {
  
  // only allow PLAY_FRAMES action
  if (cmd.getType() != PLAY_FRAMES){
    LOGE(PLAYF_A, "ERROR. %s action cannot play action of type %s.", 
      getActionName(), cmd.commandTypeToString().c_str());
    return;
  }

   _dimFactor = options.dimFactor;
  _pframe = pframe;
  _pframe->idx = 0; // only one frame
  _pframe->isLastFrame = false;

  // read params
  _startFrame = cmd.getField(1);
  _startFrame = constrain(_startFrame,0,N_FRAMES-1);
  _endFrame = cmd.getField(2);
  _endFrame = constrain(_endFrame,0,N_FRAMES-1);
  _numLoops = cmd.getField(3);

  _delayMs = cmd.getCombinedField(4, 5);
  _pframe->delay = _delayMs;

  _currentFrame = _startFrame;
  _currentLoop = 0;
  _active = true;
  _forward = _endFrame >= _startFrame;
  imageCache.copyFrameToRegister(_pframe, _currentFrame, _dimFactor);
}

void PlayFramesAction::next(){

  if (_forward){
    if (_currentFrame + 1  >  _endFrame){
      // last frame reached
      if (_currentLoop + 1 >= _numLoops){
        // end of final loop reached
        _active = false;
      }
      else {
        // next loop starts
        _currentLoop++;
        _currentFrame = _startFrame;
      }
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
      }
      else {
        // next loop starts
        _currentLoop++;
        _currentFrame = _startFrame;
      }
    }
    else {
      // normal operation: next frame
      _currentFrame--;
    }
  }
  if (isActive()){
    imageCache.copyFrameToRegister(_pframe, _currentFrame, _dimFactor);
  }
  _pframe->isLastFrame = !_active;
}

bool PlayFramesAction::isActive(){
  return _active;
}

void PlayFramesAction::printInfo(const char* prefix){
  if (_startFrame == _endFrame && _numLoops == 1){
    LOGI(PLAYF_A, "%s %s:: Static Frame %d timeout: %d, dim factor: %.1f",
		prefix, getActionName(), _startFrame, _delayMs, _dimFactor);
  }
  else {
    LOGI(PLAYF_A, "%s %s:: Frames [%d,%d] delay: %d loops:%d dim factor: %.1f",
		 prefix, getActionName(), _startFrame, _endFrame, _delayMs, _numLoops, _dimFactor);
  }
}

void PlayFramesAction::printState(){
  LOGD(PLAYF_A, "%s: active: %d current frame: %d current loop: %d dim: %.2f", 
    getActionName(), _active, _currentFrame, _currentLoop, _dimFactor);
}

const char* PlayFramesAction::getActionName(){
  return "Play Frame";
}

void PlayFramesAction::setDimFactor(float factor){
  _dimFactor = factor;
  // update register with current dim level
  imageCache.copyFrameToRegister(_pframe, _currentFrame, _dimFactor);
}

#ifdef WITHIN_UNITTEST
uint8_t PlayFramesAction::__getCurrentFrame(){
  return _currentFrame;
}

uint16_t PlayFramesAction::__getCurrentLoop(){
  return _currentLoop;
}
#endif
