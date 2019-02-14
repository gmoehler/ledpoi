#ifndef PROGRAM_CACHE_H
#define PROGRAM_CACHE_H

#include <map>
#include "ledpoi.h"
#include "PoiCommand.h"

#ifndef WITHIN_UNITTEST
  #include "memory/PoiFlashMemory.h"
#else
  #include "mock_PoiFlashMemory.h"
#endif

/**
 * Holds the program in a 2 dimensional array (with the raw commands) 
 * along with a map to labels and sync points
 **/

class ProgramCache
{
public:
  ProgramCache();
  void addCommand(PoiCommand cmd);
  PoiCommand getPoiCommand(uint8_t line);
  uint8_t getLineOfLabel(uint8_t syncId);
  uint8_t getLineOfSyncPoint(uint8_t syncId);
  
  uint8_t getNumProgLines();
  uint8_t getNumLabels();
  uint8_t getNumSyncPoints();
  uint16_t getNumIterations(uint8_t loopId);

  void printProgram();
  bool parseProgram(uint8_t numProgLines); // init after reload
  void clearProgram();

  // pointer to data to be used only for Flash write and load
  uint8_t* getRawProgramDataPrt();

private:
  uint16_t _numProgLines;

  uint8_t _prog[N_PROG_STEPS][N_CMD_FIELDS];
  std::map<uint8_t, uint8_t> _labelMap; // map between label# and cmd#
  std::map<uint8_t, uint16_t> _numIterationsMap; // map between label# and number of loops
  std::map<uint8_t, uint8_t> _syncMap;  // map between snyc# and cmd#

  PoiCommandType _getPoiCommandType(uint8_t i);
  uint8_t _getPoiCommandField(uint8_t line, uint8_t i);
  uint16_t _getPoiCommandCombinedField(uint8_t line, uint8_t idx1, uint8_t idx2);
  bool _checkProgram();
  void _updateLabels();
  void _updateSyncPoints();
};
#endif
