#include "PoiProgramRunner.h"

PoiProgramRunner::PoiProgramRunner(PoiTimer& ptimer, Verbosity logVerbose) :
    _currentProgram(NO_PROGRAM),
    _scene(0), _startFrame(0), _endFrame(0),
    _delayMs(0), _numLoops(1),
    _currentFrame(0), _currentLoop(0), _currentFadeStep(0),
    _ptimer(ptimer),
    _duringProgramming(false), _numProgSteps(0), _currentProgStep(0),
    _logVerbose(logVerbose)
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

CmdType PoiProgramRunner::_getCommandType(uint8_t cmd[6]){
  return (CmdType) cmd[0];
}

void PoiProgramRunner::addToProgram(char cmd[7]){

  if (_numProgSteps > N_PROG_STEPS){
    printf("Error. Number of programming steps exceeds maximum (%d).\n", N_PROG_STEPS);
    // reset current program
    _clearProgram();
  }

  if ((CmdType) cmd[1] == PROG_END){
    // finished programming
    _duringProgramming = false;
    return;
  }

  if (!_duringProgramming) {
    printf("Starting to read a program...\n" );
    _duringProgramming = true;
    _numProgSteps = 0;
  }

  _prog[_numProgSteps][0] = (CmdType) cmd[1];
  for (int i=2; i<7; i++){
    _prog[_numProgSteps][i-1] = (uint8_t) cmd[i];
  }
  _numProgSteps++;
}

bool PoiProgramRunner::_programFinished(){
  return _currentProgStep + 1 >= _numProgSteps;
}

void PoiProgramRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){

  _currentProgram = SHOW_STATIC_FRAME;
  _scene = constrain(scene,0,N_SCENES-1);
  _startFrame = constrain(frame,0,N_FRAMES-1);
  _endFrame = constrain(frame,0,N_FRAMES-1);
  _delayMs = timeOutMSB << 8 + timeOutLSB;
  _numLoops = 1;
  if (_logVerbose != MUTE) {
    printf("Playing static frame: scene %d frame: %d timeout: %d\n", _scene, _startFrame, _delayMs);
  }

  _currentFrame = _startFrame;
  _currentLoop = 0;

  // play frame right away (timer will switch it off again)
  _ptimer.disable();
  _displayCurrentFrame();

  if (_logVerbose != MUTE) {
    printf("Setting timer interval to %d ms\n", _delayMs);
  }
  _ptimer.setInterval( _delayMs );
  _ptimer.enable();
}

void PoiProgramRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){

  uint32_t prevDelayMs = _delayMs;

  _currentProgram = PLAY_DIRECT;
  _scene = constrain(scene,0,N_SCENES-1);
  _startFrame = constrain(startFrame,0,N_FRAMES-1);
  _endFrame = constrain(endFrame,0,N_FRAMES-1);
  _delayMs = speed;
  _numLoops = loops;
  if (_logVerbose != MUTE) {
    printf("Playing Scene: %d frames: [%d,%d] delay: %d loops:%d \n", _scene, _startFrame, _endFrame, _delayMs, _numLoops);
  }

  _currentFrame = _startFrame;
  _currentLoop = 0;

  // play initial frame right away
  _ptimer.disable();
  _displayCurrentFrame();

  if (_logVerbose != MUTE) {
    printf("Setting timer interval to %d ms\n", _delayMs);
  }
  _ptimer.setInterval( _delayMs );
  _ptimer.enable();
}

void PoiProgramRunner::_evaluateCommand(uint8_t index) {

  uint8_t* cmd = _prog[index];
  switch(_getCommandType(cmd)) {

    case SET_SCENE:
    _scene = constrain(cmd[1],0,N_SCENES-1);
    _startFrame = constrain(cmd[2],0,N_FRAMES-1);
    break;

    case PLAY_FRAMES:
    _startFrame = constrain(cmd[1],0,N_FRAMES-1);
    _endFrame = constrain(cmd[2],0,N_FRAMES-1);
    _delayMs =  (cmd[3] << 8) + cmd[4];
    _numLoops = 1;
    break;

    case LOOP:
    _numLoops = (cmd[1] << 8) + cmd[2];
    break;

    default:
    break;
  }
}

void PoiProgramRunner::startProg(){

  if (_numProgSteps < 2 || _getCommandType(_prog[0]) != SET_SCENE
    || _getCommandType(_prog[1]) != PLAY_FRAMES){
    printf("Error. Program does not start with initial scene and play.\n" );
    _clearProgram();
    return;
  }

  _currentProgram = PLAY_PROG;
  _currentProgStep = 0;
  _ptimer.disable();
  _evaluateCommand(_currentProgStep);     // SET_SCENE
  _evaluateCommand(++_currentProgStep);   // PLAY_FRAMES

  if (_numProgSteps > _currentProgStep + 1
      || _getCommandType(_prog[_currentProgStep + 1]) == LOOP){
    _evaluateCommand(++_currentProgStep); // LOOP
  }

  // everything is set to start things
  _currentFrame = _startFrame;
  _currentLoop = 0;

  // play frame right away (timer will switch it off again)
  _ptimer.disable();
  _displayCurrentFrame();

  if (_logVerbose != MUTE) {
    printf("Setting timer interval to %d ms\n", _delayMs);
  }
  _ptimer.setInterval( _delayMs );
  _ptimer.enable();
}

