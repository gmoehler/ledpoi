#include "ledpoi.h"
#include "ledpoi_utils.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "dispatch/dispatchTask.h"
#include "display/displayTask.h"
#include "spiffs/spiffsTask.h"
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
    // esp_log_level_set(SELF_T,  DEFAULT_LOG_LEVEL);   // selftest task
    esp_log_level_set(BUT_T,   DEFAULT_LOG_LEVEL);   // button task
    esp_log_level_set(EXPL_T,  DEFAULT_LOG_LEVEL);   // example task
    esp_log_level_set(SPIF_T,  ESP_LOG_DEBUG);   // spiffs task

    esp_log_level_set(TIMER,   DEFAULT_LOG_LEVEL);   // timer util
    esp_log_level_set(POICMD,  DEFAULT_LOG_LEVEL);   // poi command util  
    esp_log_level_set(INTS,    DEFAULT_LOG_LEVEL);   // interaction state
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
  // button_setup();
  dispatch_setup(10);
  spiffs_setup(3);
  display_setup(10);       

  // start tasks with prios
  // button_start(8); 
  dispatch_start(7);
  spiffs_start(6);
  display_start(9); 

  // display loaded image
  sendRawToDispatch({LOAD_CHAN, 0, 0, 0, 0, 0}, MAIN_T); 
}

// everything works with tasks, we dont need the loop...
void loop(){
  delay(100000); // snow white sleep 
}