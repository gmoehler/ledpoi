#ifndef POI_FLASH_MEMORY_H
#define POI_FLASH_MEMORY_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <ws2812.h>
#include "ledpoi.h"

#define NVS_GENERAL_NAMESPACE "poiGeneral"
#define NVS_IMAGE_NAMESPACE "poiImage"
#define NVS_PROGRAM_NAMESPACE "poiProgram"

#define NVS_IP_INCREMENT_KEY "ipIncrement"
#define NVS_PROGRAM_KEY "program"
#define NVS_NUM_PROG_STEPS_KEY "numProgSteps"
#define NVS_NUM_SCENES_KEY "numScenes"

/**
 * Interface to flash partition memory for image and Non-volatile (NVS) memory for the program
 * Image data on flash is organized into fixed image memory segements of size
 * N_NUM_IMAGE_SECTIONS *
 *
 * in case of startup issues with flash do this
 *  PoiFlashMemory flashMem;
 *  flashMem.eraseNvsFlashPartition();
 *  flashMem.eraseImages();
 * or call runner.resetFlash()
 * A flash hard reset works like this:
 * .~/.platformio/packages/framework-arduinoespressif32/tools/esptool.py --chip esp32 --port COM6 --baud 115200
 *  --before default_reset --after hard_reset erase_flash
 **/



class PoiFlashMemory
{
public:

  void setup(LogLevel logLevel, uint8_t *initImageData); // to be called during setup

  bool saveImage(uint8_t scene, uint8_t* imageData);
  bool loadImage(uint8_t scene, uint8_t* imageData);

  bool saveProgram(uint8_t* programData, uint8_t size_x, uint8_t size_y);
  bool loadProgram(uint8_t* programData);

  bool saveNumProgramSteps(uint8_t numProgSteps);
  bool loadNumProgramSteps(uint8_t* numProgSteps);

  bool saveIpIncrement(uint8_t ipIncrement);
  bool loadIpIncrement(uint8_t* ipIncrement);

  bool saveNumScenes(uint8_t numScenes);
  bool loadNumScenes(uint8_t* numScenes);

  bool eraseNvsFlashPartition(); // all NVS stuff

 
  void listPartitions();
  void printContents();

  uint32_t getSizeOfImageSection();

private:
  const esp_partition_t* _getDataPartition();
  bool _eraseImages();
  bool _initializeProgramMemory();  // only program on nvs, reset prog steps to 0
  bool _initializeImageMemory(uint8_t *initImageData);

  esp_err_t _nvs_save_uint8_array(const char* mynamespace, const char* key, uint8_t *data,
      uint8_t size_x, uint8_t size_y);
  esp_err_t _nvs_save_uint8(const char* mynamespace, const char* key, uint8_t value);
  esp_err_t _nvs_read_uint8_array(const char* mynamespace, const char* key, uint8_t *data);
  esp_err_t _nvs_read_uint8(const char* mynamespace, const char* key, uint8_t *value);
  esp_err_t _nvs_eraseCompleteNamespace(const char* mynamespace);
};


#endif
