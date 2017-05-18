#include "PoiFlashMemory.h"

esp_err_t PoiFlashMemory::_save_uint8(const char* mynamespace, const char* key, uint8_t value){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Writing uint8_t...\n" );
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


bool PoiFlashMemory::saveImage(uint8_t *imageData, uint8_t size_x, uint8_t size_y){
  esp_err_t err = _save_uint8_array(IMAGE_NAMESPACE, "image", imageData, size_x, size_y);
  if (err != ESP_OK) {
    printf("Error (%d) writing image data to flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::saveProgram(uint8_t *programData, uint8_t size_x, uint8_t size_y){
  esp_err_t err = _save_uint8_array(PROGRAM_NAMESPACE, "program",  programData, size_x, size_y);
  if (err != ESP_OK) {
    printf("Error (%d) writing program data to flash.\n", err);
    return false;
  }
  return true;
}

esp_err_t PoiFlashMemory::_save_uint8_array(const char* mynamespace, const char* key,
    uint8_t *data, uint8_t size_x, uint8_t size_y){
  nvs_handle my_handle;
  esp_err_t err;

  // Open
  printf("Writing...\n" );
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


bool PoiFlashMemory::loadImage(uint8_t *imageData){
  esp_err_t err = _read_uint8_array(IMAGE_NAMESPACE, "image", imageData);
  if (err != ESP_OK) {
    printf("Error (%d) reading image data from flash.\n", err);
    return false;
  }
  return true;
}

bool PoiFlashMemory::loadProgram(uint8_t *programData){
  esp_err_t err = _read_uint8_array(PROGRAM_NAMESPACE, "program", programData);
  if (err != ESP_OK) {
    printf("Error (%d) readimg program data from flash.\n", err);
    return false;
  }
  return true;
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

esp_err_t PoiFlashMemory::save_restart_counter(void)
{
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_conter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // Write
    restart_counter++;
    err = nvs_set_i32(my_handle, "restart_conter", restart_counter);
    if (err != ESP_OK) return err;

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}


esp_err_t PoiFlashMemory::save_run_time(void)
{
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read the size of memory space required for blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // Read previously saved blob if available
    uint32_t* run_time = (uint32_t*) malloc(required_size + sizeof(uint32_t));
    if (required_size > 0) {
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) return err;
    }

    // Write value including previously saved blob if available
    required_size += sizeof(uint32_t);
    run_time[required_size / sizeof(uint32_t) - 1] = xTaskGetTickCount() * portTICK_PERIOD_MS;
    err = nvs_set_blob(my_handle, "run_time", run_time, required_size);
    free(run_time);

    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

/* Read from NVS and print restart counter
   and the table with run times.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t PoiFlashMemory::print_what_saved(void)
{
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read restart counter
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_conter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    printf("Restart counter = %d\n", restart_counter);

    // Read run time blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    // obtain required memory space to store blob being read from NVS
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    printf("Run time:\n");
    if (required_size == 0) {
        printf("Nothing saved yet!\n");
    } else {
        uint32_t* run_time = (uint32_t*) malloc(required_size);
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) return err;
        for (int i = 0; i < required_size / sizeof(uint32_t); i++) {
            printf("%d: %d\n", i + 1, run_time[i]);
        }
        free(run_time);
    }

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

void PoiFlashMemory::setup()
{

  esp_err_t err = nvs_flash_init();
      if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
          // NVS partition was truncated and needs to be erased
          const esp_partition_t* nvs_partition = esp_partition_find_first(
                  ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
          assert(nvs_partition && "partition table must have an NVS partition");
          ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
          // Retry nvs_flash_init
          err = nvs_flash_init();
      }
      ESP_ERROR_CHECK( err );

      err = print_what_saved();
      if (err != ESP_OK) printf("Error (%d) reading data from NVS!\n", err);

      err = save_restart_counter();
      if (err != ESP_OK) printf("Error (%d) saving restart counter to NVS!\n", err);

      err = save_run_time();
      if (err != ESP_OK) printf("Error (%d) saving run time blob to NVS!\n", err);
}
