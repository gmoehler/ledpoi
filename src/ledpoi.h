#ifndef LEDPOI_H
#define LEDPOI_H

#define N_SCENES 1
#define N_FRAMES 200 // max: 255 because of uint8_t
#define N_PIXELS 60

enum LogLevel { CHATTY, QUIET, MUTE};

enum CmdType {  PROG_END,
                SET_SCENE,
                PLAY_FRAMES,
                GOTO,
                GOTO_FADE,
                LOOP,
                LABEL
              };


#endif
