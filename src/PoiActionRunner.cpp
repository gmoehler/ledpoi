#include "PoiActionRunner.h"

PoiActionRunner::PoiActionRunner(PoiTimer& ptimer, LogLevel logLevel) :
    _currentAction(NO_PROGRAM),
    _scene(0), _startFrame(0), _endFrame(0),
    _delayMs(0), _numLoops(1),
    _currentFrame(0), _currentLoop(0),
    _ptimer(ptimer),
    _duringProgramming(false), _inLoop(false),_numProgSteps(0),
    _currentProgStep(0),
    _logLevel(logLevel), _progHandler(logLevel)
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


void PoiActionRunner::_copyCurrentFrameToRegister(uint8_t registerId, double factor){
  _copyFrameToRegister(registerId, _scene, _currentFrame, factor);
}

void PoiActionRunner::_displayRegister(uint8_t registerId){
    ws2812_setColors(N_PIXELS, _pixelRegister[registerId]);
}

void PoiActionRunner::_displayFrame(uint8_t scene, uint8_t frame){
  //printf("Showing frame: scene %d frame \n", scene, frame);
  rgbVal* pixels = _pixelMap[constrain(scene,0,N_SCENES-1)][constrain(frame,0,N_FRAMES-1)];
  ws2812_setColors(N_PIXELS, pixels);
}

void PoiActionRunner::_displayCurrentFrame(){
  _displayFrame(_scene, _currentFrame);
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
  _framePlayer.init(scene, startFrame, endFrame, speed, loops);
  if (_logLevel != MUTE) _framePlayer.printInfo();

  // play initial frame right away
  _ptimer.disable();
  _displayFrame(_framePlayer.getCurrentScene(), _framePlayer.getCurrentFrame());
  _ptimer.setIntervalAndEnable( _framePlayer.getDelayMs() );
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
  _frameFader.init((uint16_t)fadeMSB * 256 + fadeLSB);
  if (_logLevel != MUTE) _frameFader.printInfo();

   // we take what is in register 0 and remember it in register 1
   // later we will copy pixels back using a factor on the rgb values
  _copyRegisterToRegister(0, 1);

  _ptimer.disable();
  _displayRegister(0);
  _ptimer.setIntervalAndEnable( _frameFader.getDelayMs() );
}

void PoiActionRunner::showCurrent(){
  _currentAction = SHOW_CURRENT_FRAME;
  _ptimer.disable();
  _displayCurrentFrame();
}

/****************************
  * Program related methods *
  ***************************/

bool PoiActionRunner::_programFinished(){
  return _currentProgStep + 1 >= _numProgSteps;
}

CmdType PoiActionRunner::_getCommandType(uint8_t cmd[6]){
  return (CmdType) cmd[0];
}

void PoiActionRunner::addCmdToProgram(char cmd[7]){

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
    _clearProgram();
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

bool PoiActionRunner::jumpToLabel(uint8_t label){
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

void PoiActionRunner::_evaluateCommand(uint8_t index) {

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
        if (_logLevel != MUTE) printf("Loop finished.\n" );
        _inLoop = false;
        break;
      }

      // jump to label
      if (!jumpToLabel(cmd[3])){
        printf("Error. Loop aborded.\n" );
        _inLoop = false;
      }
      // continue whether it was successfull or not
      break;
    }
    // first time hit
    _numLoops = (uint16_t)cmd[1] * 256 + cmd[2];
    if (_logLevel != MUTE) printf("Loop numLoops:%d.\n",_numLoops );
    _inLoop = true;
    if (!jumpToLabel(cmd[3])){
      printf("Error. Loop aborded.\n" );
      _inLoop = false;
    }
    break;

    default:
    break;
  }
}

void PoiActionRunner::startProg(){

  _currentAction = PLAY_PROG;
  _currentProgStep = 0;

  _nextProgramStep();
  if (_logLevel != MUTE) printf("Starting program: scene %d frameStart: %d frameEnd: %d delay: %d loops: %d\n", _scene, _startFrame, _endFrame, _delayMs, _numLoops);

  // play frame right away (timer will switch it off again)
  _displayCurrentFrame();
}

void PoiActionRunner::_nextProgramStep(){

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

void PoiActionRunner::pauseProg(){
  _currentAction = PAUSE_PROG;
  if (_logLevel != MUTE) printf("Program paused.\n" );
}

void PoiActionRunner::saveProg(){
  // TODO
}

void PoiActionRunner::continueProg() {
  _currentAction = PLAY_PROG;
  if (_logLevel != MUTE) printf("Program continuing.\n" );
}

bool PoiActionRunner::_checkProgram(){
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

void PoiActionRunner::_clearProgram(){
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
      _framePlayer.next();
      if (_logLevel == CHATTY) _framePlayer.printState();
      if (_framePlayer.isActive()){
        _displayFrame(_framePlayer.getCurrentScene(), _framePlayer.getCurrentFrame());
      }
      else {
        _currentAction = NO_PROGRAM;
        // remember last frame in register 0
        _copyFrameToRegister(0, _framePlayer.getCurrentScene(), _framePlayer.getCurrentFrame());
        if (_logLevel != MUTE) printf("End of program PLAY_DIRECT.\n");
      }
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
      _frameFader.next();
      if (_logLevel == CHATTY)  _frameFader.printState();
      if (_frameFader.isActive()){
        // un-faded frame is in register 1
        _copyRegisterToRegister(1, 0, _frameFader.getCurrentFadeFactor());
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
