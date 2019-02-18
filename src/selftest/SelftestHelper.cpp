#include "SelftestHelper.h"

SelftestHelper::SelftestHelper()
{ 
	_red = makeRGBVal(254, 0, 0);
	_green = makeRGBVal(0, 254, 0);
	_expectedNumFrames = 4;
	_expectedRuntime = 4 * LONG_DELAY;
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
	return true;
}
	
