#include "test.h"
#include <AnimationHandler.h>

TEST(AnimationHandler_tests, afterDeclaration){
  ImageCache ic(32, MUTE);
  AnimationHandler aniHandler(ic);
  EXPECT_FALSE(aniHandler.isActive());
  EXPECT_EQ(aniHandler.getCurrentLoop(), 0);
  EXPECT_EQ(aniHandler.getDelayMs(), 0);
}


TEST(AnimationHandler_tests, afterInit){
  ImageCache ic(32, MUTE);
  AnimationHandler aniHandler(ic);
  aniHandler.init(ANIMATIONTYPE_WORM, 20, 3, RED, 100);
  EXPECT_TRUE(aniHandler.isActive());
  EXPECT_EQ(aniHandler.getRegisterLength(), 20);
  EXPECT_EQ(aniHandler.getCurrentLoop(), 0);
  EXPECT_EQ(aniHandler.getDelayMs(), 100);
}

// separate next test not required, since we simply count steps for next

TEST(AnimationHandler_tests, testFinished){
  ImageCache ic(32, MUTE);
  AnimationHandler aniHandler(ic);
  aniHandler.init(ANIMATIONTYPE_WORM, 20, 2, RED, 100);
  for (int i=0; i<19; i++) {
    aniHandler.next();
    EXPECT_EQ(aniHandler.getCurrentLoop(), 0);
    EXPECT_FALSE(aniHandler.isLastLoop());
    EXPECT_FALSE(aniHandler.isLastStep());
  }
  for (int i=0; i<19; i++) {
    aniHandler.next();
    EXPECT_EQ(aniHandler.getCurrentLoop(), 1);
    EXPECT_TRUE(aniHandler.isLastLoop());
    EXPECT_FALSE(aniHandler.isLastStep());
  }

  // last step
  aniHandler.next();
  EXPECT_TRUE(aniHandler.isLastLoop());
  EXPECT_TRUE(aniHandler.isActive());
  EXPECT_TRUE(aniHandler.isLastStep());

  aniHandler.next();
  EXPECT_FALSE(aniHandler.isLastLoop());
  EXPECT_FALSE(aniHandler.isActive());
  EXPECT_FALSE(aniHandler.isLastStep());
}
