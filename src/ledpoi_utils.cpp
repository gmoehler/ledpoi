#include "ledpoi_utils.h"

rgbVal makeRGBValue(uint8_t *rgb_array){
  return makeRGBVal(rgb_array[0], rgb_array[1], rgb_array[2]);
}

rgbVal makeRGBValue(Color color, uint8_t brightness){
  rgbVal rgb = makeRGBVal(0,0,0);
  uint8_t b = brightness;
  switch (color){
  	case WHITE:
      rgb = makeRGBVal(b,b,b);
      break;

  	case BLACK:
      rgb = makeRGBVal(0,0,0);
      break;

      case RED:
      rgb = makeRGBVal(b,0,0);
      break;

      case GREEN:
      rgb = makeRGBVal(0,b,0);
      break;

      case BLUE:
      rgb = makeRGBVal(0,0,b);
      break;

      case YELLOW:
      rgb = makeRGBVal(b,b,0);
      break;

      case LILA:
      rgb = makeRGBVal(b,0,b);
      break;

      case CYAN:
      rgb = makeRGBVal(0,b,b);
      break;

      case PALE_WHITE:
      rgb = makeRGBVal(16,16,16);
      break;

      default:
      //RAINBOW is not handled here -> black
      break;
      }
      return rgb;
}

rgbVal fadeColor(Color color, float factor){
  uint8_t brightness = constrain(factor * 255, 0, 254);
  return makeRGBValue(color, brightness);
}

// stuff needed for demo setup

void fillFrame(PixelFrame* pFrame, uint8_t idx, uint16_t delay, uint8_t r, uint8_t g, uint8_t b){
  pFrame->idx = idx;
  pFrame->delay = delay;
  for(uint8_t i=0; i<N_PIXELS; i++) {
    pFrame->pixel[i] = makeRGBVal(r,g,b);
  } 
}

// shifts values from a position to one higher position ending at position shiftRegisterLength
// if cyclic is true, then value at shiftRegisterLength is shifted back to position 0
void shiftPixelframe(PixelFrame *pFrame, uint8_t shiftRegisterLength, bool cyclic) {
  rgbVal valLast = cyclic ? pFrame->pixel[shiftRegisterLength-1] : makeRGBValue(BLACK, 0);
  for (int j=shiftRegisterLength-1; j>0; j--){
    pFrame->pixel[j] = pFrame->pixel[j-1];
  }
  pFrame->pixel[0] = valLast;
}


const char* _getQueueName(QueueType q) {
	switch(q) {
    case SPIFFS_QUEUE:
    return "spiffsQueue";
    
		case MEMORY_QUEUE:
		return "memoryQueue";
		
		case PLAYER_QUEUE:
		return "playerQueue";
		
		case WIFI_CONTROL_QUEUE:
		return "wifiControlQueue";
		
		case DISPLAY_QUEUE:
		return "displayQueue";
		
		case PROGRAM_QUEUE:
		return "programQueue";

    case DISPATCH_QUEUE:
		return "dispaytchQueue";
	}
	return "dispatchQueue";
}

xQueueHandle _getQueue(QueueType q) {
	switch(q) {

		case SPIFFS_QUEUE:
		return spiffsQueue;

		case MEMORY_QUEUE:
		return memoryQueue;
		
		case PLAYER_QUEUE:
		return playerQueue;
		
		case WIFI_CONTROL_QUEUE:
		return wifiControlQueue;
		
		case DISPLAY_QUEUE:
		return displayQueue;
		
		case PROGRAM_QUEUE:
		return programQueue;

    case DISPATCH_QUEUE:
    return dispatchQueue;
	}

	return dispatchQueue;
}

bool sendToQueue(QueueType q, PoiCommand& cmd, const char* logtag, bool debugPrintout){
  
  if (debugPrintout) {
	  LOGD(logtag, "Sending to queue %s:  %s", _getQueueName(q), cmd.toString().c_str());
  }
  else {
    LOGV(logtag, "Sending to queue %s:  %s", _getQueueName(q), cmd.toString().c_str());
  }
  
  xQueueHandle toQueue = _getQueue(q);
	RawPoiCommand rawCmd = cmd.getRawPoiCommand();

 	if (xQueueSendToBack(toQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
    	LOGE(logtag, "Could not add  command to queue %s.",  _getQueueName(q));
      return false;
	}
  return true;
}

bool sendToDispatch(PoiCommand& cmd, const char* logtag, bool debugPrintout) {
	return sendToQueue(DISPATCH_QUEUE, cmd, logtag, debugPrintout);
}

bool sendRawToDispatch(RawPoiCommand rawCmd, const char* logtag, bool debugPrintout) {
  PoiCommand cmd(rawCmd);
	return sendToQueue(DISPATCH_QUEUE, cmd, logtag, debugPrintout);
}

void sendFrameToDisplay(PixelFrame* pFrame, uint32_t timeout) {
  if (xQueueSendToBack(displayQueue, pFrame, timeout/portTICK_PERIOD_MS) != pdTRUE){
    LOGE(PLAY_T, "Could not add frame %d to queue.", pFrame->idx);
  }
}

void clearQueue(QueueType q) {
  xQueueHandle queue = _getQueue(q);
  xQueueReset(queue);
}

uint8_t getQueueSize(QueueType q) {
  xQueueHandle queue = _getQueue(q);
  return uxQueueMessagesWaiting(queue);
}
