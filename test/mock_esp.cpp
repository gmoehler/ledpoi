#include "mock_esp.h"

xQueueHandle dispatchQueue;
xQueueHandle playerQueue;
xQueueHandle spiffsQueue;
xQueueHandle displayQueue;

void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...)
{
    va_list list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
}

uint32_t esp_log_timestamp() {
    return 0;
}

bool xQueueSendToBack( xQueueHandle, void * , int ) {
  return true;
}

void xQueueReset(xQueueHandle){
    // nothing in mock
}
uint8_t uxQueueMessagesWaiting(xQueueHandle) {
    return 1;
}
