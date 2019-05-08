#include "SpiffsUtils.h"

#ifdef WITHIN_UNITTEST
  SpiffsClass SPIFFS;
#endif

void SpiffsUtils::setup(){

    if(!SPIFFS.begin()){
        LOGW(SPIF_U, "SPIFFS partition unusable, will create new one");
        if(!SPIFFS.begin(true)){
            LOGE(SPIF_U, "SPIFFS partition mount failed");
        return;
        }
    }
    LOGI(SPIF_U, "SPIFFS partition setup done.");
#ifndef WITHIN_UNITTEST
    listPartitions();
#endif
}

void SpiffsUtils::openFile(const char * path){
    LOGI(SPIF_U, "Opening file: %s", path);

    _file = SPIFFS.open(path);
#ifndef WITHIN_UNITTEST
    if(!_file){
        LOGE(SPIF_U,"Failed to open file for reading");
        return;
    }
#endif
    _curFrameIdx = -1;
    _currPixel = makeRGBVal(0,0,0);
    _currPixelCnt = 0;

    if (!readImageHeader()){
        LOGE(SPIF_U, "Error reading runtime encoding header")
    } 
}

void SpiffsUtils::closeFile(){
    LOGI(SPIF_U, "Closing file");

    _file.close();
}

bool SpiffsUtils::getNextFrame(PixelFrame* pframe) {
#ifndef WITHIN_UNITTEST
    if (!_file) {
        LOGE(SPIF_U, "Cannot read from closed file.");
        fillFrame(pframe,_curFrameIdx, 0, 0, 0, 0);
        return false;
    }
#endif

    if (_header.codec == NO_CODEC) {
        LOGD(SPIF_U, "Loading unencoded image...");
        return getNextFrameNoEncoding(pframe);
    }
    else if (_header.codec == RUNTIME_CODEC) {
        LOGD(SPIF_U, "Loading runtime encoded image...");
        return getNextFrameRuntimeEncoding(pframe);
    }
    LOGE(SPIF_U, "Image not encoded with known codec.");
    clearImageHeader();
    return false;
}

bool SpiffsUtils::getNextFrameNoEncoding(PixelFrame* pframe) {
    _curFrameIdx++;
    uint8_t rgbArray[3];
    for (int i = 0; i< N_PIXELS/2; i++) {
        for (int j = 0; j<3; j++) {
            if (_file.available()) {
                rgbArray[j] = _file.read();
            }
            else {
                LOGW(SPIF_U, "frame %d pixel %d is not complete", _curFrameIdx, i)
                // set remaining frame to black
                fillFrame(pframe,_curFrameIdx, 0, 0, 0, 0, i);
                return false;
            }
        } 
        LOGV(SPIF_U, "pixel %d: %u %u %u", i, rgbArray[0], rgbArray[1], rgbArray[2]);
        pframe->pixel[i] = makeRGBValue(rgbArray);
        pframe->pixel[N_PIXELS-i-1] = pframe->pixel[i];
        pframe->idx = _curFrameIdx;
    }

    return true;
}

void SpiffsUtils::clearImageHeader() {
	_header.codec = 0;
	_header.height = 0;
	_header.width = 0;
}

ImageHeader SpiffsUtils::getHeader(){
    return _header;
}
	
// header is CODEC-HEIGHT-WIDTH
bool SpiffsUtils::readImageHeader() {
    if (_file.available()) {
        uint8_t codec = _file.read();
        if (codec != RUNTIME_CODEC && codec != NO_CODEC) {
            LOGE(SPIF_U, "Image not encoded with known codec.");
            clearImageHeader();
            return false;
        }
        _header.codec = codec;
    }
    if (_file.available()) {
        _header.height = _file.read();
    }
    if (_file.available()) {
    	uint8_t widthUpper = _file.read();
        uint8_t widthLower= _file.read();
        _header.width = widthLower + 256 * widthUpper;
    }
    if (!_file.available()) {
        LOGE(SPIF_U, "Cannot read header from image.");
        clearImageHeader();
		return false;
    }
    LOGI(SPIF_U, "Header: encoding: %d, height: %d, width: %d", _header.codec, _header.height, _header.width)
    return true;
}

bool SpiffsUtils::getNextFrameRuntimeEncoding(PixelFrame* pframe) {

    uint8_t nPixels = _header.height >  N_PIXELS/2 ?  N_PIXELS/2 : _header.height;
    for (int i = 0; i < nPixels; i++) {
        if (_currPixelCnt > 0) {
            // re-use current pixel
            pframe->pixel[i] = _currPixel;
            pframe->pixel[N_PIXELS-i-1] = _currPixel;
            _currPixelCnt--;
        }
        else {
            // read next pixel
            uint8_t rgbArray[4];
            for (int j = 0; j<4; j++) {
                if (_file.available()) {
                    rgbArray[j] = _file.read();
                }
                else {
                    LOGW(SPIF_U, "frame %d pixel %d is not complete", _curFrameIdx, i)
                    return false;
                }
            }
            LOGV(SPIF_U, "pixel %d: %u %u %u", i, rgbArray[0], rgbArray[1], rgbArray[2]);
            _currPixel = makeRGBValue(rgbArray);
            _currPixelCnt = rgbArray[3];

            pframe->pixel[i] = _currPixel;
            pframe->pixel[N_PIXELS-i-1] = _currPixel;
            _currPixelCnt--;
        } 
    }
    // in case height was smaller than N_PIXEL/2: fill with black
    for (int i = nPixels; i < N_PIXELS/2; i++) {
        _currPixel = makeRGBVal(0,0,0);
        pframe->pixel[i] = _currPixel;
        pframe->pixel[N_PIXELS-i-1] = _currPixel;
    }
    
    pframe-> idx = ++_curFrameIdx;

    if (_currPixelCnt != 0) {
        // there should be no count left from current pixel
        LOGW(SPIF_U, "frame %d is not in sync", _curFrameIdx)
    }

    pframe->isLastFrame = _curFrameIdx >= _header.width-1;

    return true;
}

bool SpiffsUtils::hasNextFrame() {
    return _file.available();
}

// other general util functions
/* 
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
*/
#ifndef WITHIN_UNITTEST
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
#endif

