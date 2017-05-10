#ifndef POI_PROGRAM_H
#define POI_PROGRAM_H

/**
 * Class responsible for running the poi led program
 **/


#include <Arduino.h>
#include <ws2812.h>

#define N_SCENES 1
#define N_FRAMES 200
#define N_PIXELS 60

#define N_PROG_STEPS 20

enum PoiProgram { NO_PROGRAM,
                  PLAY_DIRECT,            // 0
                  PLAY_PROG,              // 1
                  PAUSE_PROG,             // 2
                  NUM_POI_PROGRAMS };     // only used for enum size

enum Verbosity { CHATTY, QUIET, MUTE};

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
  PoiProgramRunner(Verbosity logVerbose);
  // load image
  void setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx);

  // simple play methods
  void playScene(uint8_t scene, uint8_t frameStart,uint8_t frameEnd, uint8_t speed, uint8_t loops);
  void showFrame(uint8_t scene, uint8_t frame);
  void displayOff();
  void displayTest(uint8_t r, uint8_t g, uint8_t b);

  void showCurrent();
  void fadeToBlack();

  void addToProgram(char cmd[7]);
  bool _checkProgram();
  void startProg();
  void pauseProg();
  void resetProg(PoiProgram prog_id);
  void saveProg();

  void setup();         // to be called in setup()
  void loop();          // to be called in the loop
  void onInterrupt();   // to be called during the timer interrupt

  // getters
  uint32_t getDelay();

private:
  PoiProgram _currentProgram;

  // member variables set by the program
  uint8_t _scene;
  uint8_t _frame;
  uint8_t _startFrame;
  uint8_t _endFrame;
  volatile uint8_t _delayMs;
  uint8_t _numLoops;

  // member variables holding the current state of the program
  uint32_t _currentFrame;
  uint32_t _currentLoop;

  volatile SemaphoreHandle_t _timerSemaphore;
  portMUX_TYPE _timerMux;

  rgbVal _pixels[N_PIXELS]; // for temps
  rgbVal _pixelMap[N_SCENES][N_FRAMES][N_PIXELS];

  void _nextProgramStep();
  void _clearProgram();
  bool _programFinished();
  CmdType _getCommandType(uint8_t cmd[6]);
  void _evaluateCommand(uint8_t index);

  bool _duringProgramming;
  uint8_t _numProgSteps;
  uint8_t _currentProgStep;
  uint8_t _prog[N_PROG_STEPS][6];

  Verbosity _logVerbose;
};


#endif
