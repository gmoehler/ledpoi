#ifndef COMMAND_H
#define COMMAND_H

#include "ledpoi.h"

enum PoiCommandType {
    SET_PIXEL=0,
    HEAD_SCENE=192,
    TAIL_SCENE,
    SAVE_SCENE,
    HEAD_PROG,
    TAIL_PROG,
    SAVE_PROG,
    INIT_FLASH,
    LOAD_SCENE,
    SHOW_RGB = 200,
    PLAY_FRAMES,
    ANIMATE,
    DISPLAY_IP,
    DIM,
    FADE2FRAME,
    START_PROG,
    STOP_PROC,
    PAUSE_PROC,
    JUMP2SYNC,
    CONNECT = 210,
    DISCONNECT,
    CLIENT_DISCON,
    USE_SSID,
    LOOP_START,
    SYNC_POINT,
    LOOP_END=216,
    END_PREDEF_COMMANDS,

    START_INTERNAL_COMMANDS=230,//  internal commands/events follow
    POISTATUS,				    // queue status
	BUTTON0_CLICK,              // event for button 0 was clicked
    BUTTON0_LONGCLICK,          // event for button 0 was clicked a long time
    BUTTON0_RELEASE,            // event for button 0 was pressed very long and released
    PLAYER_ACTIVE,              // status event for start of player action
    PLAYER_INACTIVE,            // status event for end of player action
	END_INTERNAL_COMMANDS,
    
    NO_COMMAND=255          // default
};

extern const char *commandNames [];

struct RawPoiCommand {   
    uint8_t field[N_CMD_FIELDS];       // fields of the command 
};

String rawCommandToString(RawPoiCommand rawCmd);

class PoiCommand {
public:
    PoiCommand();
    PoiCommand(RawPoiCommand rawCmd);
    PoiCommandType& getType();
    void setType(PoiCommandType type);
    bool isEmpty();
    RawPoiCommand getRawPoiCommand();
    String commandTypeToString();
    String toString();
    bool operator == (const PoiCommand &b) const;
    uint8_t getField(int id);
    uint16_t getCombinedField(int id1, int id2);
 
    bool isActionCommand();  // an actual play action
    bool isPlayableCommand(); // anything the player understands
    bool isProgramStatement(); // anything that can be a prog. statement
    bool isProgramControlCommand(); // control cmds for program task
    bool isWifiControlCommand(); // control cmds for wifi task
    bool isMemoryControlCommand(); // control cmds for memory task
    bool isInternalCommand(); // commands/events issued intetnally
    
private:
    PoiCommandType _type;   // the type of the command      
    RawPoiCommand _rawCmd;  // raw command
};

#endif