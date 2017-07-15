#include "test.h"
#include <PoiProgramHandler.h>

// smaller init helper
PoiProgramHandler createProghandler(){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  PlayHandler playHandler(ic);;
  PoiFlashMemory flash;
  PoiProgramHandler progHandler(playHandler, flash, QUIET);
  return progHandler;
}

TEST(PoiProgramHandler_tests, afterDeclaration){
  PoiProgramHandler progHandler = createProghandler();
  EXPECT_FALSE(progHandler.isActive());
  EXPECT_EQ(progHandler.getCurrentScene(), 0);
  EXPECT_EQ(progHandler.__getCurrentFrame(), 0);
  EXPECT_EQ(progHandler.getDelayMs(), 0);
}

TEST(PoiProgramHandler_tests, addProgram){
  PoiProgramHandler progHandler = createProghandler();
  char unsigned cmd1[7] = {253, PLAY_FRAMES,0,200,0,100};
  progHandler.addCmdToProgram(cmd1);
  char unsigned cmd2[7] = {253, PROG_END,0,0,0,0};
  progHandler.addCmdToProgram(cmd2);
  EXPECT_EQ(progHandler.__getNumProgSteps(), 1); // PROG_END does not count
}

TEST(PoiProgramHandler_tests, programWithSyncPoint){
  PoiProgramHandler progHandler = createProghandler();
  char unsigned cmd1[7] = {253, SYNC_POINT,2,0,0,0};
  progHandler.addCmdToProgram(cmd1);
  char unsigned cmd2[7] = {253, LABEL,5,0,0,0};
  progHandler.addCmdToProgram(cmd2);
  char unsigned cmd3[7] = {253, PROG_END,0,0,0,0};
  progHandler.addCmdToProgram(cmd3);
  EXPECT_EQ(progHandler.__getNumSyncPoints(), 1); 
  EXPECT_EQ(progHandler.__getNumLabels(), 1); 
}


//TODO more testing on how handler work with programs
