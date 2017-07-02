#include "PoiFlashMemory.h"

void PoiFlashMemory::setup(LogLevel logLevel, uint8_t *initImageData){
  // check whether flash is initialized
  // it is when it contains the correct number of scenes
  uint8_t numScenes = 0;
  if (!loadNumScenes(&numScenes) || numScenes != N_SCENES){
      // initialize image partition
      printf("Initializing the image partition of the flash memory\n" );
      if (!_initializeImageMemory(initImageData)){
        printf("Error. Initializing the image partition of the flash memory failed.\n" );
      }
  }
  uint8_t numProgSteps = 0;
  if (!loadNumProgramSteps(&numProgSteps)){
    printf("Initializing the number of program steps to 0\n" );
    saveNumProgramSteps(numProgSteps); // initialize with 0
  }

  if (logLevel != MUTE) printContents();
}

void PoiFlashMemory::printContents(){

    listPartitions();
    uint8_t numScenes = 0;
    loadNumScenes(&numScenes);
    uint8_t numProgSteps = 0;
    loadNumProgramSteps(&numProgSteps);

    printf("Flash memory has space for %d scenes and contains %d program steps.\n",
      numScenes, numProgSteps);
}

void PoiFlashMemory::listPartitions(){

  printf("Available data partitions on flash memory:\n");
  esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    const esp_partition_t* prev = NULL;
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *p = esp_partition_get(it);
        if (p == NULL) {
          printf("Partition not found\n" );
        }
        else{
              printf("  %s size: %d type %d\n", p->label, p->size, p->subtype);
        }
    }
    printf("Available app partitions on flash memory:\n");
    it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
      for (; it != NULL; it = esp_partition_next(it)) {
          const esp_partition_t *p = esp_partition_get(it);
          if (p == NULL) {
            printf("Partition not found\n" );
          }
          else{
                printf("  %s size: %d type %d\n", p->label, p->size, p->subtype);
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
  	printf("Error. Cannot save scene %d. Maximum number of scenes is %d .", scene, N_SCENES);
      return false;
  }
  const esp_partition_t *p = _getDataPartition();
  if (p == 0){
    printf("Error. Data partition does not exist.");
    return false;
  }
  printf("Using partition %s at 0x%x, size 0x%x\n",
    p->label, p->address, p->size);

  const uint32_t offset = scene * getSizeOfImageSection();

  printf("Erasing partition range %d at offset %d for image\n", getSizeOfImageSection(), offset);
  esp_err_t err = esp_partition_erase_range(p, offset, getSizeOfImageSection());
  if (err != ESP_OK){
    printf("Error. Cannot erase partition section for new image (Errorcode: %d).\n", err);
    return false;
  }

  printf("Writing image data\n");
  err = esp_partition_write(p, offset, imageData, getSizeOfImageSection());
  if (err != ESP_OK){
    printf("Error. Cannot write image data to partition (Errorcode: %d).\n", err);
    return false;
  }

  return true;
}

