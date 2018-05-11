#include "PoiCommand.h"

const char *commandNames [] = {
    "HEAD_SCENE",
    "TAIL_SCENE",
    "SAVE_SCENE",
    "HEAD_PROG",
    "TAIL_PROG",
    "SAVE_PROG",
    "INIT_FLASH",
    "LOAD_SCENE",
    "SHOW_RGB",
    "PLAY_FRAMES",
    "ANIMATE",
    "DISPLAY_IP",
    "DIM",
    "FADE2FRAME",
    "START_PROG",
    "STOP_PROC",
    "PAUSE_PROC",
    "JUMP2SYNC",
    "CONNECT",
    "DISCONNECT",
    "CLIENT_DISCON",
    "USE_SSID",
    "LOOP_START",
    "SYNC_POINT",
    "LOOP_END"
};

const char *internalCommandNames [] = {
    "POISTATUS",
    "SELFTEST",
    "BUT0_CLICK",
    "BUT0_LONGCLICK",
    "BUT0_RELEASE",
};

const char* commandNameNoCommand = "NO_COMMAND      ";
const char* commandNameSetPixel  = "SET_PIXEL       ";
const char* commandNameUnknownCmd  = "UNKNOWN_COMMAND ";

PoiCommand::PoiCommand()
    :_type(NO_COMMAND){}

PoiCommand::PoiCommand(RawPoiCommand rawCmd) {
    _rawCmd = rawCmd;

    // determine command type
    uint8_t f0 = _rawCmd.field[0];
    if (f0 < N_PIXELS) {
        _type = SET_PIXEL;
    }
    else if (f0 < END_PREDEF_COMMANDS 
             || (f0 > START_INTERNAL_COMMANDS
                 && f0 < END_INTERNAL_COMMANDS)){
        _type = static_cast<PoiCommandType>(f0);
    }
    else if (f0 == 255) {
        _type = NO_COMMAND;
    }
    else {
        _type = NO_COMMAND;
        LOGE(POICMD, "Unknown raw command: %s", rawCommandToString(rawCmd).c_str());
    }
}

PoiCommandType& PoiCommand::getType(){
    return _type;
}

void PoiCommand::setType(PoiCommandType type){
    _type = type;
}    

bool PoiCommand::isEmpty(){
    return (_type == NO_COMMAND);
}    

RawPoiCommand PoiCommand::getRawPoiCommand(){
    return _rawCmd;
}

String PoiCommand::commandTypeToString(){
    char buffer [18];
    uint8_t f0 = _rawCmd.field[0];
    switch(_type){
        case NO_COMMAND:
            snprintf(buffer, 18, "%-16s", commandNameNoCommand);
            break;
        case SET_PIXEL:
            snprintf(buffer, 18, "%-16s", commandNameSetPixel);
            break;
        default:
            if (f0 >= 192 && f0 < END_PREDEF_COMMANDS){ 
                snprintf(buffer, 18, "%-16s", commandNames[f0-192]);
            }
            else if (f0 > START_INTERNAL_COMMANDS && f0 < END_INTERNAL_COMMANDS){ 
                snprintf(buffer, 18, "%-16s", internalCommandNames[f0-START_INTERNAL_COMMANDS-1]);
            }
            else {
                snprintf(buffer, 18, "%-16s", commandNameUnknownCmd);
            }
            break;
    }
    return String(buffer);
}

String PoiCommand::toString(){
    char buffer[44];
    snprintf(buffer, 44, "%s  %s", 
        commandTypeToString().c_str(),
        rawCommandToString(_rawCmd).c_str());
    return String(buffer);
}

String rawCommandToString(RawPoiCommand rawCmd){
    char buffer[40];
    snprintf(buffer, 40, "%3d %3d %3d %3d %3d %3d", 
        rawCmd.field[0],  rawCmd.field[1],  rawCmd.field[2],
        rawCmd.field[3],  rawCmd.field[4],  rawCmd.field[5]);
    return String(buffer);
}

bool PoiCommand::operator == (const PoiCommand &b) const {
    for (int i=0; i< N_CMD_FIELDS; i++) {
        if (_rawCmd.field[i] != b._rawCmd.field[i]){
            return false;
        }
    }
    return true;
}

uint8_t PoiCommand::getField(int idx) {
	if (idx > N_CMD_FIELDS-1){
		LOGE(WIFI_T, "field %d of cmd %s does not exist", idx, toString().c_str());               
		return 0;
	}
	return _rawCmd.field[idx];
}

uint16_t PoiCommand::getCombinedField(int idx1, int idx2) {
	if (idx1 > N_CMD_FIELDS-1 || idx2 > N_CMD_FIELDS-1 ){
		LOGE(WIFI_T, "field %d or %d of cmd %s does not exist", idx1, idx2, toString().c_str());               
		return 0;
	}
	return (uint16_t) _rawCmd.field[idx1] * 256 + _rawCmd.field[idx2] ;
}
	
bool PoiCommand::isActionCommand() {
	return 
            _type == SHOW_RGB ||
            _type == PLAY_FRAMES ||
            _type == ANIMATE ||
            _type == DISPLAY_IP || 
            _type == FADE2FRAME;
}
	
bool PoiCommand::isPlayableCommand() {
    return 
            _type == LOAD_SCENE ||
            _type == DIM ||
            isActionCommand();
}

bool PoiCommand::isProgramStatement() {
    return  
            _type == LOOP_START || 
            _type == LOOP_END || 
            _type == SYNC_POINT ||
	        isPlayableCommand();
}

bool PoiCommand::isProgramControlCommand() {
	return
            _type == START_PROG || 
            _type == STOP_PROC || 
            _type == PAUSE_PROC || 
            _type == JUMP2SYNC;
}

bool PoiCommand::isWifiControlCommand() {
	return 
            _type == CONNECT || 
            _type == DISCONNECT || 
            _type == CLIENT_DISCON || 
            _type == USE_SSID;
}

bool PoiCommand::isMemoryControlCommand() {
	return 
            _type == SET_PIXEL || 
            _type == HEAD_SCENE || 
            _type == TAIL_SCENE || 
            _type == SAVE_SCENE || 
            _type == HEAD_PROG || 
            _type == TAIL_PROG || 
            _type == SAVE_PROG || 
            _type == INIT_FLASH;
}

bool PoiCommand::isInternalCommand() {
	return 
		static_cast<int>(_type) > static_cast<int>(START_INTERNAL_COMMANDS);
}