#include "PoiFlashMemory.h"

bool PoiFlashMemory::saveImage(uint8_t *imageData, uint8_t scene, uint8_t size_x, uint8_t size_y){
  esp_err_t err = _save_uint8_array(IMAGE_NAMESPACE, IMAGE_KEY, imageData, size_x, size_y, 3);
  if (err != ESP_OK) {
    printf("Error (%4x) writing image data to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveProgram(uint8_t *programData, uint8_t size_x, uint8_t size_y){
  esp_err_t err = _save_uint8_array(PROGRAM_NAMESPACE, PROGRAM_KEY,  programData, size_x, size_y);
  if (err != ESP_OK) {
    printf("Error (%4x) writing program data to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveNumProgramSteps(uint8_t numProgSteps){
  esp_err_t err = _save_uint8(PROGRAM_NAMESPACE, NUM_PROG_STEPS_KEY, numProgSteps);
  if (err != ESP_OK) {
    printf("Error (%4x) writing program data to flash.\n", err);
    return false;
  }
  return true;
}


bool PoiFlashMemory::loadImage(uint8_t *imageData, uint8_t scene){
  esp_err_t err = _read_uint8_array(IMAGE_NAMESPACE, IMAGE_KEY, imageData);
  if (err != ESP_OK) {
    printf("Error (%4x) reading image data from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadProgram(uint8_t *programData){
  esp_err_t err = _read_uint8_array(PROGRAM_NAMESPACE, PROGRAM_KEY, programData);
  if (err != ESP_OK) {
    printf("Error (%4x) readimg program data from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadNumProgramSteps(uint8_t *numProgSteps){
  esp_err_t err = _read_uint8(PROGRAM_NAMESPACE, NUM_PROG_STEPS_KEY, numProgSteps);
  if (err != ESP_OK) {
    printf("Error (%4x) readimg program data from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::eraseImages(){
  esp_err_t err = _eraseCompleteNamespace(IMAGE_NAMESPACE);
  if (err != ESP_OK) {
    printf("Error (%4x) while erasing images.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::eraseProgram(){
  esp_err_t err = _eraseCompleteNamespace("storage");
  if (err != ESP_OK) {
    printf("Error (%4x) while erasing storage.\n", err);
    return false;
  }
  err = _eraseCompleteNamespace(PROGRAM_NAMESPACE);
  if (err != ESP_OK) {
    printf("Error (%4x) while erasing program.\n", err);
    return false;
  }
  return true;
}


esp_err_t PoiFlashMemory::_save_uint8(const char* mynamespace, const char* key, uint8_t value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  //printf("Writing uint8_t...\n" );
  err = nvs_open(mynamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_set_u8(my_handle, key, value);
  if (err != ESP_OK) return err;

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) return err;

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_save_uint8_array(const char* mynamespace, const char* key,
    uint8_t *data, uint8_t size_x, uint8_t size_y, uint8_t size_z){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  //printf("Writing...\n" );
  err = nvs_open(mynamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  // Write prog values
  size_t required_size = sizeof(uint8_t)*size_x*size_y*size_z;
  printf("Size: %d\n", required_size);
  err = nvs_set_blob(my_handle, key, data, required_size);
  if (err != ESP_OK) return err;

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) return err;

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_read_uint8(const char* mynamespace, const char* key, uint8_t *value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Reading uint8_t...\n" );
  err = nvs_open(mynamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_get_u8(my_handle, key,  value);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_read_uint8_array(const char* mynamespace, const char* key,
    uint8_t *data){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Reading...\n" );
  err = nvs_open(mynamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) return err;

  // Read the size of memory space required for blob
  size_t data_size = 0;  // value will default to 0, if not set yet in NVS
  err = nvs_get_blob(my_handle, key, NULL, &data_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
  printf("Read size: %d\n", data_size);

  if (data_size > 0) {
      err = nvs_get_blob(my_handle, key, data, &data_size);
      if (err != ESP_OK) return err;
  }
  else {
    return ESP_FAIL; // no data
  }

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}


esp_err_t PoiFlashMemory::_eraseCompleteNamespace(const char* mynamespace){

  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Erasing namespace %s...\n", mynamespace);
  err = nvs_open(mynamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_erase_all(my_handle);
  if (err != ESP_OK) return err;

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) return err;

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

bool PoiFlashMemory::eraseNvsFlashPartition(){
  // NVS partition was truncated and needs to be erased
  printf("Erasing NVS flash partition.\n");
  const esp_partition_t* nvs_partition = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
  assert(nvs_partition && "partition table must have an NVS partition");
  ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
  // Retry nvs_flash_init
  esp_err_t err = nvs_flash_init();
  ESP_ERROR_CHECK( err );
}
