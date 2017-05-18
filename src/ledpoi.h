#ifndef LEDPOI_H
#define LEDPOI_H

#define N_SCENES 1
#define N_FRAMES 200 // max: 255 because of uint8_t
#define N_PIXELS 60

#define N_PROG_STEPS 50
#define N_CMD_FIELDS 6

enum LogLevel { CHATTY, QUIET, MUTE};

enum CmdType {  PROG_END,
                LABEL,
                SYNC_POINT,
                PLAY_FRAMES,
                LOOP,
                SET_SCENE,
                PRESET_RGB,
                FADE_TO_RGB,
                FADE_TO_FRAME
              };


#endif
