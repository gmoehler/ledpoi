#include "test.h"
#include <AnimationHandler.h>

TEST(AnimationHandler_tests, afterDeclaration){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  AnimationHandler aniHandler(ic);
  EXPECT_FALSE(aniHandler.isActive());
  EXPECT_EQ(aniHandler.getCurrentLoop(), 0);
  EXPECT_EQ(aniHandler.getDelayMs(), 0);
}


TEST(AnimationHandler_tests, afterInit){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  AnimationHandler aniHandler(ic);
  aniHandler.init(ANIMATIONTYPE_WORM, 20, 3, RED, 100);
  EXPECT_TRUE(aniHandler.isActive());
  EXPECT_EQ(aniHandler.getCurrentLoop(), 0);
  EXPECT_EQ(aniHandler.getDelayMs(), 100);
}

// separate next test not required, since we simply count steps for next

TEST(AnimationHandler_tests, testFinished){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
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

TEST(AnimationHandler_tests, testFinishedWitDisplayFrame){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  AnimationHandler aniHandler(ic);
  aniHandler.init(ANIMATIONTYPE_WORM, 20, 2, RED, 100);
  rgbVal red = makeRGBValue(RED);
  rgbVal black = makeRGBValue(BLACK);
  rgbVal* reg0= aniHandler.getDisplayFrame();
  EXPECT_EQ_COLOR(reg0[0], red);
  EXPECT_EQ_COLOR(reg0[1], black);

  for (int i=0; i<19; i++) {
    aniHandler.next();
    EXPECT_EQ(aniHandler.getCurrentLoop(), 0);
    EXPECT_FALSE(aniHandler.isLastLoop());
    EXPECT_FALSE(aniHandler.isLastStep());
    rgbVal* reg0= aniHandler.getDisplayFrame();
    //printf("0 %d\n", i);
    //    ic.printRegister(0);
    EXPECT_EQ_COLOR(reg0[i+1], red);
    EXPECT_EQ_COLOR(reg0[i], black);
  }
  for (int i=0; i<19; i++) {
    aniHandler.next();
    EXPECT_EQ(aniHandler.getCurrentLoop(), 1);
    EXPECT_TRUE(aniHandler.isLastLoop());
    EXPECT_FALSE(aniHandler.isLastStep());
    rgbVal* reg0= aniHandler.getDisplayFrame();
    //printf("1 %d\n", i);
    //ic.printRegister(0);
    EXPECT_EQ_COLOR(reg0[i], red);
    if (i>0) {
      EXPECT_EQ_COLOR(reg0[i-1], black);
    }
  }

  // last step
  aniHandler.next();
  EXPECT_TRUE(aniHandler.isLastLoop());
  EXPECT_TRUE(aniHandler.isActive());
  EXPECT_TRUE(aniHandler.isLastStep());
  reg0= aniHandler.getDisplayFrame();
  EXPECT_EQ_COLOR(reg0[19], red);
  EXPECT_EQ_COLOR(reg0[18], black);

  aniHandler.next();
  EXPECT_FALSE(aniHandler.isLastLoop());
  EXPECT_FALSE(aniHandler.isActive());
  EXPECT_FALSE(aniHandler.isLastStep());
  reg0= aniHandler.getDisplayFrame();
  //ic.printRegister(0);
  aniHandler.printState();
  for (int i=0; i<20; i++){
   /// printf("%d\n", i);
    EXPECT_EQ_COLOR(reg0[i], black);
  }
}
