#include "ImageCache.h"

ImageCache::ImageCache(uint8_t pixels, uint16_t frames, uint32_t size, uint8_t registers):
    _nPixels(pixels), _nFrames(frames), _nRegisters(registers){
   // initialize register and map
    LOGI(ICACHE, "Initializing image map (%d x %d) and %d registers.",
       _nFrames, _nPixels, _nRegisters);
    if (size < _nFrames * _nPixels * 3){
      LOGE(ICACHE, "Error. Requested size of ImageCache is too small: %d < %d.", size, _nFrames * _nPixels * 3);
    }

    // memory section is a bit larger than required, but exactly the size
    // which we reserve on flash
    _pixelMap = (uint8_t *) calloc(size, sizeof(uint8_t));
    if (_pixelMap == 0){
      LOGE(ICACHE, "Error. Cannot allocate pixelMap with size %d.", 3);
    }

    if (_nRegisters > 0) {
      _pixelRegister = (rgbVal **) malloc(_nRegisters * sizeof(rgbVal*));
      for (uint8_t i=0; i<_nRegisters; i++){
          _pixelRegister[i] = (rgbVal *) malloc(_nPixels * sizeof(rgbVal));
          clearRegister(i);
      }
    }
}

ImageCache::~ImageCache(){
  LOGI(ICACHE, "!Freeing ImageCache now!");
  free(_pixelMap);
  for (uint8_t i=0; i<_nRegisters; i++){
    free(_pixelRegister[i]);
  }
  free(_pixelRegister);
}

void ImageCache::printInfo(){
  LOGI(ICACHE, "Image map: %d frames with %d pixels and %d registers.",
       _nFrames, _nPixels, _nRegisters);
}

uint8_t* ImageCache::getRawImageData(){
    return _pixelMap;
}

/************************************
 * Functions dealing with registers *
 ************************************/ 

rgbVal* ImageCache::getRegister(uint8_t i){
    return _pixelRegister[i];
}

void ImageCache::printRegister(uint8_t registerId, esp_log_level_t level){
  for (int i = 0; i < _nPixels; i++) {
    if (level == ESP_LOG_VERBOSE) {
     LOGV(ICACHE, "%d: %d %d %d", i, 
        _pixelRegister[registerId][i].r,
        _pixelRegister[registerId][i].g,
        _pixelRegister[registerId][i].b);
     }
    else {
      LOGD(ICACHE, "%d: %d %d %d", i, 
        _pixelRegister[registerId][i].r,
        _pixelRegister[registerId][i].g,
        _pixelRegister[registerId][i].b);
     }
  }  
}

void ImageCache::fillRegister(uint8_t registerId, rgbVal rgb, uint8_t nLeds){
  if (registerId +1 > _nRegisters){
   LOGE(ICACHE, "Error. Register %d does not exist", registerId);
    return;
  }
  for (int i = 0; i < constrain(nLeds, 0, _nPixels); i++) {
    _pixelRegister[registerId][i] = rgb;
  }
}

void ImageCache::clearRegister(uint8_t registerId) {
  rgbVal black = makeRGBVal(0,0,0);
  fillRegister(registerId, black);
}

// shifts values from a position to one higher position ending at position shiftRegisterLength
// if cyclic is true, then value at shiftRegisterLength is shifted back to position 0
void ImageCache::shiftRegister(uint8_t registerId1, uint8_t shiftRegisterLength, bool cyclic) {
  rgbVal valLast = _pixelRegister[registerId1][shiftRegisterLength-1];
  for (int j=shiftRegisterLength-1; j>0; j--){
    _pixelRegister[registerId1][j] = _pixelRegister[registerId1][j-1];
  }
  _pixelRegister[registerId1][0] = cyclic ? valLast : makeRGBValue(BLACK, 0);
}


/****************************************
 * Functions dealing with the image map *
 ****************************************/ 

void ImageCache::setPixel(uint8_t frame_idx, uint8_t pixel_idx, Color color){
  rgbVal pixel = makeRGBValue(color);
  setPixel(frame_idx, pixel_idx,  pixel.r, pixel.g, pixel.b);
}


