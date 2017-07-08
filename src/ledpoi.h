#ifndef LEDPOI_H
#define LEDPOI_H

#ifndef WITHIN_UNITTEST
  #include <ws2812.h>
#else
  #include "../test/mock_ws2812.h"
#endif

// maximum number of pois that can be configured
#define N_POIS 10

// on flash image must fit into a multiple of SPI_FLASH_SEC_SIZE (4096)
// in 2MB there are 256 sections available
#define N_NUM_FLASH_SECTIONS_PER_IMAGE 10
// this is int( N_NUM_IMAGE_SECTIONS*4096/(N_PIXELS*3) )
// max is 255 because of uint8_t
#define N_FRAMES 227
#define N_PIXELS 60

// assuming a 2MB flash partition, this is less than
// 2M / ( 4096 * N_NUM_IMAGE_SECTIONS )
#define N_SCENES 49

#define N_PROG_STEPS 512
#define N_PROG_FIELDS 5

#define N_REGISTERS 2

enum LogLevel { CHATTY, QUIET, MUTE};

enum CmdType {  PROG_END,     // 0
                LABEL,        // 1
                SYNC_POINT,   // 2
                PLAY_FRAMES,  // 3
                LOOP,         // 4 
                SET_SCENE,    // 5
                PRESET_RGB,   // 6
                FADE_TO_RGB,  // 7
                FADE_TO_FRAME,// 8
                JUMPTO          // 9
              };

// currently mainly used for playWorm()
enum Color {       WHITE,   // 0
                   BLACK,   // 1
                   RED,     // 2
                   GREEN,   // 3
                   BLUE,    // 4
                   YELLOW,  // 5
                   LILA,
                   CYAN,
                   RAINBOW,  // 8
                   PALE_WHITE
                   };


rgbVal makeRGBValue(uint8_t *rgb_array);

rgbVal makeRGBValue(Color color, uint8_t brightness=255);


#endif
