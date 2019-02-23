#ifndef SPIFFS_UTILS
#define SPIFFS_UTILS

#include "ledpoi.h"
#include "ledpoi_utils.h"

#include <FS.h>
#include <SPIFFS.h>
#include <esp_partition.h>

class SpiffsUtils
{
public:
  void setup();
  bool getNextFrame(PixelFrame* pFrame);
  bool hasNextFrame();
  void openFile(const char * path);
  void closeFile();
  
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
  File _file;
  uint16_t _curFrame = 0;
};

#endif