#ifndef POI_ACTION_RUNNER_H
#define POI_ACTION_RUNNER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
  #include <ws2812.h>
#else
  #include "../test/mock_Arduino.h"
  #include "../test/mock_ws2812.h"
#endif

#include <map>
#include "ledpoi.h"
#include "PoiTimer.h"
#include "PlayHandler.h"
#include "FadeHandler.h"
#include "PoiProgramHandler.h"
#include "AnimationHandler.h"
#include "ImageCache.h"

enum PoiAction {  NO_ACTION,
                  SHOW_STATIC_RGB,
                  PLAY_DIRECT,
                  SHOW_CURRENT_FRAME,
                  SHOW_STATIC_FRAME,
                  FADE_TO_BLACK,
                  PLAY_PROG,
                  PAUSE_PROG,
                  ANIMATION_WORM
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
  void showStaticRgb(uint8_t r, uint8_t g, uint8_t b, uint8_t nLeds=N_PIXELS);
  void displayOff();
  void fadeToBlack(uint8_t fadeMSB, uint8_t fadeLSB);
  void pauseAction();
  void jumptoSync(uint8_t syncId);
  void playWorm(Color color, uint8_t registerLength, uint8_t numLoops, bool synchronous = true);
  void displayIp(uint8_t ipIncrement, bool withStaticBackgroun);

  // program related methods
  void addCmdToProgram(unsigned char cmd[7]);
  void startProg();
  void pauseProg();
  void continueProg();
  bool isProgramActive();
  uint8_t getIpIncrement();
  void saveIpIncrement(uint8_t ipIncrement);

  void setup();             // to be called in setup()
  void loop();               // to be called in the loop
  void onInterrupt();   // to be called during the timer interrupt
  
  void initializeFlash();
  void clearImageMap();

private:

  void _currentHandlerStart(AbstractHandler *handler, PoiAction action);

  // data stores
  void _updateSceneFromFlash(uint8_t scene);

  // display functions
  void _display(rgbVal* frame);
  void _displayFrame(uint8_t frame);
  void _displayRegister(uint8_t register Id);

  PoiAction _currentAction;
  uint8_t _currentSyncId;
  uint8_t _currentScene;

  ImageCache _imageCache;
  PoiFlashMemory _flashMemory;

  // handlers for the different programs
  PlayHandler _playHandler;
  FadeHandler _fadeHandler;
  PoiProgramHandler _progHandler;
  AnimationHandler _animationHandler;

  AbstractHandler *_currentHandler;

  // other member variables
  PoiTimer _ptimer;
  volatile SemaphoreHandle_t _timerSemaphore;
  portMUX_TYPE _timerMux;
  LogLevel _logLevel;

};


#endif
