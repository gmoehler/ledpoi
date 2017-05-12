#include "PoiProgramRunner.h"

PoiProgramRunner::PoiProgramRunner(PoiTimer& ptimer, LogLevel logLevel) :
    _currentAction(NO_PROGRAM),
    _scene(0), _startFrame(0), _endFrame(0),
    _delayMs(0), _numLoops(1), _numFadeSteps(N_FADE_STEPS_DEFAULT),
    _currentFrame(0), _currentLoop(0), _currentFadeStep(0),
    _ptimer(ptimer),
    _duringProgramming(false), _inLoop(false),_numProgSteps(0),
    _currentProgStep(0),
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

void PoiProgramRunner::_copyFrameToRegister(uint8_t scene_idx, uint8_t frame_idx, float factor){
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

/****************************
  * External action methods *
  ***************************/

void PoiProgramRunner::setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel){
  _pixelMap[constrain(scene_idx,0,N_SCENES-1)][constrain(frame_idx,0,N_FRAMES-1)][constrain(pixel_idx,0,N_PIXELS-1)] = pixel;
}

void PoiProgramRunner::showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB){

  _currentAction = SHOW_STATIC_FRAME;
  _scene          = constrain(scene,0,N_SCENES-1);
  _startFrame     = constrain(frame,0,N_FRAMES-1);
  _endFrame       = constrain(frame,0,N_FRAMES-1);
  _delayMs        =  (uint16_t)timeOutMSB *256 + timeOutLSB;
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

  uint16_t fadeTime = (uint16_t)fadeMSB * 256 + fadeLSB;

  if (fadeTime < MIN_FADE_TIME){
  	displayOff();
      return;
  }

  _currentAction = FADE_TO_BLACK;
  _numFadeSteps = N_FADE_STEPS_DEFAULT;
  _delayMs = fadeTime / _numFadeSteps;
  if (_delayMs < MIN_FADE_TIME){
    _delayMs = MIN_FADE_TIME;
    _numFadeSteps = fadeTime / _delayMs;
  }
  if (_logLevel != MUTE) printf("Fade to black - fade time: %ld fade-steps: %d.\n", fadeTime, _numFadeSteps);
  // dont touch _scene, _startFrame, _endFrame and _numLoops
 _currentFadeStep = 0; // will iterate this one up to _numFadeSteps

  // start with current frame
  _copyCurrentFrameToRegister();

  _ptimer.disable();
  _displayRegister();
  _ptimer.setIntervalAndEnable( _delayMs );
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

  if (_numProgSteps >= N_PROG_STEPS){
    printf("Error. Number of programming steps exceeds maximum (%d).\n", N_PROG_STEPS);
    // reset current program
    _clearProgram();
  }

  if ((CmdType) cmd[1] == PROG_END){
    if (_logLevel != MUTE) {
      printf("Program loaded: %d cmds, %d labels, %d sync points.\n",
        _numProgSteps, _labelMap.size(), _syncMap.size());
    }
    // finished programming
    _duringProgramming = false;
    return;
  }

  if (!_duringProgramming) {
    if (_logLevel != MUTE) printf("Starting to read a program...\n" );
    _duringProgramming = true;
    _numProgSteps = 0;
  }

  if ((CmdType) cmd[1] == LABEL){
    // keep labels separate in a map along with (next) cmd number
    if (cmd[2] != 0) {
      _labelMap[cmd[2]] = _numProgSteps;
    }
    else {
      printf("Error. Label code cannot be zero.\n" );
    }
    if (cmd[3] != 0) {
      _syncMap[cmd[3]] = _numProgSteps;
    }
  }

  // add cmd to program memory
  _prog[_numProgSteps][0] = (CmdType) cmd[1];
  for (int i=2; i<7; i++){
    _prog[_numProgSteps][i-1] = (uint8_t) cmd[i];
  }


  _numProgSteps++;
}

bool PoiProgramRunner::jumpToLabel(uint8_t label){
  if (label == 0){
      printf("Error. Loop jump label 0 not allowed.\n");
      return false;
  }

  std::map<uint8_t,uint8_t>::iterator it = _labelMap.find(label);
  if (it != _labelMap.end()) {
    _currentProgStep = it->second;
    return true;
  }

  printf("Error. Loop jump label %d not found.\n", label);
  return false;
}

