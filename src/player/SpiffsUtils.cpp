#include "SpiffsUtils.h"

void SpiffsUtils::setup(){

    if(!SPIFFS.begin()){
        LOGW(SPIF_U, "SPIFFS partition unusable, will create new one");
        if(!SPIFFS.begin(true)){
            LOGE(SPIF_U, "SPIFFS partition mount failed");
        return;
        }
    }
    LOGI(SPIF_U, "SPIFFS partition setup done.");
    listPartitions();
}

void SpiffsUtils::openFile(const char * path){
    LOGI(SPIF_U, "Opening file: %s", path);

    _file = SPIFFS.open(path);
    if(!_file){
        LOGE(SPIF_U,"Failed to open file for reading");
        return;
    }
    _curFrame = 0;
}

void SpiffsUtils::closeFile(){
    LOGI(SPIF_U, "Closing file");

    _file.close();
}

bool SpiffsUtils::getNextFrame(PixelFrame* pframe) {

    if (!_file) {
        LOGE(SPIF_U, "Cannot read from closed file.")
    }

    _curFrame++;
    uint8_t rgbArray[3];
    for (int i = 0; i< N_PIXELS/2; i++) {
        for (int j = 0; j<3; j++) {
            if (_file.available()) {
                rgbArray[j] = _file.read();
            }
            else {
                LOGW(SPIF_U, "frame %d pixel %d is not complete", _curFrame, i)
                return false;
            }
        } 
        LOGV(SPIF_U, "pixel %d: %u %u %u", i, rgbArray[0], rgbArray[1], rgbArray[2]);
        pframe->pixel[i] =  makeRGBValue(rgbArray);
        pframe-> idx = _curFrame;
    }

    return true;
}

 bool SpiffsUtils::hasNextFrame() {
     return _file.available();
 }

// other general util functions

void SpiffsUtils::listDir(const char * dirname, uint8_t levels){
    LOGI(SPIF_U, "Listing directory: %s", dirname);

    File root = SPIFFS.open(dirname);
    if(!root){
        LOGE(SPIF_U, "Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        LOGE(SPIF_U, "Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            LOGI(SPIF_U, "  DIR : ");
            LOGI(SPIF_U, "%s", file.name());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            LOGI(SPIF_U,"  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
            if(levels){
                listDir(file.name(), levels -1);
            }
        } else {
            LOGI(SPIF_U,"  FILE: ");
            LOGI(SPIF_U, "%s", file.name());
            LOGI(SPIF_U,"  SIZE: ");
            LOGI(SPIF_U, "%d", file.size());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            LOGI(SPIF_U,"  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

void SpiffsUtils::createDir(const char * path){
    LOGI(SPIF_U, "Creating Dir: %s", path);
    if(SPIFFS.mkdir(path)){
        LOGI(SPIF_U, "Dir created");
    } else {
        LOGE(SPIF_U, "mkdir failed");
    }
}

void SpiffsUtils::removeDir(const char * path){
    LOGI(SPIF_U, "Removing Dir: %s", path);
    if(SPIFFS.rmdir(path)){
        LOGI(SPIF_U, "Dir removed");
    } else {
        LOGE(SPIF_U, "rmdir failed");
    }
}

void SpiffsUtils::readFile(const char * path){
    LOGI(SPIF_U, "Reading file: %s", path);

    File file = SPIFFS.open(path);
    if(!file){
        LOGE(SPIF_U, "Failed to open file for reading");
        return;
    }

    LOGI(SPIF_U, "Read from file: ");
    while(file.available()){
        LOGI(SPIF_U, "%d", file.read());
    }
    file.close();
}

void SpiffsUtils::writeFile(const char * path, const char * message){
    LOGI(SPIF_U, "Writing file: %s", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if(!file){
        LOGE(SPIF_U, "Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        LOGI(SPIF_U, "File written");
    } else {
        LOGE(SPIF_U, "Write failed");
    }
    file.close();
}

void SpiffsUtils::appendFile(const char * path, const char * message){
    LOGI(SPIF_U, "Appending to file: %s", path);

    File file = SPIFFS.open(path, FILE_APPEND);
    if(!file){
        LOGI(SPIF_U, "Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        LOGI(SPIF_U, "Message appended");
    } else {
        LOGE(SPIF_U, "Append failed");
    }
    file.close();
}

void SpiffsUtils::renameFile(const char * path1, const char * path2){
    LOGI(SPIF_U, "Renaming file %s to %sLOGI(SPIF_U, ", path1, path2);
    if (SPIFFS.rename(path1, path2)) {
        LOGI(SPIF_U, "File renamed");
    } else {
        LOGE(SPIF_U, "Rename failed");
    }
}

void SpiffsUtils::deleteFile(const char * path){
    LOGI(SPIF_U, "Deleting file: %s", path);
    if(SPIFFS.remove(path)){
        LOGI(SPIF_U, "File deleted");
    } else {
        LOGE(SPIF_U, "Delete failed");
    }
}

void SpiffsUtils::listPartitions() {
    LOGI(SPIF_U, "Available data partitions on flash memory:");
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *p = esp_partition_get(it);
        if (p == NULL) {
          LOGE(SPIF_U, "Partition not found" );
        }
        else {
            LOGI(SPIF_U, "  %s size: %d type %d", p->label, p->size, p->subtype);
        }
    }
    LOGI(SPIF_U, "Available app partitions on flash memory:");
    it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
      for (; it != NULL; it = esp_partition_next(it)) {
          const esp_partition_t *p = esp_partition_get(it);
          if (p == NULL) {
            LOGE(SPIF_U, "Partition not found" );
          }
          else{
                LOGI(SPIF_U, "  %s size: %d type %d", p->label, p->size, p->subtype);
          }
      }
    esp_partition_iterator_release(it);
}

void SpiffsUtils::example(){
    listDir("/", 0);
    //removeDir("/mydir");
    //createDir("/mydir");
    deleteFile("/hello.txt");
    writeFile("/hello.txt", "Hello ");
    appendFile("/hello.txt", "World!");
    writeFile("/mydir3/hello3.txt", "Hello ");

	listDir("/", 2);
    listDir("/mydir3", 0);
}

