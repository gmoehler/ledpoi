#ifndef POI_PROGRAM_HANDLER_H
#define POI_PROGRAM_HANDLER_H

#include "ledpoi.h"
#include "PoiCommand.h"
#include "memory/ProgramCache.h"

extern ProgramCache programCache;

struct ProgramStatus {
  uint8_t curLine = 0; 
  bool inLoop = false;    // whether we are in a loop
  uint8_t numLoops = 0;   // number of loops for current loop
  uint16_t curLoop = 0;   // current loop count
  bool finished = false;
};

/**
 * Runs a program
 **/

class PoiProgramHandler
{
public:
  PoiProgramHandler();
  
  void init(uint8_t startLine=0); // init program start at particular line
  bool next(); // next program line
  PoiCommand getCurrentCommand();

private:
  ProgramStatus _status;
  ProgramStatus nextStatus;
  bool _initial;  // true for first cmd after reset

  bool _findNext();
  bool _handleControlCommand();

  void _resetLoop(ProgramStatus& status);
};
#endif
