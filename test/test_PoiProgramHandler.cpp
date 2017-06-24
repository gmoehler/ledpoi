#include "test.h"
#include <PoiProgramHandler.h>

// smaller init helper
PoiProgramHandler createProghandler(){
  PlayHandler playHandler;
  PoiFlashMemory flash;
  PoiProgramHandler progHandler(playHandler, flash, QUIET);
  return progHandler;
}

TEST(PoiProgramHandler_tests, afterDeclaration){
  PoiProgramHandler progHandler = createProghandler();
  EXPECT_FALSE(progHandler.isActive());
  EXPECT_EQ(progHandler.getCurrentScene(), 0);
  EXPECT_EQ(progHandler.getCurrentFrame(), 0);
  EXPECT_EQ(progHandler.getDelayMs(), 0);
}

TEST(PoiProgramHandler_tests, addProgram){
  PoiProgramHandler progHandler = createProghandler();
  char cmd1[7] = {-3, PLAY_FRAMES,0,-55,0,100};
  progHandler.addCmdToProgram(cmd1);
  char cmd2[7] = {-3, PROG_END,0,0,0,0};
  progHandler.addCmdToProgram(cmd2);
  EXPECT_EQ(progHandler.getNumProgSteps(), 1); // PROG_END does not count

  EXPECT_TRUE(progHandler.checkProgram());
}