bool PoiFlashMemory::loadImage(uint8_t scene, uint8_t *imageData){
  if (scene > N_SCENES-1){
  	printf("Error. Cannot load scene %d. Maximum number of scenes is %d .", scene, N_SCENES);
      return false;
  }
  const esp_partition_t *p = _getDataPartition();
  if (p == 0){
    printf("Error. Data partition does not exist.");
    return false;
  }
  printf("Using partition %s at 0x%x, size 0x%x\n",
    p->label, p->address, p->size);

  const uint32_t offset = scene * getSizeOfImageSection();

  printf("Loading image for scene %d from offset %d.\n", scene, offset);
  esp_err_t err = esp_partition_read(p, offset, imageData, getSizeOfImageSection());
  if (err != ESP_OK){
    printf("Error. Cannot read data from partition (Errorcode: %d).\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveProgram(uint8_t *programData, uint8_t size_x, uint8_t size_y){
  esp_err_t err = _nvs_save_uint8_array(NVS_PROGRAM_NAMESPACE, NVS_PROGRAM_KEY,  programData, size_x, size_y);
  if (err != ESP_OK) {
    printf("Error (%4x) writing program data to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadProgram(uint8_t *programData){
  esp_err_t err = _nvs_read_uint8_array(NVS_PROGRAM_NAMESPACE, NVS_PROGRAM_KEY, programData);
  if (err != ESP_OK) {
    printf("Error (%4x) readimg program data from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveIpIncrement(uint8_t ipIncrement){
  esp_err_t err = _nvs_save_uint8(NVS_GENERAL_NAMESPACE, NVS_IP_INCREMENT_KEY, ipIncrement);
  if (err != ESP_OK) {
    printf("Error (%4x) writing ip increment to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadIpIncrement(uint8_t *ipIncrement){
  esp_err_t err = _nvs_read_uint8(NVS_GENERAL_NAMESPACE, NVS_IP_INCREMENT_KEY, ipIncrement);
  if (err != ESP_OK) {
    printf("Error (%4x) reading ip increment from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveNumProgramSteps(uint8_t numProgSteps){
  esp_err_t err = _nvs_save_uint8(NVS_PROGRAM_NAMESPACE, NVS_NUM_PROG_STEPS_KEY, numProgSteps);
  if (err != ESP_OK) {
    printf("Error (%4x) writing mumber of program steps to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadNumProgramSteps(uint8_t *numProgSteps){
  esp_err_t err = _nvs_read_uint8(NVS_PROGRAM_NAMESPACE, NVS_NUM_PROG_STEPS_KEY, numProgSteps);
  if (err != ESP_OK) {
    printf("Error (%4x) reading number of program steps from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveNumScenes(uint8_t numScenes){
  esp_err_t err = _nvs_save_uint8(NVS_IMAGE_NAMESPACE, NVS_NUM_SCENES_KEY, numScenes);
  if (err != ESP_OK) {
    printf("Error (%4x) writing number of scenes to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadNumScenes(uint8_t *numScenes){
  esp_err_t err = _nvs_read_uint8(NVS_IMAGE_NAMESPACE, NVS_NUM_SCENES_KEY, numScenes);
  if (err != ESP_OK) {
    printf("Error (%4x) reading number of scenes from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::_initializeImageMemory(uint8_t *initImageData){

  if (!_eraseImages()){
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

bool PoiFlashMemory::_eraseImages(){

	// first remove image data
  const esp_partition_t *p = _getDataPartition();
  if (p == 0){
    printf("Error. Data partition does not exist.");
    return false;
  }
  printf("Using partition %s at 0x%x, size 0x%x\n",
    p->label, p->address, p->size);

  printf("Erasing complete image partition. \n");
  esp_err_t err = esp_partition_erase_range(p, 0, p->size);
  if (err != ESP_OK){
    printf("Error. Cannot erase complete image partition (Errorcode: %d).\n", err);
    return false;
  }

  // TODO: initialize images with 0s (instead of 255s)

  // then remove nvs namespace with image data (number of images for now)
  err = _nvs_eraseCompleteNamespace(NVS_IMAGE_NAMESPACE);
  if (err != ESP_OK) {
    printf("Error (%4x) while erasing images.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::_initializeProgramMemory(){
  esp_err_t  err = _nvs_eraseCompleteNamespace(NVS_PROGRAM_NAMESPACE);
  if (err != ESP_OK) {
    printf("Error (%4x) while erasing program.\n", err);
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
  printf("Writing uint8_t value %d with key %s to flash nvs...\n", value, key );
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
    uint8_t *data, uint8_t size_x, uint8_t size_y){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Writing uint8_t array with key %s to falsh nvs...\n", key );
  err = nvs_open(mynamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  // Write prog values
  size_t required_size = sizeof(uint8_t)*size_x*size_y;
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

esp_err_t PoiFlashMemory::_nvs_read_uint8(const char* mynamespace, const char* key, uint8_t *value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Reading uint8_t with key %s from flash nvs...\n", key );
  err = nvs_open(mynamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) return err;

  err = nvs_get_u8(my_handle, key,  value);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
  printf("Read value: %d\n", *value);

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t PoiFlashMemory::_nvs_read_uint8_array(const char* mynamespace, const char* key,
    uint8_t *data){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Reading uint8_t array with key %s from flash nvs...\n", key );
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


esp_err_t PoiFlashMemory::_nvs_eraseCompleteNamespace(const char* mynamespace){

  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Erasing namespace %s on flash nvs...\n", mynamespace);
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
  printf("Erasing complete NVS flash partition.\n");
  const esp_partition_t* nvs_partition = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
  assert(nvs_partition && "partition table must have an NVS partition");
  ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
  // Retry nvs_flash_init
  esp_err_t err = nvs_flash_init();
  ESP_ERROR_CHECK( err );
}
