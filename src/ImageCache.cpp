#include "ImageCache.h"

ImageCache::ImageCache(uint32_t size, LogLevel logLevel):
    _logLevel(logLevel){
   // initialize register and map
    if (_logLevel != MUTE) printf("Initializing image map and register.\n" );

    // memory section is a bit larger than required, but exactly the size
    // we reserve on flash
    _pixelMap = (uint8_t *) calloc(size, sizeof(uint8_t));
    if (_pixelMap == 0){
      printf("Error. Cannot allocate pixelMap with size %d.\n", 3);
    }

    _pixelRegister = (rgbVal **) malloc(N_REGISTERS * sizeof(rgbVal*));
    for (uint8_t i=0; i<N_REGISTERS; i++){
        _pixelRegister[i] = (rgbVal *) malloc(N_FRAMES * sizeof(rgbVal));
        _clearRegister(i);
    }
}

uint8_t* ImageCache::getRawImageData(){
    return _pixelMap;
}

/**********************
  * Utility functions *
  *********************/
// version that takes an rgb array
rgbVal ImageCache::_makeRGBValue(uint8_t *rgb_array){
  return makeRGBVal(rgb_array[0], rgb_array[1], rgb_array[2]);
}
rgbVal ImageCache::_makeRGBValue(Color color, uint8_t brightness){
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

      default:
      //RAINBOW is not handled here -> black
      break;
      }
      return rgb;
}

/************************************
 * Functions dealing with registers *
 ************************************/ 

rgbVal* ImageCache::getRegister(uint8_t i){
    return _pixelRegister[i];
}

void ImageCache::printRegister(uint8_t registerId){
   for (int i = 0; i < N_PIXELS; i++) {
    printf("%d: %d %d %d\n", i, 
        _pixelRegister[registerId][i].r,
        _pixelRegister[registerId][i].g,
        _pixelRegister[registerId][i].b);
  }  
}

void ImageCache::_fillRegister(uint8_t registerId, rgbVal rgb, uint8_t nLeds){
  if (registerId +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId);
    return;
  }
  for (int i = 0; i < constrain(nLeds, 0, N_PIXELS); i++) {
    _pixelRegister[registerId][i] = rgb;
  }
}

void ImageCache::_clearRegister(uint8_t registerId) {
  rgbVal black = makeRGBVal(0,0,0);
  _fillRegister(registerId, black);
}

// shifts values from a position to one higher position ending at position shiftRegisterLength
// if cyclic is true, then value at shiftRegisterLength is shifted back to position 0
void ImageCache::_shiftRegister(uint8_t registerId1, uint8_t shiftRegisterLength, bool cyclic) {
  rgbVal valLast = _pixelRegister[registerId1][shiftRegisterLength-1];
  for (int j=shiftRegisterLength-1; j>0; j--){
    _pixelRegister[registerId1][j] = _pixelRegister[registerId1][j-1];
  }
  _pixelRegister[registerId1][0] = cyclic ? valLast : _makeRGBValue(BLACK, 0);
}


/****************************************
 * Functions dealing with the image map *
 ****************************************/ 

void ImageCache::_setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _setPixel(frame_idx, pixel_idx,  pixel.r, pixel.g, pixel.b);
}

void ImageCache::_setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b){
  if (frame_idx >= N_FRAMES || pixel_idx >= N_PIXELS){
    printf("Error. Pixel index exceeds boundaries for setPixel:  %d >= %d || %d >= %d ",
      frame_idx, N_FRAMES, pixel_idx, N_PIXELS);
    return;
  }
  _pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3]     = r;
  _pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3 + 1] = g;
  _pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3 + 2] = b;
}

rgbVal ImageCache::_getPixel(uint8_t frame_idx, uint8_t pixel_idx) {
  if (frame_idx >= N_FRAMES || pixel_idx >= N_PIXELS){
    printf("Error. Pixel index exceeds boundaries for getPixel: %d >= %d || %d >= %d ",
      frame_idx, N_FRAMES, pixel_idx, N_PIXELS);
    return makeRGBVal(0, 0, 0);
  }
  return _makeRGBValue(&_pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3] );
}

void ImageCache::_fillImageMap(rgbVal rgb){
  for (int f=0; f<N_FRAMES; f++){
    for (int p=0; p<N_PIXELS; p++){
      _setPixel(f, p, rgb);
    }
  }
}

void ImageCache ::clearImageMap(){
	rgbVal black = makeRGBVal(0,0,0);
    _fillImageMap(black);
}

/**************************
 * Functions copying data *
 **************************/

void ImageCache::_copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor){
  if (frame_idx + 1 > N_FRAMES){
    printf("Error. Cannot copy frame %d (> %d) to regsiter %d\n", frame_idx, N_FRAMES, registerId);
    return;
  }
  if (registerId +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId);
    return;
  }

  //printf("Copying frame to register:\n");
  for (int i = 0; i < N_PIXELS; i++) {
    rgbVal rgb = _getPixel(frame_idx, i);
    if (factor == 1){
      _pixelRegister[registerId][i] = rgb;
    }
    else {
      _pixelRegister[registerId][i] =
            makeRGBVal(  ((double)rgb.r) * factor,
                         ((double)rgb.g) * factor,
                         ((double)rgb.b) * factor );
    }
  }
}

void ImageCache::_copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor){
  if (registerId1 +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId1);
    return;
  }
  if (registerId2 +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId2);
    return;
  }

  for (int i = 0; i < N_PIXELS; i++) {
    rgbVal rgb = _pixelRegister[registerId1][i];
    if (factor == 1){
      _pixelRegister[registerId2][i] = rgb;
    }
    else {
      _pixelRegister[registerId2][i] =
            makeRGBVal(  ((double)rgb.r) * factor,
                         ((double)rgb.g) * factor,
                         ((double)rgb.b) * factor );
    }
  }
}
