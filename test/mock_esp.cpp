#include "mock_esp.h"

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