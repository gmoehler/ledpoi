#include "PoiFlashMemory.h"

void PoiFlashMemory::setup(uint8_t *initImageData){
  // check whether flash is initialized
  // it is when it contains the correct number of scenes
  uint8_t numScenes = 0;
  if (!loadNumScenes(&numScenes) || numScenes != N_SCENES){
      // initialize image partition
      LOGI(FLASH, "Initializing the image partition of the flash memory" );
      if (!_initializeImageMemory(initImageData)){
        LOGI(FLASH, "Error. Initializing the image partition of the flash memory failed." );
      }
  }

  uint16_t numProgSteps = 0;
  if (!loadNumProgramSteps(&numProgSteps)){
    LOGI(FLASH, "Initializing the number of program steps to 0" );
    saveNumProgramSteps(numProgSteps); // initialize with 0
  }

  uint8_t ipIncrement = 0;
  if (!loadIpIncrement(&ipIncrement)){
    LOGI(FLASH, "Initializing the ip increment to 0" );
    saveIpIncrement(ipIncrement); // initialize with 0
  }

  printContents();
}

void PoiFlashMemory::initializeFlash(uint8_t *initImageData) {

  _eraseImagePartition();

  // a more thorough NVS partition cleaning is done by _eraseNvsFlashPartition() 
  // but it currently results in a cpu halt and, therefore, requires a hard reset:
  // https://github.com/espressif/arduino-esp32/issues/365
  // therefore erase all namespaces here:
  _nvs_eraseCompleteNamespace(NVS_GENERAL_NAMESPACE);
  _nvs_eraseCompleteNamespace(NVS_PROGRAM_NAMESPACE);
  _nvs_eraseCompleteNamespace(NVS_IMAGE_NAMESPACE);
  setup(initImageData);
}

void PoiFlashMemory::printContents(){

    _listPartitions();
    uint8_t numScenes = 0;
    loadNumScenes(&numScenes);
    uint16_t numProgSteps = 0;
    loadNumProgramSteps(&numProgSteps);

    LOGI(FLASH, "Flash memory has space for %d scenes and contains %d program steps.",
      numScenes, numProgSteps);
}

void PoiFlashMemory::_listPartitions(){

  LOGI(FLASH, "Available data partitions on flash memory:");
  esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    const esp_partition_t* prev = NULL;
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *p = esp_partition_get(it);
        if (p == NULL) {
          LOGE(FLASH, "Partition not found" );
        }
        else{
              LOGI(FLASH, "  %s size: %d type %d", p->label, p->size, p->subtype);
        }
    }
    LOGI(FLASH, "Available app partitions on flash memory:");
    it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
      for (; it != NULL; it = esp_partition_next(it)) {
          const esp_partition_t *p = esp_partition_get(it);
          if (p == NULL) {
            LOGE(FLASH, "Partition not found" );
          }
          else{
                LOGI(FLASH, "  %s size: %d type %d", p->label, p->size, p->subtype);
          }
      }
    esp_partition_iterator_release(it);
}

uint32_t PoiFlashMemory::getSizeOfImageSection(){
  // fixed size for image section
  return N_NUM_FLASH_SECTIONS_PER_IMAGE * SPI_FLASH_SEC_SIZE;
}

bool PoiFlashMemory::saveImage(uint8_t scene, uint8_t *imageData){
  if (scene > N_SCENES-1){
  	LOGE(FLASH, "Error. Cannot save scene %d. Maximum number of scenes is %d .", scene, N_SCENES);
      return false;
  }
  const esp_partition_t *p = _getDataPartition();
  if (p == 0){
    LOGE(FLASH, "Error. Data partition does not exist.");
    return false;
  }

  const uint32_t offset = scene * getSizeOfImageSection();

  LOGI(FLASH, "Saving scene %d to partition '%s' at 0x%x, offset %d, size %d",
    scene, p->label, p->address, offset, p->size);

  LOGD(FLASH, "Erasing partition range %d at offset %d for image", getSizeOfImageSection(), offset);
  esp_err_t err = esp_partition_erase_range(p, offset, getSizeOfImageSection());
  if (err != ESP_OK){
    LOGE(FLASH, "Error. Cannot erase partition section for new image (Errorcode: 0x%d).", err);
    //return false;
  }

  LOGD(FLASH, "Writing image data");
  err = esp_partition_write(p, offset, imageData, getSizeOfImageSection());
  if (err != ESP_OK){
    LOGE(FLASH, "Error. Cannot write image data to partition (Errorcode: %d).", err);
    return false;
  }

  return true;
}

