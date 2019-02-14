#include "PoiProgramHandler.h"

PoiProgramHandler::PoiProgramHandler() : 
  _initial(true) { 
  //initialize nextStatus
  _findNext();
 }

void PoiProgramHandler::init(uint8_t startLine){
  LOGD(PROGH, "Initializing Poi program handler at line %d.", startLine);
  
  _initial = true;
  _status.curLine = startLine; // default: 0
  
  _resetLoop(_status);
  nextStatus = _status;
  _findNext();
}

PoiCommand PoiProgramHandler::getCurrentCommand(){
	return programCache.getPoiCommand(_status.curLine);
}

bool PoiProgramHandler::next(){

   if (!nextStatus.finished){
    _status = nextStatus;
     _findNext();
     return true;
   }
   
   return false;
}

bool PoiProgramHandler::_findNext(){

  if(_initial){
  	// start with line 0
  	_initial = false;
  }
  else {
  	// go to next line
  	nextStatus.curLine++;
  }

  // continue as long as we hit a playable command
  // or the end of the program
  while (_handleControlCommand()) { }
  
  // true unless end of the program
  nextStatus.finished = nextStatus.curLine >= programCache.getNumProgLines();
  if (nextStatus.finished) {
  	LOGD(PROGH, "_findNext: Program finished." );
  }
  return !nextStatus.finished;
}

bool PoiProgramHandler::_handleControlCommand() { 

  PoiCommand cmd = programCache.getPoiCommand(nextStatus.curLine);
  PoiCommandType type = cmd.getType();
  RawPoiCommand rawCmd = cmd.getRawPoiCommand();
  
  bool isControl = true;
  switch(type) {

    case LOOP_START:
    if (!nextStatus.inLoop) {
    	// loops starts
    	nextStatus.inLoop = true;
    	nextStatus.curLoop=1;
    	nextStatus.numLoops = programCache.getNumIterations(rawCmd.field[1]);
    	LOGD(PROGH, "_findNext: Start program loop %d (line %d) numLoops: %d count: %d",
      	rawCmd.field[1], nextStatus.curLine, nextStatus.numLoops, nextStatus.curLoop );
    }
    nextStatus.curLine++;
    break;

    case SYNC_POINT:
    // just skip it
    nextStatus.curLine++;
    break;

    case LOOP_END:
    if (nextStatus.inLoop) {
      // continue the loop
      nextStatus.curLoop++;

      // reached end of iterations
      if (nextStatus.curLoop > nextStatus.numLoops){
        LOGD(PROGH, "_findNext: Program loop finished for next." );
        nextStatus.curLine++;
        // reset loop and return
        _resetLoop(nextStatus);
        break;
      }
      
      // update program line
      nextStatus.curLine = programCache.getLineOfLabel(rawCmd.field[1]);
      LOGD(PROGH, "_findNext: Loop end %d (starting at line %d) numLoops: %d count: %d",
        rawCmd.field[1], nextStatus.curLine, nextStatus.numLoops, nextStatus.curLoop );
    }
    else {
      LOGD(PROGH, "_findNext: skipping loop end without loop start.");
    }
    break;
      
    default:
    isControl = false;
    break;
  }
  
  // return false (stop loop) for playable comand 
  // or end of program
  return isControl 
    && nextStatus.curLine < programCache.getNumProgLines();
}

void PoiProgramHandler::_resetLoop(ProgramStatus& status){
   status.inLoop = false;          
   status.numLoops =0; 
   status.curLoop =0; 
}
