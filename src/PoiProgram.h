#ifndef POI_PROGRAM_H
#define POI_PROGRAM_H

#include <Arduino.h>
#include <ws2812.h>

#define N_SCENES 1
#define N_FRAMES 200
#define N_PIXELS 60


class PoiProgram
{
public:
  void setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx);

private:
  rgbVal _pixelMap[N_SCENES][N_FRAMES][N_PIXELS];
  //rgbVal _pixels[N_PIXELS];
};


#endif