bool PoiFlashMemory::loadImage(uint8_t scene, uint8_t *imageData){
  if (scene > N_SCENES-1){
  	LOGE(FLASH, "Error. Cannot load scene %d. Maximum number of scenes is %d .", scene, N_SCENES);
      return false;
  }
  const esp_partition_t *p = _getDataPartition();
  if (p == 0){
    LOGE(FLASH, "Error. Data partition does not exist.");
    return false;
  }
  const uint32_t offset = scene * getSizeOfImageSection();
  
  LOGD(FLASH, "Using partition %s at 0x%x, size 0x%x",
    p->label, p->address, p->size);

  LOGI(FLASH, "Loading image for scene %d from offset %d.", scene, offset);
  esp_err_t err = esp_partition_read(p, offset, imageData, getSizeOfImageSection());
  if (err != ESP_OK){
    LOGE(FLASH, "Error. Cannot read data from partition (Errorcode: %d).", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveProgram(uint8_t *programData){
  esp_err_t err = _nvs_save_uint8_array(NVS_PROGRAM_NAMESPACE, NVS_PROGRAM_KEY,  programData, N_PROG_STEPS, N_CMD_FIELDS);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) writing program data to flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadProgram(uint8_t *programData){
  esp_err_t err = _nvs_read_uint8_array(NVS_PROGRAM_NAMESPACE, NVS_PROGRAM_KEY, programData);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) reading program data from flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveIpIncrement(uint8_t ipIncrement){
  esp_err_t err = _nvs_save_uint8(NVS_GENERAL_NAMESPACE, NVS_IP_INCREMENT_KEY, ipIncrement);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) writing ip increment to flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadIpIncrement(uint8_t *ipIncrement){
  esp_err_t err = _nvs_read_uint8(NVS_GENERAL_NAMESPACE, NVS_IP_INCREMENT_KEY, ipIncrement);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) reading ip increment from flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveNumProgramSteps(uint16_t numProgSteps){
  esp_err_t err = _nvs_save_uint16(NVS_PROGRAM_NAMESPACE, NVS_NUM_PROG_STEPS_KEY, numProgSteps);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) writing mumber of program steps to flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadNumProgramSteps(uint16_t *numProgSteps){
  esp_err_t err = _nvs_read_uint16(NVS_PROGRAM_NAMESPACE, NVS_NUM_PROG_STEPS_KEY, numProgSteps);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) reading number of program steps from flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveNumScenes(uint8_t numScenes){
  esp_err_t err = _nvs_save_uint8(NVS_IMAGE_NAMESPACE, NVS_NUM_SCENES_KEY, numScenes);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) writing number of scenes to flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadNumScenes(uint8_t *numScenes){
  esp_err_t err = _nvs_read_uint8(NVS_IMAGE_NAMESPACE, NVS_NUM_SCENES_KEY, numScenes);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) reading number of scenes from flash.", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::_initializeImageMemory(uint8_t *initImageData){

  if (!_eraseImagePartition()){
    return false;
  }
  
  // then remove nvs namespace with image data (number of images for now)
  esp_err_t err = _nvs_eraseCompleteNamespace(NVS_IMAGE_NAMESPACE);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) while erasing images.", err);
    return false;
  }

  // initialize with initImageData (black)
  for (int i=0; i< N_SCENES; i++){
    if (!saveImage(i, initImageData)){
      return false;
    }
  }
  if (!saveNumScenes(N_SCENES)){
    return false;
  }
  return true;
}

