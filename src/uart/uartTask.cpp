#include "uartTask.h"

#define EX_UART_NUM UART_NUM_0
#define ECHO_TEST_TXD  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_RXD  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (4096)

static char buffer[80];

void uartSendCommand(PoiCommand cmd){

  PoiCommandType type = cmd.getType();
  RawPoiCommand rawCmd = cmd.getRawPoiCommand();

  if (type == SET_PIXEL) {
    LOGV(UART_T, "Sending cmd: %s", cmd.toString().c_str());
  }
  else {
    LOGV(UART_T, "Sending cmd: %s", cmd.toString().c_str());
  }

  // send command to central dispatch queue
  if (xQueueSendToBack(dispatchQueue, &(rawCmd),  portMAX_DELAY) != pdTRUE){
    LOGE(UART_T, "Could not add command to dispatchQueue: %s", cmd.toString().c_str());
  }
}

void uartTask(void* arg)
{
    RawPoiCommand rawCmd;
    uint8_t b;
    int i = 0;

    while (1) {
      // Read data from the UART
      int len = uart_read_bytes(EX_UART_NUM, &b, 1, (portTickType)portMAX_DELAY);
      // echo it back to UART
      //uart_write_bytes(EX_UART_NUM, (const char*) &b, 1);

      // initial flag 255
      if (b==255){
        i=0;
      }
      else {
        i++;
      }

      // read fields
      if (i>0 && i<N_CMD_FIELDS+1){
        rawCmd.field[i-1] = b;
        // cmd complete
        if (i==N_CMD_FIELDS){
          PoiCommand cmd(rawCmd);
          if (cmd.getType() != NO_COMMAND){

            uartSendCommand(cmd);
          }
       }
    }
  }
}

void uart_setup(){
      /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(EX_UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
}

void uart_start(uint8_t prio){ 
  xTaskCreate(uartTask, "uartTask", 4096, NULL, prio, NULL);
}
