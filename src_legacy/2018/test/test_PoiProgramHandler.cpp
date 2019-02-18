#include "test.h"
#include <program/PoiProgramHandler.h>

uint8_t numCommands1 = 7;
RawPoiCommand rawCmd1[] = {
  {{ANIMATE,  0,  0,  0,  0,  20}}, 
  {{DISPLAY_IP,  0,  0,  0,  1, 254}}, 
  {{SYNC_POINT, 11, 0, 0, 0, 0}},
  {{DISPLAY_IP,  4,  1,  0, 10, 254}}, 
  {{LOOP_START, 12, 0, 0, 0, 3}},
  {{PLAY_FRAMES,  0,  1,  2,  4, 254}},
  {{LOOP_END, 12, 0, 0, 0, 0}}
};

void loadPrgToCache(RawPoiCommand rawCmd[], uint8_t numCommands) {
  programCache.clearProgram();
  for (int i=0; i<numCommands; i++) {
    PoiCommand cmd(rawCmd[i]);
    programCache.addCommand(cmd);
  }
}

TEST(PoiProgramHandler_tests, readProgLines){
  
  loadPrgToCache(rawCmd1, numCommands1);
  PoiProgramHandler progHandler;
  
  // expected sequence of commands
  int expectedCmdNum[] = {
  	0 ,1 ,3 ,5, 5, 5};
  int numSteps = 6;
  
  for(int i=0; i< numSteps; i++) {
    // first next() does not advance prg
    bool hasNext = progHandler.next();
    EXPECT_TRUE(i==numSteps-1 || hasNext);
  
    int expCmd = expectedCmdNum[i];
    PoiCommand cmd1(rawCmd1[expCmd]);
    PoiCommand cmd2 = progHandler.getCurrentCommand();
    //printf("%d: %s %s\n", expCmd, cmd1.toString().c_str(), cmd2.toString().c_str());
    EXPECT_TRUE(cmd1 == cmd2);
  }
  EXPECT_FALSE(progHandler.next());
}

TEST(PoiProgramHandler_tests, jumptosync){
  
  loadPrgToCache(rawCmd1, numCommands1);
  PoiProgramHandler progHandler;
  
  //jump to sync 11
  uint8_t progLine = programCache.getLineOfSyncPoint(11);
  progHandler.init(progLine);
  
  // expected sequence of commands
  int expectedCmdNum[] = {
  	3 ,5, 5, 5};
  int numSteps =4;
  
  for(int i=0; i< numSteps; i++) {
    // first next() does not advance prg
    bool hasNext = progHandler.next();
    EXPECT_TRUE(i==numSteps-1 || hasNext);
  
    int expCmd = expectedCmdNum[i];
    PoiCommand cmd1(rawCmd1[expCmd]);
    PoiCommand cmd2 = progHandler.getCurrentCommand();
    //printf("%d: %s %s\n", expCmd, cmd1.toString().c_str(), cmd2.toString().c_str());
    EXPECT_TRUE(cmd1 == cmd2);
  }
  
  EXPECT_FALSE(progHandler.next());
}
