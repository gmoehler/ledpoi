#include "PoiProgramRunner.h"

PoiProgramRunner::PoiProgramRunner(PoiTimer& ptimer, LogLevel logLevel) :
    _currentAction(NO_PROGRAM),
    _scene(0), _startFrame(0), _endFrame(0),
    _delayMs(0), _numLoops(1),
    _currentFrame(0), _currentLoop(0), _currentFadeStep(0),
    _ptimer(ptimer),
    _duringProgramming(false), _numProgSteps(0), _currentProgStep(0),
    _logLevel(logLevel)
    {
      // initialize map and register
      if (_logLevel != MUTE) printf("Initializing image map and register.\n" );
      rgbVal black = makeRGBVal(0,0,0);
      _fillRegister(black);
      _fillMap(black);
    }

void PoiProgramRunner::setup(){
  // Create semaphore to inform us when the timer has fired
  _timerSemaphore = xSemaphoreCreateBinary();
}

/**********************
  * Utility functions *
  *********************/

rgbVal PoiProgramRunner::_getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx) {
  return _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)];
}

void PoiProgramRunner::_fillRegister(rgbVal rgb){
  for (int i = 0; i < N_PIXELS; i++) {
    _pixelRegister[i] = rgb;
  }
}

void PoiProgramRunner::_fillMap(rgbVal rgb){
  for (int s=0; s<N_SCENES; s++){
    for (int f=0; f<N_FRAMES; f++){
      for (int p=0; p<N_PIXELS; p++){
        setPixel(s, f, s, rgb);
      }
    }
  }
}


void PoiProgramRunner::_copyFrameToRegister(uint8_t scene_idx, uint8_t frame_idx, double factor){
  for (int i = 0; i < N_PIXELS; i++) {
    rgbVal rgb = _getPixel(scene_idx, frame_idx, i);
    if (factor == 1){
      _pixelRegister[i] = rgb;
    }
    else {
      _pixelRegister[i] =  makeRGBVal(  ((double)rgb.r) * factor,
                                        ((double)rgb.g) * factor,
                                        ((double)rgb.b) * factor );
    }
  }
}

void PoiProgramRunner::_copyCurrentFrameToRegister(double factor){
  _copyFrameToRegister(_scene, _currentFrame, factor);
}

void PoiProgramRunner::_displayRegister(){
    ws2812_setColors(N_PIXELS, _pixelRegister);
}

void PoiProgramRunner::_displayFrame(uint8_t scene, uint8_t frame){
  //printf("Showing frame: scene %d frame \n", scene, frame);
  rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][constrain(frame,0,N_FRAMES-1)];
  ws2812_setColors(N_PIXELS, pixels);
}

void PoiProgramRunner::_displayCurrentFrame(){
  _displayFrame(_scene, _currentFrame);
}

/**********************
  * External methods *
  *********************/

void PoiProgramRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)] = pixel;
}

void PoiProgramRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){

  _currentAction = SHOW_STATIC_FRAME;
  _scene          = constrain(scene,0,N_SCENES-1);
  _startFrame     = constrain(frame,0,N_FRAMES-1);
  _endFrame       = constrain(frame,0,N_FRAMES-1);
  _delayMs        = timeOutMSB << 8 + timeOutLSB;
  _numLoops       = 1;

  if (_logLevel != MUTE) printf("Playing static frame: scene %d frame: %d timeout: %d\n", _scene, _startFrame, _delayMs);

  _currentFrame = _startFrame;
  _currentLoop = 0;

  // play frame right away (timer will switch it off again after timeout)
  _ptimer.disable();
  _displayCurrentFrame();
  _ptimer.setIntervalAndEnable( _delayMs );
}

void PoiProgramRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){

  _currentAction = PLAY_DIRECT;
  _scene          = constrain(scene,0,N_SCENES-1);
  _startFrame     = constrain(startFrame,0,N_FRAMES-1);
  _endFrame       = constrain(endFrame,0,N_FRAMES-1);
  _delayMs        = speed;
  _numLoops       = loops;

  if (_logLevel != MUTE) printf("Playing Scene: %d frames: [%d,%d] delay: %d loops:%d \n", _scene, _startFrame, _endFrame, _delayMs, _numLoops);

  _currentFrame = _startFrame;
  _currentLoop = 0;

  // play initial frame right away
  _ptimer.disable();
  _displayCurrentFrame();
  _ptimer.setIntervalAndEnable( _delayMs );
}

