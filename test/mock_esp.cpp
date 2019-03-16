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

uint8_t* _fileData;
uint16_t _fileIdx = 0;
uint16_t _numElem = 0;

void setFileData(uint8_t* data, uint16_t numElem) {
    _fileData = data;
    _fileIdx = 0;
    _numElem = numElem;
    printf("Data: %d elements\n", numElem);
}

void fs::File::close() {
    return;
}

int fs::File::read(){
    if (_fileIdx < _numElem) {
        // printf("Reading %d: %d\n", _fileIdx, _fileData[_fileIdx]);
        return _fileData[_fileIdx++];
    }
    printf("WARN: Reading over bounds\n");
    return 0;
}

int fs::File::available() {
    return _fileIdx < _numElem;
}

bool fs::File::operator!(){
    return true;
}

bool SpiffsClass::begin() {
    return true;
}
    
bool SpiffsClass::begin(bool){
    return true;
}

fs::File SpiffsClass::open(const char*){
    fs::File *file = new fs::File;
    return *file;
}

