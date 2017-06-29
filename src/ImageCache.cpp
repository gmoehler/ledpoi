#include "ImageCache.h"

ImageCache::ImageCache(uint32_t size, LogLevel logLevel):
    _logLevel(logLevel){
   // initialize register and map
    if (_logLevel != MUTE) printf("Initializing image map and register.\n" );

    // memory section is a bit larger than required, but exactly the size
    // we reserve on flash
    _pixelMap = (uint8_t *) calloc(size, sizeof(uint8_t));
    if (_pixelMap == 0){
      printf("Error. Cannot allocate pixelMap with size %d.\n",
        N_FRAMES * N_PIXELS * 3);
    }

    _pixelRegister = (rgbVal **) malloc(N_REGISTERS * sizeof(rgbVal*));
    for (uint8_t i=0; i<N_REGISTERS; i++){
        _pixelRegister[i] = (rgbVal *) malloc(N_REGISTERS * sizeof(rgbVal));
        _clearRegister(i);
    }
}

rgbVal* ImageCache::getRegister(uint8_t i){
    return _pixelRegister[i];
}

void ImageCache::_fillRegister(uint8_t registerId, rgbVal rgb){
  if (registerId +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId);
    return;
  }
  for (int i = 0; i < N_PIXELS; i++) {
    _pixelRegister[registerId][i] = rgb;
  }
}

void ImageCache::_clearRegister(uint8_t registerId) {
  rgbVal black = makeRGBVal(0,0,0);
  _fillRegister(registerId, black);
}
/*
void ImageCache::_fillMap(rgbVal rgb){
  for (int f=0; f<N_FRAMES; f++){
    for (int p=0; p<N_PIXELS; p++){
      _setPixel(f, p, rgb);
    }
  }
}*/

