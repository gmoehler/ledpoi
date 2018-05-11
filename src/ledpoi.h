#ifndef LEDPOI_H
#define LEDPOI_H

// default compile time logging level need to be defined
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
//#define CONFIG_LOG_COLORS 1 // does not work in VSC terminal

#ifndef WITHIN_UNITTEST
  #include <Arduino.h>
  #include "esp_log.h"
#else
  #include "../test/mock_Arduino.h"
  #include "../test/mock_esp.h"
#endif


// need to define my own logging macros since other are not activated by LOG_LOCAL_LEVEL as expected
#ifndef LOG_MACROS
#define LOG_MACROS
#define LOGE( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR)   { esp_log_write(ESP_LOG_ERROR,   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define LOGW( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_WARN)    { esp_log_write(ESP_LOG_WARN,    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define LOGI( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO)    { esp_log_write(ESP_LOG_INFO,    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define LOGD( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG)   { esp_log_write(ESP_LOG_DEBUG,   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define LOGV( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_VERBOSE) { esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
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
// 2M / ( 4096 * N_NUM_IMAGE_SECTIONS ) = 49
// assuming a 1.5MB flash partition, this is less than
// 1503232 / ( 4096 * N_NUM_IMAGE_SECTIONS ) = 36
#define N_SCENES 36 // 49

#define N_CMD_FIELDS 6

// this is due to max NVS length of 1984 bytes
#define N_PROG_STEPS 330

// serial baud rate
//#define UART_BAUD 115200
#define UART_BAUD 921600

#define WS2812_PIN 23 

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

// setup file tags for tagger

#define DSPCH_T "DIPT"  // dispatch task
#define DISP_T  "DIST"   // display task
#define MEM_T   "MEMT"   // memory task
#define WIFI_T  "WIFT"   // wifi task
#define UART_T  "UART"   // wifi task
#define PLAY_T  "PLAT"   // player task
#define PROG_T  "PRGT"   // program task
#define SELF_T  "SELT"   // selftest task
#define EXPL_T  "EXPT"   // example task

#define NOACT_A "NOAA"   // void ("no") action
#define PLAYF_A "PLFA"   // play frames action
#define SRGB_A  "SRGA"   // show rgb action
#define ANIM_A  "ANIA"   // animation action

#define POICMD  "COMU"   // poi command
#define TIMER   "TIMU"   // poi timer util
#define ICACHE  "ICHU"   // image cache util
#define FLASH   "FLAU"   // flash memory helper
#define PROGH   "PGHU"   // program handler 
#define PCACHE  "PCHU"   // program cache
#define INTS    "INTS"   // interaction state

#endif
