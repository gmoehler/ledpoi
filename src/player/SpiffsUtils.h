#ifndef SPIFFS_UTILS
#define SPIFFS_UTILS

#include "ledpoi.h"
#include "ledpoi_utils.h"

#ifndef WITHIN_UNITTEST
  #include <FS.h>
  #include <SPIFFS.h>
  #include <esp_partition.h>
#endif

struct ImageHeader {   
    uint8_t codec;       
    uint8_t height;
    uint16_t width; 
};

class SpiffsUtils
{
public:
  void setup();
  bool getNextFrame(PixelFrame* pFrame);
  bool getNextFrameNoEncoding(PixelFrame* pFrame);
  bool getNextFrameRuntimeEncoding(PixelFrame* pFrame);
  bool hasNextFrame();
  void openFile(const char * path);
  void closeFile();
  bool readImageHeader();
  void clearImageHeader();
  ImageHeader getHeader();
  
  void listDir(const char * dirname, uint8_t levels);
  void createDir(const char * path);
  void removeDir(const char * path);
  void readFile(const char * path);
  void writeFile(const char * path, const char * message);
  void appendFile(const char * path, const char * message);
  void renameFile(const char * path1, const char * path2);
  void deleteFile(const char * path);
  void listPartitions();
  void example();

private: 
  fs::File _file;
  ImageHeader _header;
  uint16_t _curFrameIdx = 0;
  rgbVal _currPixel;
  uint8_t _currPixelCnt = 0;
};

#endif