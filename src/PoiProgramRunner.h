#ifndef POI_PROGRAM_H
#define POI_PROGRAM_H

#include <Arduino.h>
#include <ws2812.h>

#define N_SCENES 1
#define N_FRAMES 200
#define N_PIXELS 60

enum OperationMode {  SYNC,
                      ASYNC};

enum PoiProgram { NO_PROGRAM,
                  PLAY_SCENE,             // 0
                  NUM_POI_PROGRAMS};        // only used for enum size


class PoiProgramRunner
{
public:
  void setPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx, rgbVal pixel);
  rgbVal getPixel(uint8_t scene_idx, uint8_t frame_idx, uint8_t pixel_idx);
  void playScene(uint8_t scene, uint8_t frameStart,uint8_t frameEnd, uint8_t speed, uint8_t loops, OperationMode mode);
  void showFrame(uint8_t scene, uint8_t frame);
  void displayOff();
  void displayTest();
  void statusIO();
  void statusNIO();
  void showCurrent();
  void fadeToBlack();

  void setProgram(PoiProgram prog_id);
  void defineProgram(PoiProgram prog_id, uint8_t cmd0, uint8_t cmd1, uint8_t cmd2);
  void startProg();
  void pauseProg();
  void resetProg(PoiProgram prog_id);
  void saveProg();

  void loop();

private:
  PoiProgram _currentProgram = NO_PROGRAM;
  uint8_t _scene = 0;
  uint8_t _frame = 0;
  uint8_t _startFrame = 0;
  uint8_t _endFrame = 0;
  uint8_t _delay = 10;
  uint8_t _loops = 1;

/* original program variables
  uint8_t progIx=0;
  uint8_t progDef[254][5];
  uint8_t progCurrIx=0;
  uint8_t progLastIx=0;
  uint8_t progState=0;
*/


  uint32_t _currentFrame = 0;

  rgbVal _pixels[N_PIXELS];
  rgbVal _pixelMap[N_SCENES][N_FRAMES][N_PIXELS];
};


#endif
