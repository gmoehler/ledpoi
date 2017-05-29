#ifndef POI_ACTION_RUNNER_H
#define POI_ACTION_RUNNER_H

#include <Arduino.h>
#include <ws2812.h>
#include <map>
#include "ledpoi.h"
#include "PoiTimer.h"
#include "PlayHandler.h"
#include "FadeHandler.h"
#include "PoiProgramHandler.h"

#define N_REGISTERS 2
#define N_PROG_STEPS 50

enum PoiAction { NO_PROGRAM,
                  SHOW_STATIC_RGB,
                  PLAY_DIRECT,
                  SHOW_CURRENT_FRAME,
                  SHOW_STATIC_FRAME,
                  FADE_TO_BLACK,
                  PLAY_PROG,
                  PAUSE_PROG
                };

/**
 * Class responsible for running the poi led program
 * After each action the last frame is always copied to Register 0
 **/


class PoiActionRunner
{
public:
  PoiActionRunner(PoiTimer& ptimer, LogLevel logLevel);

  void setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx,
      uint8_t r, uint8_t g, uint8_t b);

  // action methods
  void saveScene(uint8_t scene);
  void playScene(uint8_t scene, uint8_t frameStart,uint8_t frameEnd, uint8_t speed, uint8_t loops);
  void showStaticFrame(uint8_t scene, uint8_t frame, uint8_t timeOutMSB, uint8_t timeOutLSB);
  void showStaticRgb(uint8_t r, uint8_t g, uint8_t b);
  void displayOff();
  void fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB);
  void showCurrent();
  void jumptoSync(uint8_t syncId);

  // program related methods
  void addCmdToProgram(char cmd[7]);
  void startProg();
  void pauseProg();
  void continueProg();

  void setup();             // to be called in setup()
  void loop();               // to be called in the loop
  void onInterrupt();   // to be called during the timer interrupt
  void resetFlash();
  void clearImageMap();

private:
  PoiAction _currentAction;
  uint8_t _currentSyncId;
  uint8_t _currentScene;

  // handlers for the different programs
  PlayHandler _playHandler;
  FadeHandler _fadeHandler;
  PoiProgramHandler _progHandler;

  PoiFlashMemory _flashMemory;

  // data stores
  // after each action the last frame is stored in _pixelRegister[0]
  rgbVal _pixelRegister[2][N_PIXELS];
  uint8_t *_pixelMap;

  // access functions
  rgbVal _makeRGBValue(uint8_t rgb_array[3]);
  void _setPixel(uint8_t frame_idx, uint8_t pixel_idx,  uint8_t r, uint8_t g, uint8_t b);
  void _setPixel(uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal _getPixel(uint8_t frame_idx, uint8_t pixel_idx);
  void _updateSceneFromFlash(uint8_t scene);
  void _copyFrameToRegister(uint8_t registerId, uint8_t frame_idx, float factor=1);
  void _copyRegisterToRegister(uint8_t registerId1, uint8_t registerId2, float factor=1);
  void _fillRegister(uint8_t register Id, rgbVal rgb);
  void _fillMap(rgbVal rgb);

  // display functions
  void _displayFrame(uint8_t frame);
  void _displayRegister(uint8_t register Id);

  // other member variables
  PoiTimer _ptimer;
  volatile SemaphoreHandle_t _timerSemaphore;
  portMUX_TYPE _timerMux;
  LogLevel _logLevel;
};


#endif
