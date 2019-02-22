#include "DisplayIpAction.h"

DisplayIpAction::DisplayIpAction() :
  _active(false), _ipOffset(0), 
  _withStaticBackground(false) {}

void DisplayIpAction::init(PoiCommand cmd, PixelFrame* pframe, ActionOptions options) {

    // only allow DISPLAY_IP action
  if (cmd.getType() != DISPLAY_IP){
    LOGE(DISP_T, "ERROR. %s action cannot play action of type %s ", 
      getActionName(), cmd.commandTypeToString().c_str());
    return;
  }

  _pframe = pframe;
  _pframe->idx = 0; // only one frame
  _pframe->isLastFrame = true;
  _pframe->delay = 0;

  // get relative ip
  _ipOffset = cmd.getField(1);
  _withStaticBackground = cmd.getField(2) > 0;

  // set back the ip led to black
  _black();

  // fill with background
  if (_withStaticBackground){
    rgbVal paleWhite = makeRGBVal(8,8,8);
    _fillFrame(paleWhite, N_POIS);
  }
  // display colored led (first one less bright for each)
  if (_ipOffset < N_POIS) {
  	uint8_t b = 64;
  	if (_ipOffset %2 == 0){
   	 b=8;
  	}
  
      Color col =  static_cast<Color>(_ipOffset/2 + 2);
  	rgbVal color =  makeRGBValue(col, b);
  	
	  _pframe->pixel[_ipOffset]= color;
  }
  // declare action to be started
  _active = true;
}

bool DisplayIpAction::isActive(){
  return _active;
}


void DisplayIpAction::_black(){
  for (int i=0; i< N_PIXELS; i++){
    _pframe->pixel[i] = makeRGBVal(0,0,0);
  }
}

void DisplayIpAction::_fillFrame(rgbVal rgb, uint8_t nLeds){
  for (int i=0; i< nLeds; i++){
    _pframe->pixel[i] = rgb;
  }
}

void DisplayIpAction::next(){
  // static: only has one state
  _active = false;
}

void DisplayIpAction::printInfo(const char* prefix){
  char buffer[40];
  if (_withStaticBackground){
    strcpy(buffer, " with static background");
  }
  else {
    strcpy(buffer, "");
  }
  LOGI(DISP_T, "%s%s: Ip Offset: %d %s", prefix, getActionName(), _ipOffset, buffer);
}

void DisplayIpAction::printState(){
  LOGD(DISP_T, "State: Ip Offset: %d", _ipOffset);
}

const char* DisplayIpAction::getActionName(){
  return "Display IP Action";
}

