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
#include "example/exampleTask.h"

// unless defined differently below this is the default log level
#define DEFAULT_LOG_LEVEL ESP_LOG_DEBUG

void logging_setup(){

    esp_log_level_set(DSPCH_T, DEFAULT_LOG_LEVEL);   // dispatch task
    esp_log_level_set(DISP_T,  DEFAULT_LOG_LEVEL);   // display task
    esp_log_level_set(WIFI_T,  DEFAULT_LOG_LEVEL);   // wifi task
    esp_log_level_set(UART_T,  DEFAULT_LOG_LEVEL);   // uart task
    esp_log_level_set(PROG_T,  DEFAULT_LOG_LEVEL);   // program task
    esp_log_level_set(PLAY_T,  DEFAULT_LOG_LEVEL);   // play task
    esp_log_level_set(MEM_T,   DEFAULT_LOG_LEVEL);   // memory task
    esp_log_level_set(EXPL_T,  DEFAULT_LOG_LEVEL);   // example task

    esp_log_level_set(PLAYF_A, DEFAULT_LOG_LEVEL);   // play frames action
    esp_log_level_set(NOACT_A, DEFAULT_LOG_LEVEL);   // void ("no") action
    esp_log_level_set(ANIM_A,  DEFAULT_LOG_LEVEL);   // animation action

    esp_log_level_set(TIMER,   DEFAULT_LOG_LEVEL);   // timer util
    esp_log_level_set(POICMD,  DEFAULT_LOG_LEVEL);   // poi command util  
    esp_log_level_set(ICACHE,  DEFAULT_LOG_LEVEL);   // image cache util  
    esp_log_level_set(PCACHE,  DEFAULT_LOG_LEVEL);   // program cache util 
    esp_log_level_set(RWIFIS,  ESP_LOG_INFO);        // Robust wifi server
    esp_log_level_set(FLASH,   ESP_LOG_INFO);        // flash memory
    esp_log_level_set(PROGH,   DEFAULT_LOG_LEVEL);   // program handler
    esp_log_level_set(INTS,    DEFAULT_LOG_LEVEL);   // interaction state
}

void setup() {
  logging_setup();

  // setup tasks and queues with sizes
  button_setup();
  memory_setup(10); 
  dispatch_setup(10);
  display_setup(5);       
  player_setup(5);
  program_setup(3);
  wifi_setup(3);
  uart_setup();

  // start tasks with prios
  memory_start(8);     
  dispatch_start(7);
  program_start(6, 5);
  player_start(4);
  display_start(3); 
  wifi_start(5);
  uart_start(5);

  // send wifi start command to central dispatch queue after everything is set up
  RawPoiCommand rawCmdStartWifi = {CONNECT, 0, 0, 0, 0, 0}; 
  PoiCommand cmdStartWifi (rawCmdStartWifi);
  if (xQueueSendToBack(dispatchQueue, &(rawCmdStartWifi),  portMAX_DELAY) != pdTRUE){
    LOGE(WIFI_T, "Main: Could not add command to dispatchQueue: %s", cmdStartWifi.toString().c_str());
  }

  // fill queues with example values
  example_start(WORM_EXAMPLE, 5); 
}

// everything works with tasks, we dont need the loop...
void loop(){
  delay(100000); // snow white sleep 
}