#include "test.h"
#include <FadeHandler.h>

TEST(FadeHandler_tests, afterDeclaration){
  FadeHandler fadeHandler;
  EXPECT_FALSE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 0);
  EXPECT_EQ(fadeHandler.getDelayMs(), 0);
}

TEST(FadeHandler_tests, afterInit){
  FadeHandler fadeHandler;
  fadeHandler.init(5000);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), 100);
}

TEST(FadeHandler_tests, afterInitSmallFadeTime1){
  FadeHandler fadeHandler;
  fadeHandler.init(500);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), MIN_FADE_TIME);
}

TEST(FadeHandler_tests, afterInitSmallFadeTime2){
  FadeHandler fadeHandler;
  fadeHandler.init(100);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), MIN_FADE_TIME);
}

TEST(FadeHandler_tests, afterInitSmallFadeTime3){
  FadeHandler fadeHandler;
  fadeHandler.init(10);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), MIN_FADE_TIME);
}


TEST(FadeHandler_tests, testNext){
  FadeHandler fadeHandler;
  fadeHandler.init(5000);
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.98, 2);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.96, 2);
}

TEST(FadeHandler_tests, testNextSmallFadeTime1){
  FadeHandler fadeHandler;
  fadeHandler.init(100);
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.8, 1);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.6, 1);
}

TEST(FadeHandler_tests, testNextSmallFadeTime2){
  FadeHandler fadeHandler;
  fadeHandler.init(10); // smaller than MIN_FADE_TIME, 1 step only
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0, 1);
}

TEST(FadeHandler_tests, testFinished){
  FadeHandler fadeHandler;
  fadeHandler.init(5000);
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  for (int i=0; i<49; i++) {
    fadeHandler.next();
    EXPECT_FALSE(fadeHandler.isLastStep());
  }
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.01, 2);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_FALSE(fadeHandler.isLastStep());

  // last iteration
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.00, 2);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_TRUE(fadeHandler.isLastStep());

  // finished
  fadeHandler.next();
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 0);
  EXPECT_FALSE(fadeHandler.isActive());
  EXPECT_FALSE(fadeHandler.isLastStep());
}

TEST(FadeHandler_tests, testFinishedSmallFadeTime1){
  FadeHandler fadeHandler;
  fadeHandler.init(100);
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  for (int i=0; i<4; i++) {
    fadeHandler.next();
    EXPECT_FALSE(fadeHandler.isLastStep());
  }
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.1, 1);
  EXPECT_TRUE(fadeHandler.isActive());

  // last iteration
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.0, 1);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_TRUE(fadeHandler.isLastStep());

  // finished
  fadeHandler.next();
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 0);
  EXPECT_FALSE(fadeHandler.isActive());
}


TEST(FadeHandler_tests, testFinishedSmallFadeTime2){
  FadeHandler fadeHandler;
  fadeHandler.init(10); // only one step expected
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 1.0);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_FALSE(fadeHandler.isLastStep());
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.getCurrentFadeFactor(), 0.00, 2);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_TRUE(fadeHandler.isLastStep());

  // finished
  fadeHandler.next();
  EXPECT_EQ(fadeHandler.getCurrentFadeFactor(), 0);
  EXPECT_FALSE(fadeHandler.isActive());
  EXPECT_FALSE(fadeHandler.isLastStep());
}
