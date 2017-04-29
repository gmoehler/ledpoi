#include "PoiProgramRunner.h"

void PoiProgramRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)] = pixel;
}

rgbVal PoiProgramRunner::getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx) {
  return _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)];
}

void PoiProgramRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){
  printf("Playing Scene: %d frames: [%d,%d] delay: %d loops:%d \n", scene, startFrame, endFrame, speed, loops);
  _currentProgram = PLAY_SCENE;
  _scene = scene;
  _startFrame = startFrame;
  _endFrame = endFrame;
  _delay = speed;
  _loops = loops;

  _currentFrame = _startFrame;

/*  for (uint8_t runner=0;runner<loops;runner++){
    for (int i=frameStart;i<frameEnd;i++){
      rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][i];
      ws2812_setColors(N_PIXELS, pixels);  // LEDs updaten
      delay(speed);
    }
  }*/
}



void PoiProgramRunner::showFrame(uint8_t scene, uint8_t frame){
  //printf("Showing frame: scene %d frame \n", scene, frame);
  rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][constrain(frame,0,N_FRAMES-1)];
  ws2812_setColors(N_PIXELS, pixels);
}

void PoiProgramRunner::displayOff() {
  for (int i = 0; i < N_PIXELS; i++) {
    _pixels[i] = makeRGBVal(0, 0, 0);
  }
  ws2812_setColors(N_PIXELS, _pixels);
}

void PoiProgramRunner::displayTest() {
  for (int i = 0; i < N_PIXELS; i++) {
    _pixels[i] = makeRGBVal(33, 33, 0);
  }
  ws2812_setColors(1, _pixels);
  //ws2812_setColors(NUM_PIXELS, _pixels);
}


void PoiProgramRunner::loop(){


  switch(_currentProgram){

    case PLAY_SCENE:
    //  printf("Playing scene: %d frame: %d\n", _scene, _currentFrame);
      displayTest();
/*      Serial.print("Playing scene at frame ");
      Serial.println(_currentFrame);
      showFrame(_scene, _currentFrame++);
      if (_currentFrame > _endFrame){
        _currentFrame = _startFrame;
      }*/
  }

}
