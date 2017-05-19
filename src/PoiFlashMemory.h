#ifndef POI_FLASH_MEMORY_H
#define POI_FLASH_MEMORY_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <ws2812.h>
#include "ledpoi.h"

#define STORAGE_NAMESPACE "poiStorage"
#define IMAGE_NAMESPACE "poiImage"
#define PROGRAM_NAMESPACE "poiProgram"

#define IMAGE_KEY "image"
#define PROGRAM_KEY "program"
#define NUM_PROG_STEPS_KEY "numProgSteps"

/**
 * Interface to Non-volatile (flash) memory for image and program
 **/

class PoiFlashMemory
{
public:
  bool saveImage(rgbVal *imageData, uint8_t scene, uint8_t size_x, uint8_t size_y);
  bool saveProgram(uint8_t *programData, uint8_t size_x, uint8_t size_y);
  bool saveNumProgramSteps(uint8_t numProgSteps);

  bool loadImage(rgbVal *imageData, uint8_t scene);
  bool loadProgram(uint8_t *programData);
  bool loadNumProgramSteps(uint8_t *numProgSteps);

private:
  esp_err_t _save_uint8_array(const char* mynamespace, const char* key, uint8_t *data,
      uint8_t size_x, uint8_t size_y);
  esp_err_t _save_uint8(const char* mynamespace, const char* key, uint8_t value);
  esp_err_t _read_uint8_array(const char* mynamespace, const char* key, uint8_t *data);
  esp_err_t _read_uint8(const char* mynamespace, const char* key, uint8_t *value);
  void serializeRgbData(rgbVal *imageData, uint8_t *serializedImageData,
      uint8_t size_x, uint8_t size_y, uint8_t *newSize);
  void deserializeRgbData(uint8_t *serializedImageData, rgbVal *deserializedImageData,
      uint8_t size_x, uint8_t size_y);

};


#endif
