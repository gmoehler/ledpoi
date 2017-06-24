#ifndef POI_PROGRAM_HANDLER_H
#define POI_PROGRAM_HANDLER_H

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
  #include "PoiFlashMemory.h"
#else
  #include "../test/mock_Arduino.h"
  #include "../test/mock_PoiFlashMemory.h"
#endif

#include <map>
#include "ledpoi.h"
#include "PlayHandler.h"

/**
 * Holds the information for a fade action on a given frame in the scene
 **/

class PoiProgramHandler
{
public:
  PoiProgramHandler(PlayHandler& playHandler, PoiFlashMemory& flashMemory, LogLevel logLevel);
  void setup();
  void addCmdToProgram(char cmd[7]);
  void init(); // init program start

  void next(); // next program line

  bool checkProgram();
  bool syncNow(uint8_t syncId);

  bool isActive();
  bool hasDelayChanged();

  // current scene, frame and delay from player
  uint8_t getCurrentScene();
  uint8_t getCurrentFrame();
  uint16_t getDelayMs();

  void printInfo();
  void printState();

private:
  bool _active;
  bool _duringProgramming;
  bool _delayChanged;
  bool _inLoop;
  uint8_t _numProgSteps;
  uint8_t _currentProgStep;
  uint8_t _numLoops;
  uint16_t _currentLoop;
  uint8_t _currentScene;
  PlayHandler _playHandler;

  PoiFlashMemory _flashMemory;

  uint8_t _prog[N_PROG_STEPS][N_PROG_FIELDS];
  std::map<uint8_t, uint8_t> _labelMap; // map between label# and cmd#
  std::map<uint8_t, uint8_t> _syncMap;  // map between snyc# and cmd#

  bool _isProgramFinished();
  void _clearProgram();
  CmdType _getCommandType(uint8_t cmd[N_PROG_FIELDS]);
  void _nextProgramStep(bool initial=false);
  void _evaluateCommand(uint8_t index);
  bool _jumpToLabel(uint8_t label);

  void _printProgram();
  LogLevel _logLevel;

};
#endif
