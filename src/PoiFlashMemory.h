#ifndef POI_FLASH_MEMORY_H
#define POI_FLASH_MEMORY_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <ws2812.h>
#include "ledpoi.h"

#define NVS_IMAGE_NAMESPACE "poiImage"
#define NVS_PROGRAM_NAMESPACE "poiProgram"

#define NVS_PROGRAM_KEY "program"
#define NVS_NUM_PROG_STEPS_KEY "numProgSteps"
#define NVS_NUM_SCENES_KEY "numScenes"

/**
 * Interface to flash partition memory for image and Non-volatile (NVS) memory for the program
 * Image data on flash is organized into fixed image memory segements of size
 * N_NUM_IMAGE_SECTIONS *
 **/

class PoiFlashMemory
{
public:
  bool saveImage(uint8_t scene, uint8_t* imageData);
  bool loadImage(uint8_t scene, uint8_t* imageData);

  bool saveProgram(uint8_t* programData, uint8_t size_x, uint8_t size_y);
  bool loadProgram(uint8_t* programData);

  bool saveNumProgramSteps(uint8_t numProgSteps);
  bool loadNumProgramSteps(uint8_t* numProgSteps);

  bool saveNumScenes(uint8_t numScenes);
  bool loadNumScenes(uint8_t* numScenes);

  bool eraseImages();
  bool eraseProgram();

  bool eraseNvsFlashPartition();

  uint32_t getSizeOfImageSection();

private:
  bool _imagePartitionInitialized = false;
  
  void  _checkImagePartitionInitialized();
  const esp_partition_t* _getDataPartition();
  esp_err_t _nvs_save_uint8_array(const char* mynamespace, const char* key, uint8_t *data,
      uint8_t size_x, uint8_t size_y, uint8_t size_z=1);
  esp_err_t _nvs_save_uint8(const char* mynamespace, const char* key, uint8_t value);
  esp_err_t _nvs_read_uint8_array(const char* mynamespace, const char* key, uint8_t *data);
  esp_err_t _nvs_read_uint8(const char* mynamespace, const char* key, uint8_t *value);
  esp_err_t _nvs_eraseCompleteNamespace(const char* mynamespace);
};


#endif
