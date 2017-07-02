#include "mock_PoiFlashMemory.h"

  void PoiFlashMemory::setup(LogLevel logLevel, uint8_t *initImageData){
    return;
  }

  bool PoiFlashMemory::saveImage(uint8_t scene, uint8_t* imageData){
    //TODO
    return true;
  }

  bool PoiFlashMemory::loadImage(uint8_t scene, uint8_t* imageData){
    //TODO
    return true;
  }

  bool PoiFlashMemory::saveProgram(uint8_t* programData, uint8_t size_x, uint8_t size_y){
   for (int i=0; i< N_PROG_STEPS; i++){
      for (int j=0; j< N_PROG_FIELDS; j++){
        _prog[i][j] = programData [ i*N_PROG_FIELDS+j ];
      }
    }
    return true;
  }

  bool PoiFlashMemory::loadProgram(uint8_t* programData) {
    for (int i=0; i< N_PROG_STEPS; i++){
      for (int j=0; j< N_PROG_FIELDS; j++){
         programData [ i*N_PROG_FIELDS+j ] = _prog[i][j];
      }
    }
    return true;
  }

  bool PoiFlashMemory::saveNumProgramSteps(uint8_t numProgSteps) {
    _numProgSteps = numProgSteps;
    return true;
  }
  bool PoiFlashMemory::loadNumProgramSteps(uint8_t* numProgSteps){
    *numProgSteps = _numProgSteps;
    return true;
  }

  bool PoiFlashMemory::saveIpIncrement(uint8_t ipIncrement){
    _ipIncrement = ipIncrement;
    return true;
  }
  bool PoiFlashMemory::loadIpIncrement(uint8_t* ipIncrement){
    *ipIncrement = _ipIncrement;
    return true;
  }

  bool PoiFlashMemory::saveNumScenes(uint8_t numScenes){
    _numScenes = numScenes;
    return true;
  }
  bool PoiFlashMemory::loadNumScenes(uint8_t* numScenes){
    *numScenes = _numScenes;
    return true;
  }

  bool PoiFlashMemory::_initializeImageMemory(){
    //TODO
    _numScenes=0;
    return true;
  }
  bool PoiFlashMemory::eraseNvsFlashPartition(){
    _numProgSteps = 0;
    _numScenes = 0;
    _ipIncrement = 0;
    return true;
  }
  bool PoiFlashMemory::_initializeProgramMemory(){
    for (int i=0; i< N_PROG_STEPS; i++){
      for (int j=0; j< N_PROG_FIELDS; j++){
          _prog[i][j] = 0;
      }
    }
    _numProgSteps=0;
    return true;
  }


  void PoiFlashMemory::listPartitions(){
    // not required for mock
    return;
  }
  void PoiFlashMemory::printContents(){
    // not required for mock
    return;
  }

  uint32_t PoiFlashMemory::getSizeOfImageSection(){
    // fixed size for image section
    return N_NUM_FLASH_SECTIONS_PER_IMAGE * SPI_FLASH_SEC_SIZE;
  }
