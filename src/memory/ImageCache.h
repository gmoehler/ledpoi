#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H

#include "ledpoi.h"
#include "ledpoi_utils.h"

#ifndef WITHIN_UNITTEST
  #include "memory/PoiFlashMemory.h"
#else
  #include "mock_PoiFlashMemory.h"
#endif

/**
 * Holds the a chache for the image of the current scene and 2 registers
 **/

class ImageCache
{
public:
  ImageCache(uint8_t pixels, uint16_t frames, uint32_t size, uint8_t registers);
  ~ImageCache();
  void printInfo();
  uint8_t* getRawImageData();

  // TODO: once being used, move register implementation to PixelFrame 
  rgbVal* getRegister(uint8_t i);
  void printRegister(uint8_t i, esp_log_level_t level);
  void fillRegister(uint8_t registerId, rgbVal rgb, uint8_t nLeds=N_PIXELS);
  void clearRegister(uint8_t registerId); // fill with black
  void shiftRegister(uint8_t registerId1, uint8_t shiftRegisterLength, bool cyclic=false);

  void setPixel(uint8_t frame_idx, uint8_t pixel_idx, Color color);
  void setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b);
  void setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal getPixel(uint8_t frame_idx, uint8_t pixel_idx);

  void clearImageMap();
  void fillImageMap(rgbVal rgb);
  void printImageFrame(uint8_t frame_idx, esp_log_level_t level);
  String pixelToString(uint8_t frame_idx, uint8_t pixel_idx);
  void copyFrameToRegister(PixelFrame* regFrame, uint8_t frame_idx, float factor);
  void copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor=1);
  void copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor=1);

private:
  uint8_t _nPixels;
  uint16_t _nFrames;
  uint8_t _nRegisters;

  // data stores
  // after each action the last frame is stored in _pixelRegister[0]
  rgbVal **_pixelRegister;
  uint8_t *_pixelMap;
};



#endif