#include "ImageCache.h"

ImageCache::ImageCache(uint32_t size, LogLevel logLevel):
    _logLevel(logLevel){
   // initialize register and map
    if (_logLevel != MUTE) printf("Initializing image map and register.\n" );

    // memory section is a bit larger than required, but exactly the size
    // we reserve on flash
    _pixelMap = (uint8_t *) malloc(size * sizeof(uint8_t));
    if (_pixelMap == 0){
      printf("Error. Cannot allocate pixelMap with size %d.\n",
        N_FRAMES * N_PIXELS * 3);
    }

    _pixelRegister = (rgbVal **) malloc(N_REGISTERS * sizeof(rgbVal*));
    for (uint8_t i=0; i<N_REGISTERS; i++){
        _pixelRegister[i] = (rgbVal *) malloc(N_REGISTERS * sizeof(rgbVal));
    }
}

rgbVal* ImageCache::getRegister(uint8_t i){
    return _pixelRegister[i];
}