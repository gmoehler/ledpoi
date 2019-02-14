#include "NoAction.h"

NoAction::NoAction() :
  _active(false) {}
 
void NoAction::init(PoiCommand cmd, PixelFrame* pframe, ActionOptions options) {
  _active = false;
  fillFrame(pframe, 0, 100,  0, 0, 0);
}

void NoAction::next(){
  return;
}

bool NoAction::isActive(){
  return _active;
}

void NoAction::printInfo(const char* prefix){
  LOGI(NOACT_A, "%sNoAction", prefix);
}

void NoAction::printState(){
  LOGD(NOACT_A, "NoAction - inactive");
}

const char* NoAction::getActionName(){
  return "No Action";
}