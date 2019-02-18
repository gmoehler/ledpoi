#include "ledpoi_utils.h"

class SpiffsUtils
{
public:
  void setup();
  bool getNextFrame(PixelFrame* pFrame);
  bool hasNextFrame();
  void openFile(const char * path);
  void closeFile();
  
  void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
  void createDir(fs::FS &fs, const char * path);
  void removeDir(fs::FS &fs, const char * path);
  void readFile(fs::FS &fs, const char * path);
  void writeFile(fs::FS &fs, const char * path, const char * message);
  void appendFile(fs::FS &fs, const char * path, const char * message);
  void renameFile(fs::FS &fs, const char * path1, const char * path2);
  void deleteFile(fs::FS &fs, const char * path);
  void listPartitions();
  void example();

private: 
  File _file;
  uint16_t _curFrame = 0;
};