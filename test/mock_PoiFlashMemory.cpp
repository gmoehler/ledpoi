#include "mock_PoiFlashMemory.h"

  void PoiFlashMemory::setup(LogLevel logLevel, uint8_t *initImageData){
    return;
  }

  bool PoiFlashMemory::saveImage(uint8_t scene, uint8_t* imageData){
    return true;
  }

  bool PoiFlashMemory::loadImage(uint8_t scene, uint8_t* imageData){
    return true;
  }

  bool PoiFlashMemory::saveProgram(uint8_t* programData, uint8_t size_x, uint8_t size_y){
    return true;
  }
  bool PoiFlashMemory::loadProgram(uint8_t* programData) {
      return true;
  }

  bool PoiFlashMemory::saveNumProgramSteps(uint8_t numProgSteps) {
    return true;
  }
  bool PoiFlashMemory::loadNumProgramSteps(uint8_t* numProgSteps){
    return true;
  }

  bool PoiFlashMemory::saveIpIncrement(uint8_t ipIncrement){
    return true;
  }
  bool PoiFlashMemory::loadIpIncrement(uint8_t* ipIncrement){
    return true;
  }

  bool PoiFlashMemory::saveNumScenes(uint8_t numScenes){
    return true;
  }
  bool PoiFlashMemory::loadNumScenes(uint8_t* numScenes){
    return true;
  }

  bool PoiFlashMemory::eraseImages(){
    return true;
  }
  bool PoiFlashMemory::eraseNvsFlashPartition(){
    return true;
  }
  bool PoiFlashMemory::eraseProgram(){
    return true;
  }


  void PoiFlashMemory::listPartitions(){
    return;
  }
  void PoiFlashMemory::printContents(){
    return;
  }

  uint32_t PoiFlashMemory::getSizeOfImageSection(){
    return 0;
  }
