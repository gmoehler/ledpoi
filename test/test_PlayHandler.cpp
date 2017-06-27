#include "test.h"
#include <PlayHandler.h>

TEST(PlayHandler_tests, afterDeclaration){
  PlayHandler playHandler;
  EXPECT_FALSE(playHandler.isActive());
  EXPECT_EQ(playHandler.getCurrentFrame(), 0);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  EXPECT_EQ(playHandler.getDelayMs(), 0);
}

TEST(PlayHandler_tests, afterInit){
  PlayHandler playHandler;
  playHandler.init(10, 50, 100, 3);
  EXPECT_TRUE(playHandler.isActive());
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  EXPECT_EQ(playHandler.getDelayMs(), 100);
}

TEST(PlayHandler_tests, testNext){
  PlayHandler playHandler;
  playHandler.init(10, 12, 100, 3);
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 11);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 12);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 1);
}

TEST(PlayHandler_tests, testFinished){
  PlayHandler playHandler;
  playHandler.init(10, 12, 100, 3);
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  for (int i=0; i<3*3-2; i++) {
    playHandler.next();
    EXPECT_FALSE(playHandler.isLastStep());
  }
  // last iteration
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 12);
  EXPECT_EQ(playHandler.getCurrentLoop(), 2);
  EXPECT_TRUE(playHandler.isActive());
  EXPECT_TRUE(playHandler.isLastStep());

  // finished
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 12);
  EXPECT_EQ(playHandler.getCurrentLoop(), 2);
  EXPECT_FALSE(playHandler.isActive());
  EXPECT_FALSE(playHandler.isLastStep());
}

TEST(PlayHandler_tests, testBackwardComplete){
  PlayHandler playHandler;
  EXPECT_FALSE(playHandler.isActive());

  playHandler.init(12, 10, 100, 3);
  EXPECT_EQ(playHandler.getCurrentFrame(), 12);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  EXPECT_EQ(playHandler.getDelayMs(), 100);

  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 11);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);

  for (int i=0; i<3*3-3; i++) {
    playHandler.next();
    EXPECT_FALSE(playHandler.isLastStep());
  }
  // last iteration
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 2);
  EXPECT_TRUE(playHandler.isActive());
  EXPECT_TRUE(playHandler.isLastStep());

  // finished
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 2);
  EXPECT_FALSE(playHandler.isActive());
  EXPECT_FALSE(playHandler.isLastStep());
}
