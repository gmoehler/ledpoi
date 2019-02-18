#include "test.h"

#include "memory/ProgramCache.h"

uint8_t numCommands = 7;
RawPoiCommand rawCmd[] = {
  {{ANIMATE,  0,  0,  0,  0,  20}}, 
  {{DISPLAY_IP,  0,  0,  0,  1, 254}}, 
  {{SYNC_POINT, 11, 0, 0, 0, 0}},
  {{DISPLAY_IP,  4,  1,  0, 10, 254}}, 
  {{LOOP_START, 12, 0, 0, 1, 2}},
  {{PLAY_FRAMES,  0,  1,  2,  4, 254}},
  {{LOOP_END, 12, 0, 0, 0, 0}}
};

void readCmds(ProgramCache& pCache) {
  for (int i=0; i<numCommands; i++) {
    PoiCommand cmd(rawCmd[i]);
    pCache.addCommand(cmd);
  }
}

TEST(ProgramCache_tests, checkNumLines){
  ProgramCache pCache;
  readCmds(pCache);
  pCache.printProgram();

  EXPECT_EQ(pCache.getNumProgLines(), numCommands);
}

TEST(ProgramCache_tests, checkClearProg){
  ProgramCache pCache;
  readCmds(pCache);
  EXPECT_EQ(pCache.getNumProgLines(), numCommands);
  pCache.clearProgram();
  EXPECT_EQ(pCache.getNumProgLines(), 0);
}


TEST(ProgramCache_tests, checkProgLines){
  ProgramCache pCache;
  readCmds(pCache);

  for (int i=0; i<numCommands; i++) {
    PoiCommand cmd1(rawCmd[i]);
    PoiCommand cmd2 = pCache.getPoiCommand(i);
    // printf("%d %s %s\n", i, cmd1.toString().c_str(), cmd2.toString().c_str());
    EXPECT_TRUE(cmd1 == cmd2);
  }
}

TEST(ProgramCache_tests, checkLabel){
  ProgramCache pCache;
  readCmds(pCache);

  EXPECT_EQ(pCache.getNumLabels(), 1);
  EXPECT_EQ(pCache.getLineOfLabel(12), 4);
  EXPECT_EQ(pCache.getNumIterations(12), 258);
}

TEST(ProgramCache_tests, checkSyncPoint){
  ProgramCache pCache;
  readCmds(pCache);

  EXPECT_EQ(pCache.getNumSyncPoints(), 1);
  EXPECT_EQ(pCache.getLineOfSyncPoint(11), 2);
}

TEST(ProgramCache_tests, loadFromFlash) {

  // first store cms directly to flash
  PoiFlashMemory flash;
  uint8_t prog[N_PROG_STEPS][N_CMD_FIELDS];
  for (int i=0; i< numCommands; i++){
    for (int j=0; j< N_CMD_FIELDS; j++){
      prog [i][j] = rawCmd[i].field[j];
    }
  }
  flash.saveNumProgramSteps(numCommands);
  flash.saveProgram(&prog[0][0]);

  // now read flash to program cache
  ProgramCache pCache;
	uint16_t numProgLines = 0;
  flash.loadNumProgramSteps(&numProgLines);
  EXPECT_EQ(numProgLines, N_PROG_STEPS);
	
  flash.loadProgram(pCache.getRawProgramDataPrt());
  for (int i=0; i<numCommands; i++) {
    PoiCommand cmd1(rawCmd[i]);
    PoiCommand cmd2 = pCache.getPoiCommand(i);
    // printf("%d %s %s\n", i, cmd1.toString().c_str(), cmd2.toString().c_str());
    EXPECT_TRUE(cmd1 == cmd2);
  }

  bool ok = pCache.parseProgram(numCommands);
  EXPECT_TRUE(ok);
}

TEST(ProgramCache_tests, saveToFlash) {
  // now read flash to program cache
  ProgramCache pCache;
  readCmds(pCache);

  // save to flash
  PoiFlashMemory flash;
  bool ok = flash.saveProgram(pCache.getRawProgramDataPrt());
  EXPECT_TRUE(ok);

  // check was was saved
  uint8_t prog[N_PROG_STEPS][N_CMD_FIELDS];
  ok = flash.loadProgram(&prog[0][0]);
  EXPECT_TRUE(ok);
  
  for (int i=0; i< numCommands; i++){
    for (int j=0; j< N_CMD_FIELDS; j++){
      EXPECT_EQ(prog [i][j], rawCmd[i].field[j]);
    }
  }
}

