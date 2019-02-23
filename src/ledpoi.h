#ifndef LEDPOI_H
#define LEDPOI_H

// default compile time logging level need to be defined
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
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
#define N_POIS 12

#define N_PIXELS 60
#define N_CMD_FIELDS 6

// default frame delay (in ms)
#define DEFAULT_DELAY 10

// serial baud rate
//#define UART_BAUD 115200
#define UART_BAUD 921600

// pin for led strip
#define WS2812_PIN 23 

// legacy stuff ... for interactionState... 
// ip increment which means: not connected
#define NO_CONNECTION_IPINCR 254
// timeout for ip configuration in sec
#define IP_CONFIG_TIMEOUT 20
enum Color {       WHITE,   // 0
                   BLACK,   // 1
                   RED,     // 2
                   GREEN,   // 3
                   BLUE,    // 4
                   YELLOW,  // 5
                   LILA,    // 6
                   CYAN,    // 7
                   ORANGE,  // 8
                   RAINBOW, // 9
                   PALE_WHITE // 10
                   };

// setup file tags for logger
#define MAIN_T  "MAIN"   // main program task
#define DSPCH_T "DIPT"   // dispatch task
#define DISP_T  "DIST"   // display task
#define MEM_T   "MEMT"   // memory task
#define WIFI_T  "WIFT"   // wifi task
#define UART_T  "UART"   // uart task
#define PLAY_T  "PLAT"   // player task
#define PROG_T  "PRGT"   // program task
#define SELF_T  "SELT"   // selftest task
#define BUT_T   "BUTT"   // button task
#define EXPL_T  "EXPT"   // example task
#define SPIF_T  "SPIT"   // SPIFFS task

#define NOACT_A "NOAA"   // void ("no") action
#define SPIFF_A "SPIA"   // play spiffs image action
#define SRGB_A  "SRGA"   // show rgb action
#define ANIM_A  "ANIA"   // animation action

#define POICMD  "COMU"   // poi command
#define TIMER   "TIMU"   // poi timer util
#define ICACHE  "ICHU"   // image cache util
#define FLASH   "FLAU"   // flash memory helper
#define PROGH   "PGHU"   // program handler 
#define PCACHE  "PCHU"   // program cache
#define INTS    "INTS"   // interaction state
#define SPIF_U  "SPIU"   // spiffs utils
#define SELF_H  "SELH"   // selftest helper

#endif
