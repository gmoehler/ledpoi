#include "ProgramCache.h"

ProgramCache::ProgramCache() :
	_numProgLines(0) {}

void ProgramCache::addCommand(PoiCommand cmd){

  if (_numProgLines >= N_PROG_STEPS){
    printf("Error. Number of programming steps exceeds maximum (%d).", N_PROG_STEPS);
    // reset current program
    clearProgram();
		return;
  }

  PoiCommandType type = cmd.getType();
  uint8_t field1 = cmd.getField(1);
  uint16_t field45 = cmd.getCombinedField(4, 5);

  if (type == LOOP_START){
    // keep labels separate in a map along with (next) cmd number
    // field1 is loopId, field45 is number of iterations
    _labelMap[field1] = _numProgLines;
    _numIterationsMap[field1] = field45;
  }
  
  if (type == SYNC_POINT){
    // keep lsync points separate in a map along with (next) cmd number
    _syncMap[field1] = _numProgLines;
  }

  // add cmd to program memory
  memcpy(_prog[_numProgLines], cmd.getRawPoiCommand().field, sizeof(cmd.getRawPoiCommand().field));

  _numProgLines++;
}

 uint8_t* ProgramCache::getRawProgramDataPrt(){
   return (&_prog[0][0]); // cast into a single pointer
 }

// direct access to type of command 
PoiCommandType ProgramCache::_getPoiCommandType(uint8_t line){
  if (line >= _numProgLines) {
    LOGE(PCACHE, "Line %d exeeds number of lines (%d).", line, _numProgLines);
    return NO_COMMAND;
  }
  return static_cast<PoiCommandType> (_getPoiCommandField(line, 0));
}

// direct access to field of command 
uint8_t ProgramCache::_getPoiCommandField(uint8_t line, uint8_t i){
  if (line >= _numProgLines) {
    LOGE(PCACHE, "Line %d exeeds number of lines (%d).", line, _numProgLines);
    return 0;
  }
  return _prog[line][i];
}

uint16_t ProgramCache::_getPoiCommandCombinedField(uint8_t line, uint8_t idx1, uint8_t idx2){
 	uint8_t field1 = _getPoiCommandField(line, idx1);
     uint8_t field2 = _getPoiCommandField(line, idx2);
     return (uint16_t)field1 *256 + field2;
}

// constructs a poi command by copying the command
// from the ProgramCache. Safe for external actions
PoiCommand ProgramCache::getPoiCommand(uint8_t i){
  //TODO test against _numProgLines and retun nocmd if not existing
  RawPoiCommand rawCmd;
  memcpy(rawCmd.field, _prog[i], sizeof(rawCmd.field)); // copy memory
  PoiCommand cmd(rawCmd);
  return cmd;
}

bool ProgramCache::parseProgram(uint8_t numProgLines){
  _numProgLines = numProgLines;
  _updateLabels();
  _updateSyncPoints();
  return _checkProgram();
}

void ProgramCache::_updateLabels(){
  if (_numProgLines>0) {
    _labelMap.clear();
    _numIterationsMap.clear();
    for (int i=0; i<_numProgLines; i++){
      PoiCommandType type = _getPoiCommandType(i);
      if (type == LOOP_START){
          uint16_t field45 = _getPoiCommandCombinedField(i, 4, 5);

        _labelMap[_prog[i][1]] = i;
        _numIterationsMap[_prog[i][1]] = field45;
      }
    }
  }
}

void ProgramCache::_updateSyncPoints(){
  if (_numProgLines>0) {
  _syncMap.clear();
    for (int i=0; i<_numProgLines; i++){
      PoiCommandType type = _getPoiCommandType(i);
      if (type == SYNC_POINT){
        _syncMap[_prog[i][1]] = i;
      }
    }
  }
}

void ProgramCache::printProgram(){
	for (int i=0; i<_numProgLines; i++){
    /* for (int j=0; j<N_CMD_FIELDS; j++){    
        printf ("%d-", _prog[i][j]);
    }
    printf("\n"); */
    PoiCommand cmd = getPoiCommand(i);
		LOGI(PCACHE, "%d: %s", i, cmd.toString().c_str());
	}
}

uint8_t ProgramCache::getLineOfLabel(uint8_t label){
  if (label == 0){
      LOGE(PCACHE, "Loop jump label 0 not allowed.");
      return 0;
  }

  std::map<uint8_t,uint8_t>::iterator it = _labelMap.find(label);
  if (it != _labelMap.end()) {
    return it->second;
  }

  LOGE(PCACHE, "Loop jump label %d not found.", label);
  return 0;
}

uint8_t ProgramCache::getLineOfSyncPoint(uint8_t syncPoint){
  if (syncPoint == 0){
      LOGE(PCACHE, "Sync Point 0 not allowed.");
      return 0;
  }

  std::map<uint8_t,uint8_t>::iterator it = _syncMap.find(syncPoint);
  if (it != _syncMap.end()) {
    return it->second;
  }

  LOGE(PCACHE, "Sync Point %d not found.", syncPoint);
  return 0;
}

void ProgramCache::clearProgram(){
  
  _numProgLines = 0;

  _labelMap.clear();
  _syncMap.clear();
}

bool ProgramCache::_checkProgram(){
  // is there any program?
  if (_numProgLines == 0){
    LOGW(PCACHE, "Warning. Program has no commands." );
    return false;
  }
  // see whether all loops refer to knwon labels
  for (int i=0; i<_numProgLines; i++){
    PoiCommandType type = _getPoiCommandType(i);
    if (type == LOOP_END){
      uint8_t label = _getPoiCommandField(i, 1);
      uint8_t line = getLineOfLabel(label);
      if (line == 0){
          LOGE(PCACHE, "Program Check: Label %d not found.", label);
          return false;
      }
    }
  }

  return true;
}

uint8_t ProgramCache::getNumProgLines(){
	return _numProgLines;
}

uint8_t ProgramCache::getNumSyncPoints(){
	return _syncMap.size();
}

uint8_t ProgramCache::getNumLabels(){
	return _labelMap.size();
}

uint16_t ProgramCache::getNumIterations(uint8_t loopId){
	return _numIterationsMap[loopId];
}
