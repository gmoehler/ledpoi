#include "PoiActionRunner.h"

PoiActionRunner::PoiActionRunner(PoiTimer& ptimer, LogLevel logLevel) :
  _currentAction(NO_ACTION), _currentSyncId(0), _currentScene(0),
  _playHandler(_imageCache),
  _imageCache(_flashMemory.getSizeOfImageSection(), logLevel), _ptimer(ptimer),
  _progHandler(_playHandler, _flashMemory, logLevel),
  _logLevel(logLevel)
{}

void PoiActionRunner::clearImageMap(){
  _imageCache.clearImageMap();
}

void PoiActionRunner::setup(){
  // Create semaphore to inform us when the timer has fired
  _timerSemaphore = xSemaphoreCreateBinary();
  _imageCache.clearImageMap();
  _flashMemory.setup(_logLevel, _imageCache.getRawImageData());
  _progHandler.setup(); // load program
  _updateSceneFromFlash(0); // load scene 0 into flash
}

/**********************
  * Utility functions *
  *********************/

void PoiActionRunner::_display(rgbVal* frame){
    ws2812_setColors(N_PIXELS, frame);
}

void PoiActionRunner::_displayRegister(uint8_t registerId){
    ws2812_setColors(N_PIXELS, _imageCache.getRegister(registerId));
}

void PoiActionRunner::_displayFrame(uint8_t frame){
  //printf("Showing frame %d\n", frame);
  _imageCache.copyFrameToRegister(0, frame);
  _displayRegister(0);
  // TODO : directly get it from image - needs rework of code
}

/****************************
  * External action methods *
  ***************************/

void PoiActionRunner::initializeFlash(){
    _imageCache.clearImageMap();
    _flashMemory.initializeFlash(_logLevel, _imageCache.getRawImageData());
}

void PoiActionRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx,
    uint8_t r, uint8_t g, uint8_t b){
  // TODO: possibly handle cases in which scene_idx changes
  _currentScene = scene_idx;
  _imageCache.setPixel(frame_idx, pixel_idx, r, g, b);
}

void PoiActionRunner::saveScene(uint8_t scene){
  if (_logLevel != MUTE) printf("Saving image of scene %d to flash.\n", _currentScene);
  if (_flashMemory.saveImage(scene, _imageCache.getRawImageData())){
    _currentScene = scene;
    if (_logLevel != MUTE) printf("Image of scene %d saved to flash.\n", _currentScene);
  }
  else {
    printf("Error saving scene %d to flash.", _currentScene);
  }
}

// load scene from flash into memory
void PoiActionRunner::_updateSceneFromFlash(uint8_t scene){
  if (_flashMemory.loadImage(scene, _imageCache.getRawImageData())){
    _currentScene = scene;
    if (_logLevel != MUTE) printf("Scene %d loaded from Flash.\n", _currentScene);
  }
  else{
    printf("Error. Cannot load scene %d\n", scene);
  }
}

void PoiActionRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){
  _currentAction = SHOW_STATIC_FRAME;
  _updateSceneFromFlash(scene);
  uint8_t timeout = (uint16_t)timeOutMSB *256 + timeOutLSB;
  if (_logLevel != MUTE)  printf("Play static frame: %d timeout: %d \n", frame, timeout);
  _imageCache.copyFrameToRegister(0, frame);

  // play initial frame right away
  _ptimer.disable();
  _displayRegister(0);
  _ptimer.setIntervalAndEnable( timeout );
}

void PoiActionRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){
  _currentAction = PLAY_DIRECT;
  if (scene != _currentScene){
    _updateSceneFromFlash(scene);
    _currentScene = scene;
  }
  _playHandler.init(startFrame, endFrame, speed, loops);
  if (_logLevel != MUTE) _playHandler.printInfo();

  // play initial frame right away
  _ptimer.disable();
  _display(_playHandler.getDisplayFrame());
  _ptimer.setIntervalAndEnable( _playHandler.getDelayMs() );
}

void PoiActionRunner::showStaticRgb(uint8_t r, uint8_t g, uint8_t b, uint8_t nLeds) {
  // directly "play" out of register
  _imageCache.fillRegister(0, makeRGBVal(r,g,b), nLeds);

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
   _imageCache.copyRegisterToRegister(0, 1);

  _ptimer.disable();
  _displayRegister(0);
  _ptimer.setIntervalAndEnable( _fadeHandler.getDelayMs() );
}

void PoiActionRunner::showCurrent(){
  _currentAction = SHOW_CURRENT_FRAME;
  _ptimer.disable();
  _displayRegister(0);
}

