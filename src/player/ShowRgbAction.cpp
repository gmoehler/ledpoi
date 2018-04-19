#include "ShowRgbAction.h"

ShowRgbAction::ShowRgbAction(){}

void ShowRgbAction::init(PoiCommand cmd, PixelFrame* pframe, ActionOptions option) {

  // only allow SHOW_RGB action
  if (cmd.getType() != SHOW_RGB){
    LOGE(SRGB_A, "ERROR. %s cannot play action of type %s.", 
      getActionName(), cmd.commandTypeToString().c_str());
    return;
  }
  
  _pframe = pframe;

  // get color
  _rgb = makeRGBVal(
	cmd.getField(1),
	cmd.getField(2),
	cmd.getField(3)
	);

  // generate output
  for (int i=0; i< N_PIXELS; i++){
    _pframe->pixel[i] = _rgb;
  }
  
  _pframe->delay = 0;
  _pframe->idx = 0;
  _pframe->isLastFrame = true;

  // declare action to be started
  _active = true;
}

void ShowRgbAction::next(){
    // static: only has one state
  _active = false;
}

bool ShowRgbAction::isActive(){
  return _active;
}

void ShowRgbAction::printInfo(const char* prefix){
  LOGI(SRGB_A, "%sShowRgbAction - rgb: %3d %3d %3d.",prefix,  _rgb.r, _rgb.g, _rgb.b);
}

void ShowRgbAction::printState(){
  LOGD(SRGB_A, "ShowRgbAction state - rgb: %3d %3d %3d.", _rgb.r, _rgb.g, _rgb.b);
}

const char* ShowRgbAction::getActionName(){
  return "Show RGB Action";
}