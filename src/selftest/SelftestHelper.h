#ifndef SELFTEST_HELPER_H
#define SELFTEST_HELPER_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"

#define SHORT_DELAY 10
#define LONG_DELAY 250

class SelftestHelper
{
public:
	SelftestHelper();

	void sendImage();
	void sendProgram();
	void startProgram();
	bool validateFrame(PixelFrame& rframe, uint16_t idx);
	uint32_t getExpectedNumFrames();
	uint16_t getExpectedRuntime();
private:
	uint16_t _expectedNumFrames;
	uint32_t _expectedRuntime;
	rgbVal _red;
	rgbVal _green;
	
	bool _sendToDispatch(RawPoiCommand rawCommand);
	rgbVal _getTestPixelColor(uint8_t i);
	uint8_t _getTestPixelIndex(uint8_t i);
	bool _isRgbEqual(rgbVal v1, rgbVal v2);
	
};

#endif