#include "ledpoi.h"
#include "ledpoi_utils.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "dispatch/dispatchTask.h"
#include "display/displayTask.h"
#include "player/playerTask.h"
#include "uart/uartTask.h"
#include "ui/button.h"
// #include "selftest/selftestTask.h"

// unless defined differently below this is the default log level
#define DEFAULT_LOG_LEVEL ESP_LOG_INFO

void logging_setup(){

    esp_log_level_set(MAIN_T,  ESP_LOG_DEBUG);   // main program task
    esp_log_level_set(DSPCH_T, ESP_LOG_DEBUG);   // dispatch task
    esp_log_level_set(DISP_T,  DEFAULT_LOG_LEVEL);   // display task
    esp_log_level_set(UART_T,  DEFAULT_LOG_LEVEL);   // uart task
	  esp_log_level_set(PLAY_T,  ESP_LOG_DEBUG);   // play task
    esp_log_level_set(SPIF_T,  ESP_LOG_DEBUG);   // spiffs task
    // esp_log_level_set(SELF_T,  DEFAULT_LOG_LEVEL);   // selftest task
    esp_log_level_set(BUT_T,   DEFAULT_LOG_LEVEL);   // button task
    esp_log_level_set(EXPL_T,  DEFAULT_LOG_LEVEL);   // example task
	
	  esp_log_level_set(SPIFF_A, ESP_LOG_DEBUG);   // play frames action
    esp_log_level_set(NOACT_A, DEFAULT_LOG_LEVEL);   // void ("no") action
    esp_log_level_set(ANIM_A,  DEFAULT_LOG_LEVEL);   // animation action

    esp_log_level_set(TIMER,   DEFAULT_LOG_LEVEL);   // timer util
    esp_log_level_set(POICMD,  DEFAULT_LOG_LEVEL);   // poi command util  
    esp_log_level_set(INTS,    DEFAULT_LOG_LEVEL);   // interaction state
    esp_log_level_set(SPIF_U,  ESP_LOG_DEBUG);   // spiffs utils
    // esp_log_level_set(SELF_H,  DEFAULT_LOG_LEVEL);   // selftest helper
}

void disableBrownOut() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
}

void setup() {

  disableBrownOut();

  // uart_setup(); // first one because this sets serial baudrate
  logging_setup();

  // setup tasks and queues with sizes
  button_setup();
  dispatch_setup(10);
  display_setup(10);  
  player_setup(5);     

  // start tasks with prios
  button_start(8); 
  dispatch_start(7);
  player_start(4);
  display_start(9); 

  sendRawToDispatch({ANIMATE, RAINBOW, 1, 5, 0, 100}, MAIN_T);
  sendRawToDispatch({SHOW_RGB, 0, 0, 0, 2, 254}, MAIN_T); // black
  
  // waiting for button interaction
}

// everything works with tasks, we dont need the loop...
void loop(){
  delay(100000); // snow white sleep 
}