void PoiActionRunner::playWorm(Color color, uint8_t registerLength, uint8_t numLoops, bool synchronous){

  _currentAction = ANIMATION_WORM;
  _animationHandler.init(ANIMATIONTYPE_WORM, registerLength, numLoops);
  if (_logLevel != MUTE) _animationHandler.printInfo();

  uint16_t delayMs = 25;
  if (_logLevel != MUTE)  printf("Play Worm Animation: Color %d Len: %d delay: %d \n",
    color, registerLength, delayMs);

  _imageCache.clearRegister(0);
  rgbVal* reg0 =  _imageCache.getRegister(0);
  if  (color == RAINBOW){
    rgbVal red =  makeRGBValue(RED);
    rgbVal green =  makeRGBValue(GREEN);
    rgbVal blue =  makeRGBValue(BLUE);
    rgbVal yellow =  makeRGBValue(YELLOW);
    rgbVal lila =  makeRGBValue(LILA);
    rgbVal cyan =  makeRGBValue(CYAN);

    // initialize register 0 with rainbow
    rgbVal rainbow[6] = {lila, blue, cyan, green, red, yellow};
    for (int i=0; i<6; i++){
      reg0[i] = rainbow[i];
    }
  }
  else {
   reg0[0] = makeRGBValue( color );
  }

  // play initial state of register right away
  _ptimer.disable();
  _displayRegister(0);

  if (synchronous){
    while (true){
      _animationHandler.next();
      _imageCache.shiftRegister(0, _animationHandler.getRegisterLength(), !_animationHandler.isLastLoop());
      if (!_animationHandler.isActive()){
        break;
      }
      _displayRegister(0);
      delay( delayMs);
    }
    displayOff();
    _currentAction = NO_ACTION;
  }
  else {
    _ptimer.setIntervalAndEnable( delayMs );
  }
}

void PoiActionRunner::displayIp(uint8_t ipIncrement, bool withStaticBackground){
  // set back the ip led to black
  _imageCache.clearRegister(0);
  if (withStaticBackground){
    rgbVal paleWhite = makeRGBVal(8,8,8);
    _imageCache.fillRegister(0, paleWhite, N_POIS);
  }
  rgbVal* reg0 =  _imageCache.getRegister(0);
    // display colored led (first one less bright for each)
  uint8_t b = 64;
  if (ipIncrement %2 == 0){
    b=8;
  }
  rgbVal color =  makeRGBValue(RED, b);
  switch(ipIncrement/2){
    case 1:
    color =  makeRGBValue(GREEN, b);
    break;

    case 2:
    color =  makeRGBValue(BLUE, b);
    break;

    case 3:
    color =  makeRGBValue(YELLOW, b);
    break;

    case 4:
    color =  makeRGBValue(LILA, b);
    break;
  }
  reg0[ipIncrement]= color;

  _displayRegister(0);
}

/****************************
  * Program related methods *
  ***************************/

void PoiActionRunner::addCmdToProgram(unsigned char cmd[7]){
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

void PoiActionRunner::pauseAction(){
  _currentAction = PAUSE_PROG;
  if (_logLevel != MUTE) printf("Action paused.\n" );
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

bool PoiActionRunner::isProgramActive(){
  return _progHandler.isActive();
}

uint8_t PoiActionRunner::getIpIncrement(){
  uint8_t ipIncrement = 0;
  _flashMemory.loadIpIncrement(&ipIncrement);
  return ipIncrement;
}

void PoiActionRunner::saveIpIncrement(uint8_t ipIncrement){
  _flashMemory.saveIpIncrement(ipIncrement);
}

/*******************************
  * Interrupt & State handling *
  ******************************/

// no printf in interrupt!
void IRAM_ATTR PoiActionRunner::onInterrupt(){

  if (_currentAction != NO_ACTION){
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
        _display(_playHandler.getDisplayFrame());
      }
      else {
        _currentAction = NO_ACTION;
        if (_logLevel != MUTE) printf("End of program PLAY_DIRECT.\n");
      }
      break;

      case PLAY_PROG:
      if (!_progHandler.checkProgram()){
        _currentAction = NO_ACTION;
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
        _displayRegister(0);
        if (_progHandler.hasDelayChanged()) {
          _ptimer.setIntervalAndEnable( _progHandler.getDelayMs() ); }
      }
      else {
        _currentAction = NO_ACTION;
        if (_logLevel != MUTE) printf("End of program PLAY_PROG.\n");
      }
      break;

      case SHOW_STATIC_FRAME:
      // reached timeout
      _currentAction = NO_ACTION;
      if (_logLevel != MUTE) printf("Timeout of SHOW_STATIC_FRAME reached.\n");
      break;

      case FADE_TO_BLACK:
      _fadeHandler.next();
      if (_logLevel == CHATTY)  _fadeHandler.printState();
      if (_fadeHandler.isActive()){
        // un-faded frame is in register 1
        _imageCache.copyRegisterToRegister(1, 0, _fadeHandler.getCurrentFadeFactor());
        _displayRegister(0);
      }
      else {
        _currentAction = NO_ACTION;
        if (_logLevel != MUTE) printf("End of program FADE_TO_BLACK.\n");
      }
      break;

      case ANIMATION_WORM:
      _animationHandler.next();
      if (_logLevel == CHATTY)  _animationHandler.printState();
      if (_animationHandler.isActive()){
        _imageCache.shiftRegister(0, _animationHandler.getRegisterLength(), !_animationHandler.isLastLoop());
        _displayRegister(0);
      }
      else {
         _imageCache.clearRegister(0);
         _displayRegister(0);
        _currentAction = NO_ACTION;
        if (_logLevel != MUTE) printf("End of program ANIMATION_WORM.\n");
      }
      break;

      case NO_ACTION:
      case PAUSE_PROG:
      // do nothing
      break;

      default:
      break;
    }
  }
}
