#ifndef POI_PROGRAM_H
#define POI_PROGRAM_H

/**
 * Class responsible for running the poi led program
 **/

#include <Arduino.h>
#include <ws2812.h>
#include "ledpoi.h"
#include "PoiTimer.h"

#define N_SCENES 1
#define N_FRAMES 200
#define N_PIXELS 60

#define N_PROG_STEPS 50
#define N_FADE_STEPS 50

enum PoiAction { NO_PROGRAM,
                  SHOW_STATIC_RGB,
                  PLAY_DIRECT,
                  SHOW_CURRENT_FRAME,
                  SHOW_STATIC_FRAME,
                  FADE_TO_BLACK,
                  PLAY_PROG,
                  PAUSE_PROG
                };

enum CmdType {  PROG_END,
                SET_SCENE,
                PLAY_FRAMES,
                GOTO,
                GOTO_FADE,
                LOOP,
                SYNC_WAIT
              };

class PoiProgramRunner
{
public:
  PoiProgramRunner(PoiTimer& ptimer, LogLevel logLevel);
  // load image
  void setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);

  // simple play methods
  void playScene(uint8_t scene, uint8_t frameStart,uint8_t frameEnd, uint8_t speed, uint8_t loops);
  void showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB);
  void displayOff();
  void showStaticRgb(uint8_t r, uint8_t g, uint8_t b);

  void showCurrent();
  void fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB);

  void addCmdToProgram(char cmd[7]);
  void startProg();
  void pauseProg();
  void continueProg();
  void saveProg();

  void setup();         // to be called in setup()
  void loop();          // to be called in the loop
  void onInterrupt();   // to be called during the timer interrupt

private:
  PoiAction _currentAction;

  // member variables set by the actions
  uint8_t _scene;
  uint8_t _startFrame;
  uint8_t _endFrame;
  volatile uint16_t _delayMs;
  uint8_t _numLoops;

  // member variables holding the current state of the action
  uint32_t _currentFrame;
  uint32_t _currentLoop;
  uint16_t _currentFadeStep;
  bool _duringProgramming;

  // data stores
  rgbVal _pixelRegister[N_PIXELS]; // for temps and static actions
  rgbVal _pixelMap[N_SCENES][N_FRAMES][N_PIXELS];

  // access functions
  rgbVal _getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx);
  void _copyFrameToRegister(uint8_t scene_idx, uint8_t frame_idx, double factor=1);
  void _copyCurrentFrameToRegister(double factor=1);
  void _fillRegister(rgbVal rgb);
  void _fillMap(rgbVal rgb);

  // display functions
  void _displayFrame(uint8_t scene, uint8_t frame);
  void _displayCurrentFrame();
  void _displayRegister();

  // program handling functions
  bool _checkProgram();
  void _nextProgramStep();
  void _clearProgram();
  bool _programFinished();
  CmdType _getCommandType(uint8_t cmd[6]);
  void _evaluateCommand(uint8_t index);

  // member variables
  PoiTimer _ptimer;
  volatile SemaphoreHandle_t _timerSemaphore;
  portMUX_TYPE _timerMux;

  // programming member variables
  uint8_t _numProgSteps;
  uint8_t _currentProgStep;
  uint8_t _prog[N_PROG_STEPS][6];

  LogLevel _logLevel;
};


#endif
