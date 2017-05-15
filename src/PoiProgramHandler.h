#ifndef POI_PROGRAM_HANDLER_H
#define POI_PROGRAM_HANDLER_H

#include <Arduino.h>
#include <map>
#include "ledpoi.h"
#include "PoiTimer.h"
#include "FramePlayer.h"

#define N_PROG_STEPS 50

/**
 * Holds the information for a fade action on a given frame in the scene
 **/

class PoiProgramHandler
{
public:
  PoiProgramHandler(LogLevel logLevel);
  void addCmdToProgram(char cmd[7]);
  void init();
  bool isActive();
  bool hasDelayChanged();
  
  void next();

  uint8_t getCurrentScene();
  uint8_t getCurrentFrame();

  bool checkProgram();

private:
  bool _active;
  bool _duringProgramming;
  bool _delayChanged;
  bool _inLoop;
  uint8_t _numLoops;
  uint32_t _currentLoop;
  uint8_t _numProgSteps;
  uint8_t _currentProgStep;
  FramePlayer _framePlayer;

  uint8_t _prog[N_PROG_STEPS][6];
  std::map<uint8_t, uint8_t> _labelMap; // map between label# and cmd#
  std::map<uint8_t, uint8_t> _syncMap;  // map between snyc# and cmd#

  bool _isProgramFinished();
  void _clearProgram();
  CmdType _getCommandType(uint8_t cmd[6]);
  void _nextProgramStep();
  void _evaluateCommand(uint8_t index);
  bool _jumpToLabel(uint8_t label);

  LogLevel _logLevel;

};
#endif
