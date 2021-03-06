#ifndef MOCK_ESP_H
#define MOCK_ESP_H

#include <stdio.h>
#include <stdarg.h> 
#include <stdint.h>

/**
 * @brief Log level
 *
 */
typedef enum {
    ESP_LOG_NONE,       /*!< No log output */
    ESP_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    ESP_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    ESP_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    ESP_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ESP_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} esp_log_level_t;

#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%d) %s: " format LOG_RESET_COLOR "\n"

void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...);

uint32_t esp_log_timestamp();

#define xQueueHandle int
#define pdTRUE true
#define portMAX_DELAY 1
#define portTICK_PERIOD_MS 1

bool xQueueSendToBack( xQueueHandle, void * , int );
void xQueueReset(xQueueHandle);
uint8_t uxQueueMessagesWaiting(xQueueHandle);

void setFileData(uint8_t* data, uint16_t numElem);

namespace fs
{

class File {
public:
    void close();
    int read();
    int available();
    bool operator!();
};

}

class SpiffsClass {
public:
    bool begin();
    bool begin(bool);
    fs::File open(const char*);
};

#endif
