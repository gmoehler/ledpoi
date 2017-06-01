#include "PoiActionRunner.h"

PoiActionRunner::PoiActionRunner(PoiTimer& ptimer, LogLevel logLevel) :
  _currentAction(NO_PROGRAM), _currentSyncId(0), _currentScene(0),
  _ptimer(ptimer),
  _progHandler(_playHandler, _flashMemory, logLevel),
  _logLevel(logLevel)
{
    // initialize register and map
    if (_logLevel != MUTE) printf("Initializing image map and register.\n" );
    rgbVal black = makeRGBVal(0,0,0);
    for (int i= 0; i< N_REGISTERS; i++){
      _fillRegister(i, black);
    }

    // memory section is a bit larger than required, but exactly the size
    // we reserve on flash
    _pixelMap = (uint8_t *) malloc(_flashMemory.getSizeOfImageSection());
    if (_pixelMap == 0){
      printf("Error. Cannot allocate pixelMap with size %d.\n",
        N_FRAMES * N_PIXELS * 3);
    }
    _fillMap(black); // not sure whether required...
}

void PoiActionRunner::clearImageMap(){
	rgbVal black = makeRGBVal(0,0,0);
    _fillMap(black);
}

void PoiActionRunner::setup(){
  // Create semaphore to inform us when the timer has fired
  _timerSemaphore = xSemaphoreCreateBinary();
  _flashMemory.setup(_logLevel);
  _progHandler.setup();
}

/**********************
  * Utility functions *
  *********************/

// version that takes an rgb array
rgbVal PoiActionRunner::_makeRGBValue(uint8_t *rgb_array){
  return makeRGBVal(rgb_array[0], rgb_array[1], rgb_array[2]);
}

rgbVal PoiActionRunner::_getPixel(uint8_t frame_idx, uint8_t pixel_idx) {
  if (frame_idx >= N_FRAMES || pixel_idx >= N_PIXELS){
    printf("Error. Pixel index exceeds boundaries for getPixel: %d >= %d || %d >= %d ",
      frame_idx, N_FRAMES, pixel_idx, N_PIXELS);
    return makeRGBVal(0, 0, 0);
  }
  return _makeRGBValue(&_pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3] );
}

void PoiActionRunner::_fillRegister(uint8_t registerId, rgbVal rgb){
  if (registerId +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId);
    return;
  }
  for (int i = 0; i < N_PIXELS; i++) {
    _pixelRegister[registerId][i] = rgb;
  }
}

void PoiActionRunner::_fillMap(rgbVal rgb){
  for (int f=0; f<N_FRAMES; f++){
    for (int p=0; p<N_PIXELS; p++){
      _setPixel(f, p, rgb);
    }
  }
}

void PoiActionRunner::_copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor){
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

void PoiActionRunner::_copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor){
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


void PoiActionRunner::_displayRegister(uint8_t registerId){
    ws2812_setColors(N_PIXELS, _pixelRegister[registerId]);
}

void PoiActionRunner::_displayFrame(uint8_t frame){
  //printf("Showing frame %d\n", frame);
  _copyFrameToRegister(0, frame);
  _displayRegister(0);
  // TODO : directly get it from image - needs rework of code
}

/****************************
  * External action methods *
  ***************************/

void PoiActionRunner::resetFlash(){
    _flashMemory.eraseNvsFlashPartition();
    _flashMemory.eraseImages();
    _flashMemory.setup(_logLevel);
}

void PoiActionRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx,
    uint8_t r, uint8_t g, uint8_t b){
  // TODO: possibly handle cases in which scene_idx changes
  _currentScene = scene_idx;
  _setPixel(frame_idx, pixel_idx, r, g, b);
}

void PoiActionRunner::_setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _setPixel(frame_idx, pixel_idx,  pixel.r, pixel.g, pixel.b);
}

void PoiActionRunner::_setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b){
  if (frame_idx >= N_FRAMES || pixel_idx >= N_PIXELS){
    printf("Error. Pixel index exceeds boundaries for setPixel:  %d >= %d || %d >= %d ",
      frame_idx, N_FRAMES, pixel_idx, N_PIXELS);
    return;
  }
  _pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3]     = r;
  _pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3 + 1] = g;
  _pixelMap[frame_idx * N_PIXELS * 3 + pixel_idx * 3 + 2] = b;
}

void PoiActionRunner::saveScene(uint8_t scene){
  if (_logLevel != MUTE) printf("Saving image of scene %d to flash.\n", _currentScene);
  if (_flashMemory.saveImage(scene, _pixelMap)){
    _currentScene = scene;
    if (_logLevel != MUTE) printf("Image of scene %d saved to flash.\n", _currentScene);
  }
  else {
    printf("Error saving scene %d to flash.", _currentScene);
  }
}

void PoiActionRunner::_updateSceneFromFlash(uint8_t scene){
  if (scene != _currentScene){
    if (_flashMemory.loadImage(scene, _pixelMap)){
      _currentScene = scene;
      if (_logLevel != MUTE) printf("Scene %d loaded from Flash.\n", _currentScene);
    }
    else{
      printf("Error. Cannot load scene %d\n", scene);
    }
  }
}

void PoiActionRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){
  _currentAction = SHOW_STATIC_FRAME;
  _updateSceneFromFlash(scene);
  uint8_t timeout = (uint16_t)timeOutMSB *256 + timeOutLSB;
  if (_logLevel != MUTE)  printf("Play static frame: %d timeout: %d \n", frame, timeout);
  _copyFrameToRegister(0, frame);

  // play initial frame right away
  _ptimer.disable();
  _displayRegister(0);
  _ptimer.setIntervalAndEnable( timeout );
}

void PoiActionRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){
  _currentAction = PLAY_DIRECT;
  _updateSceneFromFlash(scene);
  _playHandler.init(startFrame, endFrame, speed, loops);
  if (_logLevel != MUTE) _playHandler.printInfo();

  // play initial frame right away
  _ptimer.disable();
  _displayFrame(_playHandler.getCurrentFrame());
  _ptimer.setIntervalAndEnable( _playHandler.getDelayMs() );
}

void PoiActionRunner::showStaticRgb(uint8_t r, uint8_t g, uint8_t b) {
  // directly "play" out of register
  _fillRegister(0, makeRGBVal(r,g,b));

  _currentAction = SHOW_STATIC_RGB;
  _ptimer.disable();
  _displayRegister(0);
}

void PoiActionRunner::displayOff() {
  showStaticRgb(0,0,0);
}

void PoiActionRunner::fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB){

  uint16_t fadeTime = (uint16_t)fadeMSB * 256 + fadeLSB;

  if (fadeTime < MIN_FADE_TIME){
  	displayOff();
      return;
  }

  _currentAction = FADE_TO_BLACK;
  _fadeHandler.init((uint16_t)fadeMSB * 256 + fadeLSB);
  if (_logLevel != MUTE) _fadeHandler.printInfo();

   // we take what is in register 0 and remember it in register 1
   // later we will copy pixels back using a factor on the rgb values
  _copyRegisterToRegister(0, 1);

  _ptimer.disable();
  _displayRegister(0);
  _ptimer.setIntervalAndEnable( _fadeHandler.getDelayMs() );
}

void PoiActionRunner::showCurrent(){
  _currentAction = SHOW_CURRENT_FRAME;
  _ptimer.disable();
  _displayRegister(0);
}

/****************************
  * Program related methods *
  ***************************/

void PoiActionRunner::addCmdToProgram(char cmd[7]){
  _progHandler.addCmdToProgram(cmd);
}

void PoiActionRunner::startProg(){
  _currentAction = PLAY_PROG;
  if (!_progHandler.checkProgram()){
    return;
  }
  _progHandler.init();
  if (_logLevel != MUTE) _progHandler.printInfo();

  // play initial frame right away
  _ptimer.disable();
  _displayFrame(_progHandler.getCurrentFrame());
  _ptimer.setIntervalAndEnable( _progHandler.getDelayMs() );
}

void PoiActionRunner::pauseProg(){
  _currentAction = PAUSE_PROG;
  if (_logLevel != MUTE) printf("Program paused.\n" );
}

void PoiActionRunner::jumptoSync(uint8_t syncId){
  if (syncId == 0){
    _currentSyncId++;
    _progHandler.syncNow(_currentSyncId);
  }
  else {
    _progHandler.syncNow(syncId);
  }
}

void PoiActionRunner::continueProg() {
  _currentAction = PLAY_PROG;
  if (_logLevel != MUTE) printf("Program continuing.\n" );
}

/*******************************
  * Interrupt & State handling *
  ******************************/

// no printf in interrupt!
void IRAM_ATTR PoiActionRunner::onInterrupt(){

  if (_currentAction != NO_PROGRAM){
    //Serial.println("play next frame");
    // Give a semaphore that we can check in the loop
    xSemaphoreGiveFromISR(_timerSemaphore, NULL);
  }
}

void PoiActionRunner::loop(){

  float factor = 1;

  if (xSemaphoreTake(_timerSemaphore, 0) == pdTRUE){
    switch(_currentAction){

      case PLAY_DIRECT:
      _playHandler.next();
      if (_logLevel == CHATTY) _playHandler.printState();
      if (_playHandler.isActive()){
        _displayFrame(_playHandler.getCurrentFrame());
      }
      else {
        _currentAction = NO_PROGRAM;
        // remember last frame in register 0
        _copyFrameToRegister(0, _playHandler.getCurrentFrame());
        if (_logLevel != MUTE) printf("End of program PLAY_DIRECT.\n");
      }
      break;

      case PLAY_PROG:
      if (!_progHandler.checkProgram()){
        _currentAction = NO_PROGRAM;
        return;
      }
      _progHandler.next();
      if (_progHandler.isActive()){
        if (_progHandler.hasDelayChanged()) {
          _ptimer.disable();
        }
        uint8_t scene = _progHandler.getCurrentScene();
        if (scene != _currentScene){
          _updateSceneFromFlash(scene);
          _currentScene = scene;
        }
        // finally display the frame
        _displayFrame(_progHandler.getCurrentFrame());
        if (_progHandler.hasDelayChanged()) {
          _ptimer.setIntervalAndEnable( _progHandler.getDelayMs() ); }
      }
      else {
        _currentAction = NO_PROGRAM;
        // remember last frame in register 0
        _copyFrameToRegister(0,_progHandler.getCurrentFrame());
        if (_logLevel != MUTE) printf("End of program PLAY_PROG.\n");
      }
      break;

      case SHOW_STATIC_FRAME:
      // reached timeout
      _currentAction = NO_PROGRAM;
      if (_logLevel != MUTE) printf("Timeout of SHOW_STATIC_FRAME reached.\n");
      break;

      case FADE_TO_BLACK:
      _fadeHandler.next();
      if (_logLevel == CHATTY)  _fadeHandler.printState();
      if (_fadeHandler.isActive()){
        // un-faded frame is in register 1
        _copyRegisterToRegister(1, 0, _fadeHandler.getCurrentFadeFactor());
        _displayRegister(0);
      }
      else {
        _currentAction = NO_PROGRAM;
        if (_logLevel != MUTE) printf("End of program FADE_TO_BLACK.\n");
      }
      break;

      case NO_PROGRAM:
      case PAUSE_PROG:
      // do nothing
      break;

      default:
      break;
    }
  }
}
