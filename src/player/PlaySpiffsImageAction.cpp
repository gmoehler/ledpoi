#include "PlaySpiffsImageAction.h"

SpiffsUtils spiffsUtil;

PlaySpiffsImageAction::PlaySpiffsImageAction() :
  _delayMs(0), _currentFrame (0),  _active(false){ 
  
 }

void PlaySpiffsImageAction::setup() {
 spiffsUtil.setup();
}

void PlaySpiffsImageAction::init(PoiCommand cmd, PixelFrame* pframe, ActionOptions options) {
  
  // only allow PLAY_FRAMES action
  if (cmd.getType() != PLAY_SPIFFS_IMAGE){
    LOGE(SPIFF_A, "ERROR. %s action cannot play action of type %s.", 
      getActionName(), cmd.commandTypeToString().c_str());
    return;
  }

  _pframe = pframe;
  _pframe->idx = 0;
  _pframe->isLastFrame = false;

  // read params
  _delayMs = cmd.getCombinedField(4, 5);
  _delayMs = (_delayMs == 0) ? DEFAULT_DELAY : _delayMs;
  _pframe->delay = _delayMs;

  spiffsUtil.openFile("/channel.poi");
  _header =  spiffsUtil.getHeader();

  _currentFrame = 0;
  _active = spiffsUtil.getNextFrame(_pframe);
}

void PlaySpiffsImageAction::next(){
  _active = _currentFrame < _header.width-1;
  if (_active) {
    // if not enough data _active will be false
    _active = spiffsUtil.getNextFrame(_pframe);
    LOGD(SPIFF_A, "curFrame %d of %d (height: %d)\n", _currentFrame, _header.width, _header.height);

    if (_active) {
		  _currentFrame++;
		  _pframe->idx = _currentFrame;
    } 
  }
  _pframe->isLastFrame = !_active;
}

bool PlaySpiffsImageAction::isActive(){
  return _active;
}

void PlaySpiffsImageAction::printInfo(const char* prefix){
    LOGI(SPIFF_A, "%s %s:: Frames delay: %d",
		 prefix, getActionName(), _delayMs);
}

void PlaySpiffsImageAction::printState(){
  LOGD(SPIFF_A, "%s: active: %d current frame: %d", 
    getActionName(), _active, _currentFrame);
}

const char* PlaySpiffsImageAction::getActionName(){
  return "Play Spiffs Image";
}

#ifdef WITHIN_UNITTEST
uint8_t PlaySpiffsImageAction::__getCurrentFrame(){
  return _currentFrame;
}
uint16_t PlaySpiffsImageAction::__getDelay(){
  return _delayMs;
}
#endif