void PoiProgramRunner::_evaluateCommand(uint8_t index) {

  if (_logLevel != MUTE) printf("Evaluating program line %d...\n", index);

  uint8_t* cmd = _prog[index];
  switch(_getCommandType(cmd)) {

    case SET_SCENE:
    _scene = constrain(cmd[1],0,N_SCENES-1);
    _startFrame = constrain(cmd[2],0,N_FRAMES-1);
    break;

    case PLAY_FRAMES:
    _startFrame = constrain(cmd[1],0,N_FRAMES-1);
    _endFrame = constrain(cmd[2],0,N_FRAMES-1);
    _delayMs =   (uint16_t)cmd[3] * 256 + cmd[4];
    if (_delayMs == 0) _delayMs = 100; // default
    _numLoops = 1;
    break;

    case LOOP:
    if (_inLoop){
      _currentLoop++;
      printf("Loop current loop:%d.\n",_currentLoop );
      if (_currentLoop + 1 >= _numLoops){
        printf("Loop finished.\n" );
        _inLoop = false;
        break;
      }

      // jump to label
      if (!jumpToLabel(cmd[3])){
        printf("Loop aborded.\n" );
        _inLoop = false;
      }
      // continue whether it was successfull or not
      break;
    }
    // first time hit
    _numLoops = (uint16_t)cmd[1] * 256 + cmd[2];
    printf("Loop numLoops:%d.\n",_numLoops );
    _inLoop = true;
    if (!jumpToLabel(cmd[3])){
      printf("Loop aborded.\n" );
      _inLoop = false;
    }
    break;

    default:
    break;
  }
}

void PoiProgramRunner::startProg(){

  _currentAction = PLAY_PROG;
  _currentProgStep = 0;

  _nextProgramStep();
  if (_logLevel != MUTE) printf("Starting program: scene %d frameStart: %d frameEnd: %d delay: %d loops: %d\n", _scene, _startFrame, _endFrame, _delayMs, _numLoops);

  // play frame right away (timer will switch it off again)
  _displayCurrentFrame();
}

void PoiProgramRunner::_nextProgramStep(){

  bool startProcessing = false;

  while (!startProcessing && !_programFinished()){

    _evaluateCommand(++_currentProgStep);

    // only play command actually needs processing
    if (_getCommandType(_prog[_currentProgStep]) == PLAY_FRAMES){
      startProcessing = true;
    }
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
  if (_logLevel != MUTE) printf("Program paused.\n" );
}

void PoiProgramRunner::saveProg(){
  // TODO
}

void PoiProgramRunner::continueProg() {
  _currentAction = PLAY_PROG;
  if (_logLevel != MUTE) printf("Program continuing.\n" );
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
  _inLoop = false;
  _numProgSteps = 0;
  _labelMap.clear();
  _syncMap.clear();
}

/*******************************
  * Interrupt & State handling *
  ******************************/

// no printf in interrupt!
void PoiProgramRunner::onInterrupt(){

  if (_currentAction != NO_PROGRAM){
    //Serial.println("play scene  - Next frame");
    // Give a semaphore that we can check in the loop
    xSemaphoreGiveFromISR(_timerSemaphore, NULL);
  }
}

void PoiProgramRunner::loop(){

  float factor = 1;

  if (xSemaphoreTake(_timerSemaphore, 0) == pdTRUE){
    switch(_currentAction){

      case PLAY_DIRECT:
      _currentFrame++;

      if (_currentFrame > _endFrame){
        // end of scene reached
        _currentLoop++;

        if (_currentLoop >= _numLoops){
          // end of final loop reached
          _currentAction = NO_PROGRAM;
          if (_logLevel != MUTE) printf("PLAY_DIRECT: End of program reached.\n");
          break;
        }

        // next loop starts
        _currentFrame = _startFrame;
      }
      _displayCurrentFrame();
      break;

      case PLAY_PROG:
      if (!_checkProgram()){
        break;
      }
      _currentFrame++;
      if (_currentFrame > _endFrame){
        // end of frame reached
        if (_programFinished()) {
          _currentAction = NO_PROGRAM;
          if (_logLevel != MUTE) printf("PLAY_PROG: End of program reached.\n");
          break;
        }
        if (_logLevel != MUTE) printf("PLAY_PROG: Reading next program step.\n");
        _nextProgramStep();
        break;
      }
      _displayCurrentFrame();
      break;

      case SHOW_STATIC_FRAME:
      // reached timeout
      _currentAction = NO_PROGRAM;
      if (_logLevel != MUTE) printf("Timeout of SHOW_STATIC_FRAME reached.\n");
      break;

      case FADE_TO_BLACK:
      _currentFadeStep++;
      if (_currentFadeStep > _numFadeSteps){
        // finished fading
        _currentAction = NO_PROGRAM;
        if (_logLevel != MUTE) printf("End of program FADE_TO_BLACK.\n");
        break;
      }
      factor = (float)(_numFadeSteps - _currentFadeStep) / _numFadeSteps;
      _copyCurrentFrameToRegister(factor);
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
