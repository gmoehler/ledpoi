#ifndef POI_FLASH_MEMORY_H
#define POI_FLASH_MEMORY_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <nvs.h>
#include "ledpoi.h"

#define STORAGE_NAMESPACE "poiStorage"
#define IMAGE_NAMESPACE "poiImage"
#define PROGRAM_NAMESPACE "poiProgram"

/**
 * Interface to Non-volatile (flash) memory for image and program
 **/

class PoiFlashMemory
{
public:
  void saveImage(uint8_t *imageData, int size_x, int size_y);
  void saveProgram(uint8_t *programData, int size_x, int size_y);

  // get-functions assume that there is enough space in the data
  void getImage(uint8_t *imageData);
  void getProgram(uint8_t *programData);

  esp_err_t save_restart_counter(void);
  esp_err_t save_run_time(void);
  esp_err_t print_what_saved(void);
  void setup();

private:
  esp_err_t _save_uint8(const char* mynamespace, const char* key, uint8_t *data, uint8_t size_x, uint8_t size_y);
  esp_err_t _read_uint8(const char* mynamespace, const char* key, uint8_t *data);

};


#endif
