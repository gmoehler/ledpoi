#include "ledpoi.h"
#include "ledpoi_utils.h"

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

    esp_log_level_set(DSPCH_T, ESP_LOG_DEBUG);       // dispatch task
    esp_log_level_set(DISP_T,  DEFAULT_LOG_LEVEL);   // display task
    esp_log_level_set(WIFI_T,  ESP_LOG_DEBUG);       // wifi task
    esp_log_level_set(UART_T,  ESP_LOG_DEBUG);       // uart task
    esp_log_level_set(PROG_T,  ESP_LOG_DEBUG);       // program task
    esp_log_level_set(PLAY_T,  ESP_LOG_DEBUG);       // play task
    esp_log_level_set(MEM_T,   DEFAULT_LOG_LEVEL);   // memory task
    esp_log_level_set(SELF_T,  ESP_LOG_DEBUG);       // selftest task
    esp_log_level_set(BUT_T,  DEFAULT_LOG_LEVEL);    // button task
    esp_log_level_set(EXPL_T,  DEFAULT_LOG_LEVEL);   // example task

    esp_log_level_set(PLAYF_A, ESP_LOG_INFO);        // play frames action
    esp_log_level_set(NOACT_A, DEFAULT_LOG_LEVEL);   // void ("no") action
    esp_log_level_set(ANIM_A,  DEFAULT_LOG_LEVEL);   // animation action

    esp_log_level_set(TIMER,   DEFAULT_LOG_LEVEL);   // timer util
    esp_log_level_set(POICMD,  DEFAULT_LOG_LEVEL);   // poi command util  
    esp_log_level_set(ICACHE,  DEFAULT_LOG_LEVEL);   // image cache util  
    esp_log_level_set(PCACHE,  DEFAULT_LOG_LEVEL);   // program cache util 
    esp_log_level_set(RWIFIS,  ESP_LOG_DEBUG);       // Robust wifi server
    esp_log_level_set(FLASH,   DEFAULT_LOG_LEVEL);   // flash memory
    esp_log_level_set(PROGH,   DEFAULT_LOG_LEVEL);   // program handler
    esp_log_level_set(INTS,    DEFAULT_LOG_LEVEL);   // interaction state
    esp_log_level_set(SELF_H,    ESP_LOG_DEBUG);   // selftest helper
}

void setup() {
  uart_setup(); // first one because this sets serial baudrate
  logging_setup();

  // setup tasks and queues with sizes
  button_setup();
  memory_setup(10); 
  dispatch_setup(10);
  display_setup(5);       
  player_setup(5);
  program_setup(3);
  wifi_setup(3);

  // start tasks with prios
  button_start(8);
  memory_start(8);     
  dispatch_start(7);
  program_start(6, 5);
  player_start(4);
  display_start(3); 
  wifi_start(5);
  uart_start(5);

  // send wifi start command to central dispatch queue after everything is set up
  /* 
  PoiCommand cmdStartWifi ({CONNECT, 0, 0, 0, 0, 0});
  sendToDispatch(rawCmdStartWifi, WIFI_T);
  */

	// start selftest
  // selftest_start(5);
   
  // fill queues with example values
  PoiCommand cmdWorm ( {ANIMATE, RAINBOW, 1, 5, 0, 100} ); 
  sendToDispatch(cmdWorm, WIFI_T);
  PoiCommand cmdBlack ( {SHOW_RGB, 0, 0, 0, 0, 0} ); // black
  sendToDispatch(cmdBlack, WIFI_T);
}

// everything works with tasks, we dont need the loop...
void loop(){
  delay(100000); // snow white sleep 
}