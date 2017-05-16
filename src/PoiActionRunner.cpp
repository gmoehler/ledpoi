#include "PoiActionRunner.h"

PoiActionRunner::PoiActionRunner(PoiTimer& ptimer, LogLevel logLevel) :
    _currentAction(NO_PROGRAM),
    _ptimer(ptimer),
    _logLevel(logLevel), _progHandler(_playHandler, logLevel)
    {
      // initialize map and register
      if (_logLevel != MUTE) printf("Initializing image map and register.\n" );
      rgbVal black = makeRGBVal(0,0,0);
      for (int i= 0; i< N_REGISTERS; i++){
        _fillRegister(i, black);
      }
      _fillMap(black);
    }

void PoiActionRunner::setup(){
  // Create semaphore to inform us when the timer has fired
  _timerSemaphore = xSemaphoreCreateBinary();
}

/**********************
  * Utility functions *
  *********************/

rgbVal PoiActionRunner::_getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx) {
  return _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)];
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
  for (int s=0; s<N_SCENES; s++){
    for (int f=0; f<N_FRAMES; f++){
      for (int p=0; p<N_PIXELS; p++){
        setPixel(s, f, s, rgb);
      }
    }
  }
}

void PoiActionRunner::_copyFrameToRegister(uint8_t registerId, uint8_t scene_idx, uint8_t frame_idx, float factor){
  if (registerId +1 > N_REGISTERS){
    printf("Error. Register %d does not exist\n", registerId);
    return;
  }

  for (int i = 0; i < N_PIXELS; i++) {
    rgbVal rgb = _getPixel(scene_idx, frame_idx, i);
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

void PoiActionRunner::_displayFrame(uint8_t scene, uint8_t frame){
  //printf("Showing frame: scene %d frame \n", scene, frame);
  rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][constrain(frame,0,N_FRAMES-1)];
  ws2812_setColors(N_PIXELS, pixels);
}

/****************************
  * External action methods *
  ***************************/

void PoiActionRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)] = pixel;
}

void PoiActionRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){

  _currentAction = SHOW_STATIC_FRAME;
  uint8_t timeout = (uint16_t)timeOutMSB *256 + timeOutLSB;
  if (_logLevel != MUTE)  printf("Play static Scene: %d frames: %d timeout:%d \n", scene, frame, timeout);
  _copyFrameToRegister(0, scene, frame);

  // play initial frame right away
  _ptimer.disable();
  _displayRegister(0);
  _ptimer.setIntervalAndEnable( timeout );
}

void PoiActionRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){

  _currentAction = PLAY_DIRECT;
  _playHandler.init(scene, startFrame, endFrame, speed, loops);
  if (_logLevel != MUTE) _playHandler.printInfo();

  // play initial frame right away
  _ptimer.disable();
  _displayFrame(_playHandler.getCurrentScene(), _playHandler.getCurrentFrame());
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
  _displayFrame(_progHandler.getCurrentScene(), _progHandler.getCurrentFrame());
  _ptimer.setIntervalAndEnable( _progHandler.getDelayMs() );
}

void PoiActionRunner::pauseProg(){
  _currentAction = PAUSE_PROG;
  if (_logLevel != MUTE) printf("Program paused.\n" );
}

void PoiActionRunner::jumptoSync(uint8_t syncId){
  _progHandler.syncNow(syncId);
}

void PoiActionRunner::saveProg(){
  // TODO
}

void PoiActionRunner::continueProg() {
  _currentAction = PLAY_PROG;
  if (_logLevel != MUTE) printf("Program continuing.\n" );
}

/*******************************
  * Interrupt & State handling *
  ******************************/

// no printf in interrupt!
void PoiActionRunner::onInterrupt(){

  if (_currentAction != NO_PROGRAM){
    //Serial.println("play scene  - Next frame");
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
        _displayFrame(_playHandler.getCurrentScene(), _playHandler.getCurrentFrame());
      }
      else {
        _currentAction = NO_PROGRAM;
        // remember last frame in register 0
        _copyFrameToRegister(0, _playHandler.getCurrentScene(), _playHandler.getCurrentFrame());
        if (_logLevel != MUTE) printf("End of program PLAY_DIRECT.\n");
      }
      break;

      case PLAY_PROG:
      if (!_progHandler.checkProgram()){
        return;
      }
      _progHandler.next();
      if (_progHandler.isActive()){
        if (_progHandler.hasDelayChanged()) {
          _ptimer.disable(); }
        _displayFrame(_progHandler.getCurrentScene(), _progHandler.getCurrentFrame());
        if (_progHandler.hasDelayChanged()) {
          _ptimer.setIntervalAndEnable( _progHandler.getDelayMs() ); }
      }
      else {
        _currentAction = NO_PROGRAM;
        // remember last frame in register 0
        _copyFrameToRegister(0, _progHandler.getCurrentScene(), _progHandler.getCurrentFrame());
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
