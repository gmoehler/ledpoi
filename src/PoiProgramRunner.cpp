#include "PoiProgramRunner.h"

PoiProgramRunner::PoiProgramRunner(uint32_t interruptTimeMs) :
    _currentProgram(NO_PROGRAM),
    _scene(0), _frame(0), _startFrame(0), _endFrame(0),
    _delayMs(5), _numLoops(1),
    _interruptTimeMs(interruptTimeMs),
    _currentFrame(0), _currentLoop(0)
    {}

void PoiProgramRunner::setup(){
  // Create semaphore to inform us when the timer has fired
  _timerSemaphore = xSemaphoreCreateBinary();
}

void PoiProgramRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)] = pixel;
}

rgbVal PoiProgramRunner::getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx) {
  return _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)];
}

void PoiProgramRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops, OperationMode mode){
  printf("Playing Scene: %d frames: [%d,%d] delay: %d loops:%d \n", scene, startFrame, endFrame, speed, loops);
  if (mode == ASYNC){
    _currentProgram = PLAY_SCENE;
    _scene = scene;
    _startFrame = startFrame;
    _endFrame = endFrame;
    _delayMs = speed;
    _numLoops = loops;

    _currentFrame = _startFrame;
    _currentLoop = 0;
  }
  else {
    for (uint8_t runner=0;runner<loops;runner++){
      for (int i=startFrame;i<endFrame;i++){
        rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][i];
        ws2812_setColors(N_PIXELS, pixels);  // LEDs updaten
        delay(speed);
      }
    }
  }
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

void PoiProgramRunner::statusIO() {
  _pixels[0] = makeRGBVal(0, 33, 0);
  ws2812_setColors(1, _pixels);
}

void PoiProgramRunner::statusNIO() {
  _pixels[0] = makeRGBVal(33, 0, 0);
  ws2812_setColors(1, _pixels);
}

void PoiProgramRunner::showCurrent(){
  ws2812_setColors(N_PIXELS, _pixels);  // update LEDs
}

void PoiProgramRunner::fadeToBlack(){
  //TODO
}

void PoiProgramRunner::setProgram(PoiProgram prog_id){
  _currentProgram = prog_id;
  // more TODO
}

void PoiProgramRunner::defineProgram(PoiProgram prog_id, uint8_t cmd0, uint8_t cmd1, uint8_t cmd2){
    // TODO
}

void PoiProgramRunner::startProg(){
  //TODO
}

void PoiProgramRunner::pauseProg(){
  // TODO
}

void PoiProgramRunner::saveProg(){
  // TODO
}

void PoiProgramRunner::resetProg(PoiProgram prog_id) {
  _currentProgram = prog_id;
  // more TODO
}

uint32_t PoiProgramRunner::getDelay(){
  return _delayMs;
}

// no printf in interrupt!
void PoiProgramRunner::onInterrupt(){

  switch(_currentProgram){
    case PLAY_SCENE:
    //Serial.println("play scene  - Next frame");
    // Give a semaphore that we can check in the loop
    xSemaphoreGiveFromISR(_timerSemaphore, NULL);
    break;

    default:
    break;
  }
}

void PoiProgramRunner::loop(){

  if (xSemaphoreTake(_timerSemaphore, 0) == pdTRUE){
    switch(_currentProgram){

    case PLAY_SCENE:
    _currentFrame++;
    if (_currentFrame > _endFrame){
      _currentLoop++;
      if (_currentLoop > _numLoops){
        _currentProgram = NO_PROGRAM;
        printf("End of program\n");
        return;
      }
      _currentFrame = _startFrame;
    }
    printf("Playing scene: %d frame: %d\n", _scene, _currentFrame);
    showFrame(_scene, _currentFrame);
  }
}

}
