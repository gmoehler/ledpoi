#include "test.h"
#include "mock_PoiFlashMemory.h"

// test the mock

TEST(PoiFlashMemory_tests, testnvm){

  PoiFlashMemory flash;

  flash.saveNumProgramSteps(11);
  uint16_t numProgSteps = 0;
  flash.loadNumProgramSteps(&numProgSteps);
  EXPECT_EQ(numProgSteps, 11);

  flash.saveIpIncrement(11);
  uint8_t ipIncrement = 0;
  flash.loadIpIncrement(&ipIncrement);
  EXPECT_EQ(ipIncrement, 11);

  flash.saveNumScenes(11);
  uint8_t numScenes = 0;
  flash.loadNumScenes(&numScenes);
  EXPECT_EQ(numScenes, 11);
}

TEST(PoiFlashMemory_tests, saveProgram){

  PoiFlashMemory flash;
  uint8_t prog[N_PROG_STEPS][N_CMD_FIELDS];
  for (int i=0; i< N_PROG_STEPS; i++){
    for (int j=0; j< N_CMD_FIELDS; j++){
      prog [i][j] = j;
    }
  }

  flash.saveProgram(&prog[0][0]);

  // clear data
  for (int i=0; i< N_PROG_STEPS; i++){
    for (int j=0; j< N_CMD_FIELDS; j++){
      prog [i][j] = 0;
    }
  }
  flash.loadProgram(&prog[0][0]);
  for (int i=0; i< N_PROG_STEPS; i++){
    for (int j=0; j< N_CMD_FIELDS; j++){
      EXPECT_EQ(prog [i][j],j);
    }
  }

}

// saveImage is covered by test_ImageCache