bool PoiFlashMemory::_eraseImagePartition(){

	// first remove image data
  const esp_partition_t *p = _getDataPartition();
  if (p == 0){
    LOGE(FLASH, "Error. Data partition does not exist.");
    return false;
  }
  LOGD(FLASH, "Using partition %s at 0x%x, size 0x%x",
    p->label, p->address, p->size);

  LOGD(FLASH, "Erasing complete image partition. ");
  esp_err_t err = esp_partition_erase_range(p, 0, p->size);
  if (err != ESP_OK){
    LOGE(FLASH, "Error. Cannot erase complete image partition (Errorcode: %d).", err);
    return false;
  }

  return true;
}

bool PoiFlashMemory::_initializeProgramMemory(){
  esp_err_t  err = _nvs_eraseCompleteNamespace(NVS_PROGRAM_NAMESPACE);
  if (err != ESP_OK) {
    LOGE(FLASH, "Error (%4x) while erasing program.", err);
    return false;
  }
  saveNumProgramSteps(0);
  return true;
}

const esp_partition_t* PoiFlashMemory::_getDataPartition(){
    return esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
          ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
}

esp_err_t PoiFlashMemory::_nvs_save_uint8(const char* mynamespace, const char* key, uint8_t value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Writing uint8_t value %d with key %s to flash nvs...", value, key );
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

esp_err_t PoiFlashMemory::_nvs_save_uint8_array(const char* mynamespace, const char* key,
    uint8_t *data, uint16_t numLines, uint8_t numFields){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Writing uint8_t array with key %s to flash nvs...", key );
  err = nvs_open(mynamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  // Write prog values
  size_t required_size = sizeof(uint8_t)*numLines*numFields;
  LOGD(FLASH, "Size: %d", required_size);
  err = nvs_set_blob(my_handle, key, data, required_size);
  if (err != ESP_OK) return err;

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) return err;

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_nvs_save_uint16(const char* mynamespace, const char* key, uint16_t value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Writing uint16_t value %d with key %s to flash nvs...", value, key );
  err = nvs_open(mynamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_set_u16(my_handle, key, value);
  if (err != ESP_OK) return err;

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) return err;

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_nvs_read_uint8(const char* mynamespace, const char* key, uint8_t *value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Reading uint8_t with key %s from flash nvs...", key );
  err = nvs_open(mynamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_get_u8(my_handle, key,  value);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
  LOGD(FLASH, "Read value: %d", *value);

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_nvs_read_uint8_array(const char* mynamespace, const char* key,
    uint8_t *data){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Reading uint8_t array with key %s from flash nvs...", key );
  err = nvs_open(mynamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) return err;

  // Read the size of memory space required for blob
  size_t data_size = 0;  // value will default to 0, if not set yet in NVS
  err = nvs_get_blob(my_handle, key, NULL, &data_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
  LOGD(FLASH, "Read size: %d", data_size);

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

esp_err_t PoiFlashMemory::_nvs_read_uint16(const char* mynamespace, const char* key, uint16_t *value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Reading uint16_t with key %s from flash nvs...", key );
  err = nvs_open(mynamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_get_u16(my_handle, key,  value);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
  LOGI(FLASH, "Read value: %d", *value);

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}


esp_err_t PoiFlashMemory::_nvs_eraseCompleteNamespace(const char* mynamespace){

  nvs_handle my_handle;
  esp_err_t err;

  // Open
  LOGD(FLASH, "Erasing namespace %s on flash nvs...", mynamespace);
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

bool PoiFlashMemory::_eraseNvsFlashPartition(){
  // NVS partition was truncated and needs to be erased
  LOGD(FLASH, "Erasing complete NVS flash partition.");
  const esp_partition_t* nvs_partition = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
  assert(nvs_partition && "partition table must have an NVS partition");
  ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
  // Retry nvs_flash_init
  esp_err_t err = nvs_flash_init();
  ESP_ERROR_CHECK( err );
}
