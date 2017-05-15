#include "PoiProgramHandler.h"

PoiProgramHandler::PoiProgramHandler(FramePlayer& framePlayer, LogLevel logLevel) :
	_active(false), _duringProgramming(false), _delayChanged(false), _inLoop(false),
	_numProgSteps(0),_currentProgStep(0),
	_numLoops(0), _currentLoop(0), _framePlayer(framePlayer),
	_logLevel(logLevel){}

void PoiProgramHandler::init(){
	_currentProgStep = 0;
	if (_logLevel != MUTE) printf("Starting program...\n");
	 _nextProgramStep(true);
	 if (_logLevel != MUTE) _framePlayer.printInfo();
	 _active = true;
	 _delayChanged = true;
}

void PoiProgramHandler::next(){

	_framePlayer.next();

	if (!_framePlayer.isActive()){
		// end of play action
		if (_isProgramFinished()) {
			_active = false;
		}
		else {
			_nextProgramStep();
			 if (_logLevel != MUTE) _framePlayer.printInfo();
			// set timer in any case for a new command
			_delayChanged = true;
		}
	}
	else {
		_delayChanged = false;
	}
}

bool PoiProgramHandler::isActive(){
	return _active;
}

bool PoiProgramHandler::hasDelayChanged(){
	return _delayChanged;
}

uint8_t PoiProgramHandler::getCurrentScene(){
  return _framePlayer.getCurrentScene();
}

uint8_t PoiProgramHandler::getCurrentFrame(){
  return _framePlayer.getCurrentFrame();
}

uint16_t PoiProgramHandler::getDelayMs(){
	return _framePlayer.getDelayMs();
}

bool PoiProgramHandler::_isProgramFinished(){
  return _currentProgStep + 1 >= _numProgSteps;
}

CmdType PoiProgramHandler::_getCommandType(uint8_t cmd[6]){
  return (CmdType) cmd[0];
}

void PoiProgramHandler::addCmdToProgram(char cmd[7]){

  if (_numProgSteps >= N_PROG_STEPS){
    printf("Error. Number of programming steps exceeds maximum (%d).\n", N_PROG_STEPS);
    // reset current program
    _clearProgram();
  }

  if ((CmdType) cmd[1] == PROG_END){
    if (_logLevel != MUTE) {
      printf("Program loaded: %d cmds, %d labels, %d sync points.\n",
        _numProgSteps, _labelMap.size(), _syncMap.size());
    }
    // finished programming
    _duringProgramming = false;
    return;
  }

  if (!_duringProgramming) {
    if (_logLevel != MUTE) printf("Starting to read a program...\n" );
    _clearProgram();
    _duringProgramming = true;
    _numProgSteps = 0;
  }

  if ((CmdType) cmd[1] == LABEL){
    // keep labels separate in a map along with (next) cmd number
    if (cmd[2] != 0) {
      _labelMap[cmd[2]] = _numProgSteps;
    }
    else {
      printf("Error. Label code cannot be zero.\n" );
    }
    if (cmd[3] != 0) {
      _syncMap[cmd[3]] = _numProgSteps;
    }
  }

  // add cmd to program memory
  _prog[_numProgSteps][0] = (CmdType) cmd[1];
  for (int i=2; i<7; i++){
    _prog[_numProgSteps][i-1] = (uint8_t) cmd[i];
  }

  _numProgSteps++;
}

bool PoiProgramHandler::_jumpToLabel(uint8_t label){
  if (label == 0){
      printf("Error. Loop jump label 0 not allowed.\n");
      return false;
  }

  std::map<uint8_t,uint8_t>::iterator it = _labelMap.find(label);
  if (it != _labelMap.end()) {
    _currentProgStep = it->second;
    return true;
  }

  printf("Error. Loop jump label %d not found.\n", label);
  return false;
}

void PoiProgramHandler::_nextProgramStep(bool initial){
	bool noinitialcount = initial;
  while (!_isProgramFinished()){
		if (noinitialcount){
			// don't count on initial call
    	_evaluateCommand(_currentProgStep);
			noinitialcount = false;
		}
		else {
			_evaluateCommand(++_currentProgStep);
		}
    // continue until we hit a play command
    if (_getCommandType(_prog[_currentProgStep]) == PLAY_FRAMES){
      break;
    }
  }
}

void PoiProgramHandler::_evaluateCommand(uint8_t index) {

  if (_logLevel != MUTE) printf("Evaluating program line %d...\n", index);

  uint8_t* cmd = _prog[index];
  switch(_getCommandType(cmd)) {

    case SET_SCENE:
		_framePlayer.setActiveScene(constrain(cmd[1],0,N_SCENES-1));
    break;

    case PLAY_FRAMES:
		_framePlayer.init(constrain(cmd[1],0,N_FRAMES-1), constrain(cmd[2],0,N_FRAMES-1),
											((uint16_t)cmd[3] * 256 + cmd[4]), 1);
    break;

    case LOOP:
    if (_inLoop){
      _currentLoop++;
      printf("Loop current loop count: %d\n",_currentLoop+1 );
      if (_currentLoop + 1 >= _numLoops){
        if (_logLevel != MUTE) printf("Loop finished.\n" );
        _inLoop = false;
        break;
      }

      // jump to label
      if (!_jumpToLabel(cmd[3])){
        printf("Error. Loop aborded.\n" );
        _inLoop = false;
      }
      // continue whether loop was successfull or not
      break;
    }
    // first time hit
    _numLoops = (uint16_t)cmd[1] * 256 + cmd[2];
    if (_logLevel != MUTE) printf("Loop numLoops:%d.\n",_numLoops );
    _inLoop = true;
    if (!_jumpToLabel(cmd[3])){
      printf("Error. Loop aborded.\n" );
      _inLoop = false;
    }
    break;

    default:
    break;
  }
}

void PoiProgramHandler::_clearProgram(){
  _duringProgramming = false;
  _inLoop = false;
	_delayChanged = false;
	_active = false;

	_numLoops = 0;
  _numProgSteps = 0;
	_currentLoop = 0;
	_currentProgStep = 0;

  _labelMap.clear();
  _syncMap.clear();
}

bool PoiProgramHandler::checkProgram(){
	if (_duringProgramming){
    printf("Error. Cannot start a program when upload is not completed.\n" );
    return false;
  }
  if (_numProgSteps == 0){
    printf("Error. Cannot start program with no commands.\n" );
    return false;
  }
	if (_currentProgStep + 1 > _numProgSteps){
		printf("Error. current program step > number of program steps.\n" );
		return false;
	}
  return true;
}

void PoiProgramHandler::printInfo(){
  printf("ProgramHandler: %d cmds, %d labels, %d sync points.\n", _numProgSteps, _labelMap.size(), _syncMap.size());
	_framePlayer.printInfo();
}

void PoiProgramHandler::printState(){
  printf("ProgramHandler: Active: %d Current cmd: %d [%d %d %d %d %d].\n", _currentProgStep,
		_prog[_currentProgStep][0], _prog[_currentProgStep][1], _prog[_currentProgStep][2], _prog[_currentProgStep][3], _prog[_currentProgStep][4]);
	_framePlayer.printState();
}
