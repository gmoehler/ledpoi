#ifndef POI_FLASH_MEMORY_H
#define POI_FLASH_MEMORY_H

#include "ledpoi.h"
#include "ledpoi_utils.h"

#include <esp_system.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <nvs.h>


#define NVS_GENERAL_NAMESPACE "poiGeneral"
#define NVS_IP_INCREMENT_KEY "ipIncrement"

#define NVS_IMAGE_NAMESPACE "poiImage"
#define NVS_NUM_SCENES_KEY "numScenes"

#define NVS_PROGRAM_KEY "program"
#define NVS_NUM_PROG_STEPS_KEY "numProgSteps"
#define NVS_PROGRAM_NAMESPACE "poiProgram"

/**
 * Interface to flash partition memory for image and Non-volatile (NVS) memory for the program
 * Image data on flash is organized into fixed image memory segements of size
 * N_NUM_IMAGE_SECTIONS *
 *
 * in case of startup issues with flash do this
 *  PoiFlashMemory flashMem;
 *  flashMem.initializeFlash();
 * or call runner.initializeFlash()
 * A flash hard reset works like this:
 * .~/.platformio/packages/framework-arduinoespressif32/tools/esptool.py --chip esp32 --port COM6 --baud 115200
 *  --before default_reset --after hard_reset erase_flash
 **/



class PoiFlashMemory
{
public:

  void setup(uint8_t *initImageData); // to be called during setup
  void initializeFlash(uint8_t *initImageData);
  void printContents();
  uint32_t getSizeOfImageSection();

  bool saveImage(uint8_t scene, uint8_t* imageData);
  bool loadImage(uint8_t scene, uint8_t* imageData);

  bool saveProgram(uint8_t* programData);
  bool loadProgram(uint8_t* programData);

  bool saveNumProgramSteps(uint16_t numProgSteps);
  bool loadNumProgramSteps(uint16_t* numProgSteps);

  bool saveIpIncrement(uint8_t ipIncrement);
  bool loadIpIncrement(uint8_t* ipIncrement);

  bool saveNumScenes(uint8_t numScenes);
  bool loadNumScenes(uint8_t* numScenes);

private:
  void _listPartitions();

  const esp_partition_t* _getDataPartition();
  bool _eraseImagePartition();
  bool _initializeProgramMemory();  // only program on nvs, reset prog steps to 0
  bool _initializeImageMemory(uint8_t *initImageData);
  bool _eraseNvsFlashPartition(); // all NVS stuff

  esp_err_t _nvs_save_uint8_array(const char* mynamespace, const char* key, uint8_t *data,
      uint16_t size_x, uint8_t size_y);
  esp_err_t _nvs_save_uint8(const char* mynamespace, const char* key, uint8_t value);
  esp_err_t _nvs_save_uint16(const char* mynamespace, const char* key, uint16_t value);
  esp_err_t _nvs_read_uint8_array(const char* mynamespace, const char* key, uint8_t *data);
  esp_err_t _nvs_read_uint8(const char* mynamespace, const char* key, uint8_t *value);
  esp_err_t _nvs_read_uint16(const char* mynamespace, const char* key, uint16_t *value);
  esp_err_t _nvs_eraseCompleteNamespace(const char* mynamespace);
};


#endif
