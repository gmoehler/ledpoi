#include "SelftestHelper.h"

SelftestHelper::SelftestHelper()
{ 
	_red = makeRGBVal(254, 0, 0);
	_green = makeRGBVal(0, 254, 0);
	_expectedNumFrames = 4 + N_FRAMES * 2;
	_expectedRuntime = 4 * LONG_DELAY+ N_FRAMES * 2 * SHORT_DELAY;
}

bool SelftestHelper::_sendToDispatch(RawPoiCommand rawCommand){
	PoiCommand cmd(rawCommand);
	return sendToDispatch(cmd, SELF_H);
}

uint32_t SelftestHelper::getExpectedNumFrames() {
	return _expectedNumFrames;
}

uint16_t SelftestHelper::getExpectedRuntime() {
	return _expectedRuntime;
}

// generate a color for a pixel
rgbVal SelftestHelper::_getTestPixelColor(uint8_t i) {
	return makeRGBVal(i,0,254-i);
}

// generate an index for a pixel
uint8_t SelftestHelper::_getTestPixelIndex(uint8_t i) {
	return i % N_PIXELS;
}

// generate an image with max size
void SelftestHelper::sendImage() {
  	_sendToDispatch({HEAD_SCENE,  N_SCENES-1,  0,  0,  0,  0});
	
	for (uint8_t i=0; i<N_FRAMES; i++) {
		rgbVal c = _getTestPixelColor(i);
		uint8_t idx = _getTestPixelIndex(i);
		_sendToDispatch({idx,  i,  0,  c.r,  c.g,  c.b});
	}
	
	_sendToDispatch({TAIL_SCENE,  0,  0,  0,  0,  0});
	_sendToDispatch({SAVE_SCENE,  0,  0,  0,  0,  0});
}

void SelftestHelper::sendProgram() {
	
	_sendToDispatch({HEAD_PROG,  0,  0,  0,  0,  0});
	
	// scene 0 first to wipe out mem
	_sendToDispatch({LOAD_SCENE,  0,  0,  0,  0,  0});
	_sendToDispatch({LOAD_SCENE,  N_SCENES-1,  0,  0,  0,  0});
	
	// twice red-green
	_sendToDispatch({LOOP_START,  42,  0,  0,  0,  2});
	_sendToDispatch({SHOW_RGB, 254,  0,  0,  0,  LONG_DELAY});
	_sendToDispatch({SHOW_RGB,  0, 254,  0,  0,  LONG_DELAY});
	_sendToDispatch({LOOP_END, 42,  0,  0,  0,  0});
	
	// twice complete image
	_sendToDispatch({PLAY_FRAMES,  0,  N_FRAMES-1,  2,  0,  SHORT_DELAY});
	
	_sendToDispatch({TAIL_PROG,  0,  0,  0,  0,  0});
	_sendToDispatch({SAVE_PROG,  0,  0,  0,  0,  0});
}

void SelftestHelper::startProgram() {
	_sendToDispatch({START_PROG,  0,  0,  0,  0,  0});
}

bool SelftestHelper::_isRgbEqual(rgbVal v1, rgbVal v2) {
	return (v1.r == v2.r && v1.g == v2.g && v1.b == v2.b);
}

// simply check, no notion of test being ready
bool SelftestHelper::validateFrame(PixelFrame& rframe, uint16_t idx) {

	LOGD(SELF_H, "Validating display frame %d", idx);
	
	if (idx > _expectedNumFrames - 1) {
		LOGD(SELF_H, "Index (%d) exceeds number of test frames (%d).",
			idx, _expectedNumFrames);
		return false;
	}

	// 4 SHOW_RGB commands
	if (idx<4) {
		rgbVal expectedColor = idx%2 == 0 ? 
			_red : _green;

		for (uint8_t i=0; i<N_PIXELS; i++) {
			if (idx%2 == 0){
				if (!_isRgbEqual(rframe.pixel[i], expectedColor)) {
				LOGD(SELF_H, "Wrong color (%d %d %d instead of %d %d %d) at display frame %d",
					rframe.pixel[i].r, rframe.pixel[i].g, rframe.pixel[i].b, 
					expectedColor.r, expectedColor.g, expectedColor.b, idx);
				return false;
				}
			}
		}
	}
	// image frames
	else {
		uint8_t frameIdx = idx-4 >= N_FRAMES ? idx-4 - N_FRAMES : idx-4;
		rgbVal expectedColor = _getTestPixelColor(frameIdx);
		uint8_t i = _getTestPixelIndex(frameIdx);
		if (!_isRgbEqual(rframe.pixel[i], expectedColor)) {
			LOGD(SELF_H, "Wrong color (%d %d %d instead of %d %d %d) at display frame %d, image frame %d, pixel index %d",
				rframe.pixel[i].r, rframe.pixel[i].g, rframe.pixel[i].b, 
				expectedColor.r, expectedColor.g, expectedColor.b, 
				idx, frameIdx, i);
			return false;
		}
	}
	return true;
}
	