void PoiProgramRunner::_nextProgramStep(){

  uint32_t prevDelayMs = _delayMs;

  // read one more command if this one is SET_SCENE
  if (!_programFinished() && _getCommandType(_prog[_currentProgStep + 1]) == SET_SCENE){
    _evaluateCommand(++_currentProgStep); // SET_SCENE
  }

  if (!_programFinished()){
    _evaluateCommand(++_currentProgStep);
  }

  if (!_programFinished() && _getCommandType(_prog[_currentProgStep + 1]) == LOOP){
    _evaluateCommand(++_currentProgStep); // LOOP
  }

  // everything is set to start things
  _currentFrame = _startFrame;
  _currentLoop = 0;

  // play frame right away (timer will switch it off again)
  _ptimer.disable();
  _displayCurrentFrame();

  if (_logVerbose != MUTE) {
    printf("Setting timer interval to %d ms\n", _delayMs);
  }
  _ptimer.setInterval( _delayMs );
  _ptimer.enable();
}

void PoiProgramRunner::_displayCachedFrame(){
    ws2812_setColors(N_PIXELS, _pixels);
}

void PoiProgramRunner::_displayFrame(uint8_t scene, uint8_t frame){
  //printf("Showing frame: scene %d frame \n", scene, frame);
  rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][constrain(frame,0,N_FRAMES-1)];
  ws2812_setColors(N_PIXELS, pixels);
}

void PoiProgramRunner::_displayCurrentFrame(){
  _displayFrame(_scene, _currentFrame);
}

void PoiProgramRunner::displayOff() {
  for (int i = 0; i < N_PIXELS; i++) {
    _pixels[i] = makeRGBVal(0, 0, 0);
  }

  _currentProgram = FADE_TO_BLACK;
  _ptimer.disable();
  _displayCachedFrame();
}

void PoiProgramRunner::fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB){

  // will iterate this one up to N_FADE_STEPS
  _currentFadeStep = 0;

  _currentProgram = FADE_TO_BLACK;
  // dont touch _scene, _startFrame, _endFrame and _numLoops
  uint16_t fadeTime = fadeMSB << 8 + fadeLSB;
  _delayMs = fadeTime / N_FADE_STEPS;

  // copy current frame to _pixels
  for (int i = 0; i < N_PIXELS; i++) {
    _pixels[i] = getPixel(_scene, _currentFrame, i);
  }

  _ptimer.disable();
  _displayCachedFrame();

  if (_delayMs > 0){
    if (_logVerbose != MUTE) {
      printf("Setting timer interval to %d ms\n", _delayMs);
    }
    _ptimer.setInterval( _delayMs );
    _ptimer.enable();
  }
}

void PoiProgramRunner::displayTest(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < N_PIXELS; i++) {
    _pixels[i] = makeRGBVal(r, g, b);
  }
  //ws2812_setColors(1, _pixels);
  ws2812_setColors(N_PIXELS, _pixels);
}

void PoiProgramRunner::showCurrent(){
  _currentProgram = SHOW_CURRENT_FRAME;
  _ptimer.disable();
  _displayCurrentFrame();
}


void PoiProgramRunner::pauseProg(){
  _currentProgram = PAUSE_PROG;
}

void PoiProgramRunner::saveProg(){
  // TODO
}

void PoiProgramRunner::continueProg() {
  _currentProgram = PLAY_PROG;
}

bool PoiProgramRunner::_checkProgram(){
  if (_numProgSteps == 0){
    printf("Error. Cannot start program with no commands.\n" );
    return false;
  }
  if (_duringProgramming){
    printf("Error. Cannot start a program when transmission is not completed.\n" );
    return false;
  }
  return true;
}

void PoiProgramRunner::_clearProgram(){
  _duringProgramming = false;
  _numProgSteps = 0;
}

// no printf in interrupt!
void PoiProgramRunner::onInterrupt(){

  switch(_currentProgram){
    case PLAY_DIRECT:
    case PLAY_PROG:
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

      case PLAY_PROG:
        _checkProgram();
        // continue below - no break
      case PLAY_DIRECT:
      case SHOW_STATIC_FRAME:

      _currentFrame++;
      if (_currentFrame > _endFrame){
        _currentLoop++;

        // end of loops
        if (_currentLoop > _numLoops - 1){
          if (_currentProgram == PLAY_PROG){
            if (_logVerbose != MUTE){
              printf("Reading next program step.\n");
            }
            _nextProgramStep();

            if (_programFinished()) {
              _currentProgram = NO_PROGRAM;
              if (_logVerbose != MUTE){
                printf("End of program PLAY_PROG.\n");
              }
              return;
            }
          }
          else {
            _currentProgram = NO_PROGRAM;
            if (_logVerbose != MUTE){
              printf("End of program PLAY_DIRECT.\n");
            }
          }
          return;
        }

        // just continue to loop
        _currentFrame = _startFrame;
      }
      //printf("Playing scene: %d frame: %d\n", _scene, _currentFrame);
      _displayCurrentFrame();
      break;

      case FADE_TO_BLACK:
      _currentFadeStep++;
      if (_currentFadeStep >= N_FADE_STEPS){
        _currentProgram = NO_PROGRAM;
        return;
      }
      // copy faded pixels to _pixels
      for (int i = 0; i < N_PIXELS; i++) {
        rgbVal rgb = getPixel(_scene, _currentFrame, i);
        _pixels[i] = makeRGBVal(((uint32_t)rgb.r) * (N_FADE_STEPS - _currentFadeStep) / N_FADE_STEPS,
                                ((uint32_t)rgb.g) * (N_FADE_STEPS - _currentFadeStep) / N_FADE_STEPS,
                                ((uint32_t)rgb.b) * (N_FADE_STEPS - _currentFadeStep) / N_FADE_STEPS);
      }
      _displayCachedFrame();

      case PAUSE_PROG:
      // do nothing
      break;

      default:
      break;
    }
  }
}