void ImageCache::setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  setPixel(frame_idx, pixel_idx,  pixel.r, pixel.g, pixel.b);
}

void ImageCache::setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b){
  if (frame_idx >= _nFrames || pixel_idx >= _nPixels){
    LOGE(ICACHE, "Error. Pixel index exceeds boundaries for setPixel:  %d >= %d || %d >= %d ",
      frame_idx, _nFrames, pixel_idx, _nPixels);
    return;
  }
  _pixelMap[frame_idx * _nPixels * 3 + pixel_idx * 3]     = r;
  _pixelMap[frame_idx * _nPixels * 3 + pixel_idx * 3 + 1] = g;
  _pixelMap[frame_idx * _nPixels * 3 + pixel_idx * 3 + 2] = b;
}

rgbVal ImageCache::getPixel(uint8_t frame_idx, uint8_t pixel_idx) {
  if (frame_idx >= _nFrames || pixel_idx >= _nPixels){
    LOGE(ICACHE, "Error. Pixel index exceeds boundaries for getPixel: %d >= %d || %d >= %d ",
      frame_idx, _nFrames, pixel_idx, _nPixels);
    return makeRGBVal(0, 0, 0);
  }
  int index = frame_idx * _nPixels * 3 + pixel_idx * 3;
  return makeRGBValue(&_pixelMap[index] );
}

void ImageCache::printImageFrame(uint8_t frame_idx, esp_log_level_t level){
  for (int i = 0; i < _nPixels; i++) {
    if (level == ESP_LOG_VERBOSE){
      LOGV(ICACHE, "%d: %s", i, pixelToString(frame_idx, i).c_str());
    }
    else {
      LOGD(ICACHE, "%d: %s", i, pixelToString(frame_idx, i).c_str());
    }
  }  
}

String ImageCache::pixelToString(uint8_t frame_idx, uint8_t pixel_idx){
  char buffer[12];
  rgbVal px = getPixel(frame_idx, pixel_idx);
  snprintf(buffer, 12, "%3d %3d %3d", px.r, px.g, px.b);
  return String(buffer);
}

void ImageCache::fillImageMap(rgbVal rgb){
  for (int f=0; f<_nFrames; f++){
    for (int p=0; p<_nPixels; p++){
      setPixel(f, p, rgb);
    }
  }
}

void ImageCache ::clearImageMap(){
	rgbVal black = makeRGBVal(0,0,0);
    fillImageMap(black);
}

/**************************
 * Functions copying data *
 **************************/

void ImageCache::copyFrameToRegister(PixelFrame* regFrame, uint8_t frame_idx, float factor){
  if (frame_idx + 1 > _nFrames){
    LOGE(ICACHE, "Error. Cannot copy frame %d (> %d) to register frame", frame_idx, _nFrames);
    return;
  }
  for (int i = 0; i < _nPixels; i++) {
    rgbVal rgb = getPixel(frame_idx, i);
    if (factor == 1){
      regFrame->pixel[i] = rgb;
    }
    else {
      regFrame->pixel[i] =
            makeRGBVal(  ((double)rgb.r) * factor,
                         ((double)rgb.g) * factor,
                         ((double)rgb.b) * factor );
    }
  }
}

void ImageCache::copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor){
  if (frame_idx + 1 > _nFrames){
    LOGE(ICACHE, "Error. Cannot copy frame %d (> %d) to register %d", frame_idx, _nFrames, registerId);
    return;
  }
  if (registerId +1 > _nRegisters){
    LOGE(ICACHE, "Error. Register %d does not exist", registerId);
    return;
  }

  LOGD(ICACHE, "Copying frame to register");
  for (int i = 0; i < _nPixels; i++) {
    rgbVal rgb = getPixel(frame_idx, i);
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

void ImageCache::copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor){
  if (registerId1 +1 > _nRegisters){
    LOGE(ICACHE, "Error. Register %d does not exist", registerId1);
    return;
  }
  if (registerId2 +1 > _nRegisters){
    LOGE(ICACHE, "Error. Register %d does not exist", registerId2);
    return;
  }

  for (int i = 0; i < _nPixels; i++) {
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
