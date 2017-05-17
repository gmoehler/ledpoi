#ifndef POI_FLASH_MEMORY_H
#define POI_FLASH_MEMORY_H

#include <Arduino.h>
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs.h"

#define STORAGE_NAMESPACE "poiStorage"
#define IMAGE_NAMESPACE "poiImage"
#define PROGRAM_NAMESPACE "poiProgram"


/**
 * Interface to Non-volatile (flash) memory for image and program
 **/


class PoiFlashMemory
{
public:
  void saveImage(uint8_t *imageData, uint8_t sizex, uint8_t sizey);
  void saveProgram(uint8_t *programData, uint8_t sizex, uint8_t sizey);
  
  void getImage(uint8_t *imageData,);
  void getProgram(uint8_t *programData);
  
  esp_err_t save_restart_counter(void);
  esp_err_t save_run_time(void);
  esp_err_t print_what_saved(void);
  void setup();

private:
};


#endif
