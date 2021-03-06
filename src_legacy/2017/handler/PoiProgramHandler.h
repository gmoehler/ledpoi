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
#include "AbstractHandler.h"
#include "PlayHandler.h"

/**
 * Holds the information for a fade action on a given frame in the scene
 **/

class PoiProgramHandler : public AbstractHandler
{
public:
  PoiProgramHandler(PlayHandler& playHandler, PoiFlashMemory& flashMemory, LogLevel logLevel);
  void setup(); // onetime setup
  
  // methods for all handlers
  void init(); // init program start

  const char* getActionName();

  void next(); // next program line
  bool isActive();
  
  uint16_t getDelayMs();
  rgbVal* getDisplayFrame();
  
  void finish() { _active = false;};

  void printInfo();
  void printState();

 // current scene from player
  uint8_t getCurrentScene();

  // program related methods
  void addCmdToProgram(unsigned char cmd[7]);
  
  bool syncNow(uint8_t syncId);
  bool hasDelayChanged();
  
#ifdef WITHIN_UNITTEST
  uint8_t __getNumProgSteps();
  uint8_t __getCurrentFrame();
  uint8_t __getNumLabels();
  uint8_t __getNumSyncPoints();
#endif  

private:
  bool _active;
  bool _duringProgramming;
  bool _delayChanged;
  bool _inLoop;
  uint16_t _numProgSteps;
  uint8_t _currentProgStep;
  uint8_t _numLoops;
  uint16_t _currentLoop;
  uint8_t _currentScene;
  PlayHandler _playHandler;

  PoiFlashMemory _flashMemory;

  uint8_t _prog[N_PROG_STEPS][N_PROG_FIELDS];
  std::map<uint8_t, uint8_t> _labelMap; // map between label# and cmd#
  std::map<uint8_t, uint8_t> _syncMap;  // map between snyc# and cmd#

  bool _checkProgram();
  bool _isProgramFinished();
  void _clearProgram();
  void _resetProgram();
  void _updateLabels();
  void _updateSyncPoints();
  CmdType _getCommandType(uint8_t cmd[N_PROG_FIELDS]);
  void _nextProgramStep(bool initial=false);
  void _evaluateCommand(uint8_t index);
  bool _jumpToLabel(uint8_t label);

  void _printProgram();
  LogLevel _logLevel;

};
#endif
