#include "PoiProgramRunner.h"

PoiProgramRunner::PoiProgramRunner(Verbosity logVerbose) :
    _currentProgram(NO_PROGRAM),
    _scene(0), _frame(0), _startFrame(0), _endFrame(0),
    _delayMs(5), _numLoops(1),
    _currentFrame(0), _currentLoop(0), _logVerbose(logVerbose),
    _duringProgramming(false), _numProgSteps(0), _currentProgStep(0)
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

void PoiProgramRunner::playScene(uint8_t scene, uint8_t startFrame, uint8_t endFrame, uint8_t speed, uint8_t loops){
  if (_logVerbose != MUTE) {
    printf("Playing Scene: %d frames: [%d,%d] delay: %d loops:%d \n", scene, startFrame, endFrame, speed, loops);
  }
  _currentProgram = PLAY_DIRECT;
  _scene = scene;
  _startFrame = startFrame;
  _endFrame = endFrame;
  _delayMs = speed;
  _numLoops = loops;

  _currentFrame = _startFrame;
  _currentLoop = 0;
}

void PoiProgramRunner::_evaluateCommand(uint8_t index) {

  uint8_t* cmd = _prog[index];
  switch(_getCommandType(cmd)) {

    case SET_SCENE:
    _scene = cmd[1];
    _startFrame = cmd[2];
    break;

    case PLAY_FRAMES:
    _startFrame = cmd[1];
    _endFrame = cmd[2];
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
  _evaluateCommand(_currentProgStep);     // SET_SCENE
  _evaluateCommand(++_currentProgStep);   // PLAY_FRAMES

  if (_numProgSteps > _currentProgStep + 1
      || _getCommandType(_prog[_currentProgStep + 1]) == LOOP){
    _evaluateCommand(++_currentProgStep); // LOOP
  }

  // everything is set to start things
  _currentFrame = _startFrame;
  _currentLoop = 0;
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

void PoiProgramRunner::fadeToBlack(){
  //TODO
}


void PoiProgramRunner::displayTest(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < N_PIXELS; i++) {
    _pixels[i] = makeRGBVal(r, g, b);
  }
  ws2812_setColors(1, _pixels);
  //ws2812_setColors(NUM_PIXELS, _pixels);
}

void PoiProgramRunner::showCurrent(){
  ws2812_setColors(N_PIXELS, _pixels);  // update LEDs
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
      showFrame(_scene, _currentFrame);
      break;

      case PAUSE_PROG:
      // do nothing
      break;

      default:
      break;
    }
  }
}
