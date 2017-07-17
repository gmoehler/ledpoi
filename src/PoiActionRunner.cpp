#include "PoiActionRunner.h"

PoiActionRunner::PoiActionRunner(PoiTimer& ptimer, LogLevel logLevel) :
  _currentAction(NO_ACTION), _currentSyncId(0), _currentScene(0),
  _imageCache(_flashMemory.getSizeOfImageSection(), logLevel),
  _playHandler(_imageCache), _fadeHandler(_imageCache),
  _progHandler(_playHandler, _flashMemory, logLevel),
  _animationHandler(_imageCache), _staticRgbHandler(_imageCache),
  _displayIpHandler(_imageCache), _currentHandler(&_playHandler), 
  _ptimer(ptimer), _logLevel(logLevel)
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
  if (_logLevel != MUTE) printf("Saving image of scene %d to flash.\n", scene);
  if (_flashMemory.saveImage(scene, _imageCache.getRawImageData())){
    _currentScene = scene;
    if (_logLevel != MUTE) printf("Image of scene %d saved to flash.\n", scene);
  }
  else {
    printf("Error saving scene %d to flash.", scene);
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

/********************************
  * Actions operated by handler *
  *******************************/
// generic method to set current handler to passed in handler, display and start timer
void PoiActionRunner::_currentHandlerStart(AbstractHandler* handler, 
  PoiAction action){
  _currentAction = action;  
  _currentHandler = handler;

  if (_logLevel != MUTE) {
    printf("Starting action %s:\n", _currentHandler->getActionName());
    _currentHandler->printInfo();
  }

  // play initial frame right away
  _ptimer.disable();
  _display(_currentHandler->getDisplayFrame());
  _ptimer.setIntervalAndEnable( _currentHandler->getDelayMs() );
}
  
void PoiActionRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){
  
  if (scene != _currentScene){
    _updateSceneFromFlash(scene);
    _currentScene = scene;
  }

  uint8_t timeout = (uint16_t)timeOutMSB *256 + timeOutLSB;

  // use frame player but with same start and end frame and timeout as speed
  _playHandler.init(frame, frame, timeout, 1);
  _currentHandlerStart(&_playHandler, SHOW_STATIC_FRAME);
}

void PoiActionRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){
  
  // init of play action
  if (scene != _currentScene){
    _updateSceneFromFlash(scene);
    _currentScene = scene;
  }

  _playHandler.init(startFrame, endFrame, speed, loops);
  _currentHandlerStart(&_playHandler, PLAY_DIRECT);
}

void PoiActionRunner::fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB){

  uint16_t fadeTime = (uint16_t)fadeMSB * 256 + fadeLSB;
  if (fadeTime < MIN_FADE_TIME){
  	displayOff();
      return;
  }

  _fadeHandler.init(fadeTime);
  _currentHandlerStart(&_fadeHandler, FADE_TO_BLACK);
}

void PoiActionRunner::playWorm(Color color, uint8_t registerLength, uint8_t numLoops, bool synchronous){

  uint16_t delayMs = 25;
  _animationHandler.init(ANIMATIONTYPE_WORM, registerLength, numLoops, color, delayMs);

  if (synchronous){
    _ptimer.disable();
    _display(_animationHandler.getDisplayFrame());
    while (true){
      _animationHandler.next();
      if (!_animationHandler.isActive()){
        break;
      }
      _display(_animationHandler.getDisplayFrame());
      delay(delayMs);
    }
    displayOff();
    _currentAction = NO_ACTION;
  }
  else {
    _currentHandlerStart(&_animationHandler, ANIMATION_WORM);
  }
}

void PoiActionRunner::playWummer(Color color, uint8_t length, uint8_t numLoops){

  uint16_t delayMs = 30;
  printf("Playing Wummer Animation...\n");
  _animationHandler.init(ANIMATIONTYPE_WUMMER, length, numLoops, color, delayMs);  
  _currentHandlerStart(&_animationHandler, ANIMATION_WUMMER);
}


void PoiActionRunner::showStaticRgb(uint8_t r, uint8_t g, uint8_t b, uint8_t nLeds) {
  _staticRgbHandler.init(r,g,b,nLeds);
  _currentHandlerStart(&_staticRgbHandler, SHOW_STATIC_RGB);
}

void PoiActionRunner::displayOff() {
  showStaticRgb(0,0,0);
}

void PoiActionRunner::displayIp(uint8_t ipOffset, bool withStaticBackground){
  _displayIpHandler.init(ipOffset, withStaticBackground);
  _currentHandlerStart(&_displayIpHandler, SHOW_STATIC_RGB);
}

/****************************
  * Program related methods *
  ***************************/

void PoiActionRunner::addCmdToProgram(unsigned char cmd[7]){
  _progHandler.addCmdToProgram(cmd);
}

void PoiActionRunner::startProg(){
  _currentAction = PLAY_PROG;

  _progHandler.init();
  if (!_progHandler.isActive()){
    return; // if check was negative
  }
  if (_logLevel != MUTE) _progHandler.printInfo();

  // play initial frame right away
  _ptimer.disable();
  _display(_progHandler.getDisplayFrame());
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

      // all action that are using handlers based on AbastractHandler
      case PLAY_DIRECT:
      case FADE_TO_BLACK:
      case ANIMATION_WORM:
      case ANIMATION_WUMMER:
      _currentHandler->next();
      if (_logLevel == CHATTY) _currentHandler->printState();
      
      _display(_currentHandler->getDisplayFrame());

      if (!_currentHandler->isActive()) {
        _currentAction = NO_ACTION;
        if (_logLevel != MUTE) printf("End of action %s.\n", 
          _currentHandler->getActionName());
      }
      
      break;
      
      case PLAY_PROG:
      _progHandler.next();

      if (_progHandler.isActive()){
        // check and update scene and interval if changed
        if (_progHandler.hasDelayChanged()) {
          _ptimer.disable();
        }
        uint8_t scene = _progHandler.getCurrentScene();
        if (scene != _currentScene){
          _updateSceneFromFlash(scene);
          _currentScene = scene;
        }
        // finally display the frame
        _display(_progHandler.getDisplayFrame());
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

      case NO_ACTION:
      case PAUSE_PROG:
      case SHOW_STATIC_RGB:
      case DISPLAY_IP:
      // do nothing
      break;

      default:
      break;
    }
  }
}