void PoiProgramRunner::showStaticRgb(uint8_t r, uint8_t g, uint8_t b) {
  // directly "play" out of register
  _fillRegister(makeRGBVal(r,g,b));

  _currentAction = SHOW_STATIC_RGB;
  _ptimer.disable();
  _displayRegister();
}

void PoiProgramRunner::displayOff() {
  showStaticRgb(0,0,0);
}

void PoiProgramRunner::fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB){
  _currentFadeStep = 0; // will iterate this one up to N_FADE_STEPS

  _currentAction = FADE_TO_BLACK;
  // dont touch _scene, _startFrame, _endFrame and _numLoops
  uint16_t fadeTime = fadeMSB << 8 + fadeLSB;
  _delayMs = fadeTime / N_FADE_STEPS;

  // start with current frame
  _copyCurrentFrameToRegister();

  _ptimer.disable();
  _displayRegister();
  if (_delayMs > 0){
    _ptimer.setIntervalAndEnable( _delayMs );
  }
}

void PoiProgramRunner::showCurrent(){
  _currentAction = SHOW_CURRENT_FRAME;
  _ptimer.disable();
  _displayCurrentFrame();
}

/****************************
  * Program related methods *
  ***************************/

bool PoiProgramRunner::_programFinished(){
  return _currentProgStep + 1 >= _numProgSteps;
}

CmdType PoiProgramRunner::_getCommandType(uint8_t cmd[6]){
  return (CmdType) cmd[0];
}

void PoiProgramRunner::addCmdToProgram(char cmd[7]){

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

  if (_numProgSteps < 2
    || _getCommandType(_prog[0]) != SET_SCENE
    || _getCommandType(_prog[1]) != PLAY_FRAMES){
    printf("Error. Program does not start with initial scene and play.\n" );
    _clearProgram();
    return;
  }

  _currentAction = PLAY_PROG;
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
  _displayCurrentFrame();
  _ptimer.setIntervalAndEnable( _delayMs );
}

void PoiProgramRunner::_nextProgramStep(){

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
  _ptimer.setIntervalAndEnable( _delayMs );
}

void PoiProgramRunner::pauseProg(){
  _currentAction = PAUSE_PROG;
}

void PoiProgramRunner::saveProg(){
  // TODO
}

void PoiProgramRunner::continueProg() {
  _currentAction = PLAY_PROG;
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

/*******************************
  * Interrupt & State handling *
  ******************************/

// no printf in interrupt!
void PoiProgramRunner::onInterrupt(){

  switch(_currentAction){
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
    switch(_currentAction){

      case PLAY_PROG:
        _checkProgram();
        // continue below - no break
      case PLAY_DIRECT:
      case SHOW_STATIC_FRAME:

      _currentFrame++;

      // end of frame reached
      if (_currentFrame > _endFrame){
        _currentLoop++;

        // end of final loop reached
        if (_currentLoop > _numLoops){
          if (_currentAction == PLAY_PROG){
            if (_logLevel != MUTE) printf("Reading next program step.\n");
            _nextProgramStep();

            if (_programFinished()) {
              _currentAction = NO_PROGRAM;
              if (_logLevel != MUTE) printf("End of program PLAY_PROG.\n");
              return;
            }
          }
          else {
            _currentAction = NO_PROGRAM;
            if (_logLevel != MUTE) printf("End of program PLAY_DIRECT.\n");
          }
          return;
        }

        // just continue to loop
        _currentFrame = _startFrame;
      }

      _displayCurrentFrame();
      break;

      case FADE_TO_BLACK:
      _currentFadeStep++;
      if (_currentFadeStep >= N_FADE_STEPS){
        _currentAction = NO_PROGRAM;
        if (_logLevel != MUTE) printf("End of program FADE_TO_BLACK.\n");
        return;
      }
      _copyCurrentFrameToRegister(((double)(N_FADE_STEPS - _currentFadeStep)) / N_FADE_STEPS);
      _displayRegister();
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
