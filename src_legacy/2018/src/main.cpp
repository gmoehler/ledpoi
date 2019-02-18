#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "memory/memoryTask.h"
#include "dispatch/dispatchTask.h"
#include "display/displayTask.h"
#include "player/playerTask.h"
#include "program/programTask.h"
#include "wifi/wifiTask.h"
#include "uart/uartTask.h"
#include "ui/button.h"
#include "selftest/selftestTask.h"

// unless defined differently below this is the default log level
#define DEFAULT_LOG_LEVEL ESP_LOG_INFO

void logging_setup(){

    esp_log_level_set(MAIN_T,  DEFAULT_LOG_LEVEL);   // main program task
    esp_log_level_set(DSPCH_T, DEFAULT_LOG_LEVEL);   // dispatch task
    esp_log_level_set(DISP_T,  DEFAULT_LOG_LEVEL);   // display task
    esp_log_level_set(WIFI_T,  ESP_LOG_DEBUG);       // wifi task
    esp_log_level_set(UART_T,  DEFAULT_LOG_LEVEL);   // uart task
    esp_log_level_set(PROG_T,  DEFAULT_LOG_LEVEL);   // program task
    esp_log_level_set(PLAY_T,  DEFAULT_LOG_LEVEL);   // play task
    esp_log_level_set(MEM_T,   DEFAULT_LOG_LEVEL);   // memory task
    esp_log_level_set(SELF_T,  DEFAULT_LOG_LEVEL);   // selftest task
    esp_log_level_set(BUT_T,   DEFAULT_LOG_LEVEL);   // button task
    esp_log_level_set(EXPL_T,  DEFAULT_LOG_LEVEL);   // example task

    esp_log_level_set(PLAYF_A, DEFAULT_LOG_LEVEL);   // play frames action
    esp_log_level_set(NOACT_A, DEFAULT_LOG_LEVEL);   // void ("no") action
    esp_log_level_set(ANIM_A,  DEFAULT_LOG_LEVEL);   // animation action

    esp_log_level_set(TIMER,   DEFAULT_LOG_LEVEL);   // timer util
    esp_log_level_set(POICMD,  DEFAULT_LOG_LEVEL);   // poi command util  
    esp_log_level_set(ICACHE,  DEFAULT_LOG_LEVEL);   // image cache util  
    esp_log_level_set(PCACHE,  DEFAULT_LOG_LEVEL);   // program cache util 
    esp_log_level_set(RWIFIS,  DEFAULT_LOG_LEVEL);   // Robust wifi server
    esp_log_level_set(WIFI_U,  DEFAULT_LOG_LEVEL);   // Robust wifi server utils
    esp_log_level_set(FLASH,   DEFAULT_LOG_LEVEL);   // flash memory
    esp_log_level_set(PROGH,   DEFAULT_LOG_LEVEL);   // program handler
    esp_log_level_set(INTS,    DEFAULT_LOG_LEVEL);   // interaction state
    esp_log_level_set(SELF_H,  DEFAULT_LOG_LEVEL);   // selftest helper
}

void disableBrownOut() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
}

void setup() {

  disableBrownOut();

  uart_setup(); // first one because this sets serial baudrate
  logging_setup();

  // setup tasks and queues with sizes
  button_setup();
  memory_setup(10); 
  dispatch_setup(10);
  display_setup(10);       
  player_setup(5);
  program_setup(3);
  wifi_setup(3);

  // start tasks with prios
  button_start(8);
  memory_start(8);     
  dispatch_start(7);
  program_start(6, 5);
  player_start(4);
  display_start(9); 
#ifndef DISABLE_WIFI
  wifi_start(8);
#endif
  uart_start(5);
   
  // animate a welcome worm
  sendRawToDispatch({ANIMATE, RAINBOW, 1, 5, 0, 100}, MAIN_T);
  sendRawToDispatch({SHOW_RGB, 0, 0, 0, 2, 254}, MAIN_T); // black

#ifndef DISABLE_WIFI
  // start ip configuration when everything is set up
  PoiCommand cmdStartWifiConfig ({START_IP_CONFIG, 0, 0, 0, 0, 0});
  sendToDispatch(cmdStartWifiConfig, MAIN_T); 
#endif
}

// everything works with tasks, we dont need the loop...
void loop(){
  delay(100000); // snow white sleep 
}