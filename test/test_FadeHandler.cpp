#include "test.h"
#include <FadeHandler.h>

TEST(FadeHandler_tests, afterDeclaration){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  EXPECT_FALSE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 0);
  EXPECT_EQ(fadeHandler.getDelayMs(), 0);
}

TEST(FadeHandler_tests, afterInit){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(5000);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), 100);
}

TEST(FadeHandler_tests, afterInitSmallFadeTime1){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(500);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), MIN_FADE_TIME);
}

TEST(FadeHandler_tests, afterInitSmallFadeTime2){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(100);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), MIN_FADE_TIME);
}

TEST(FadeHandler_tests, afterInitSmallFadeTime3){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(10);
  EXPECT_TRUE(fadeHandler.isActive());
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  EXPECT_EQ(fadeHandler.getDelayMs(), MIN_FADE_TIME);
}

TEST(FadeHandler_tests, testNext){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(5000);
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.98, 2);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.96, 2);
}

TEST(FadeHandler_tests, testNextRegister){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  rgbVal red = makeRGBValue(RED);
  ic.fillRegister(0, red, 10);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(5000);
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  rgbVal* reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ_COLOR(reg0[0], red);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.98, 2);
  reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ(reg0[0].r, 249);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.96, 2);
  reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ(reg0[0].r, 244);
}

TEST(FadeHandler_tests, testNextSmallFadeTime1){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(100);
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.8, 1);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.6, 1);
}

TEST(FadeHandler_tests, testNextSmallFadeTime1Register){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  rgbVal red = makeRGBValue(RED);
  ic.fillRegister(0, red, 10);
  FadeHandler fadeHandler(ic);
  rgbVal* reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ_COLOR(reg0[0],red);
  fadeHandler.init(100);
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ_COLOR(reg0[0],red);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.8, 1);
  reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ(reg0[0].r, 204);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.6, 1);
  reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ(reg0[0].r, 153);
}

TEST(FadeHandler_tests, testNextSmallFadeTime2){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  rgbVal red = makeRGBValue(RED);
  ic.fillRegister(0, red, 10);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(10); // smaller than MIN_FADE_TIME, 1 step only
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0, 1);
  rgbVal* reg0= fadeHandler.getDisplayFrame();
  EXPECT_EQ(reg0[0].r, 0);
}

TEST(FadeHandler_tests, testFinished){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(5000);
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  for (int i=0; i<49; i++) {
    fadeHandler.next();
  }
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.01, 2);
  EXPECT_TRUE(fadeHandler.isActive());

  // last iteration
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.00, 2);
  EXPECT_TRUE(fadeHandler.isActive());

  // finished
  fadeHandler.next();
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 0);
  EXPECT_FALSE(fadeHandler.isActive());
}

TEST(FadeHandler_tests, testFinishedSmallFadeTime1){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(100);
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  for (int i=0; i<4; i++) {
    fadeHandler.next();
  }
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.1, 1);
  EXPECT_TRUE(fadeHandler.isActive());

  // last iteration
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.0, 1);
  EXPECT_TRUE(fadeHandler.isActive());

  // finished
  fadeHandler.next();
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 0);
  EXPECT_FALSE(fadeHandler.isActive());
}


TEST(FadeHandler_tests, testFinishedSmallFadeTime2){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  FadeHandler fadeHandler(ic);
  fadeHandler.init(10); // only one step expected
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 1.0);
  EXPECT_TRUE(fadeHandler.isActive());
  fadeHandler.next();
  EXPECT_EQ_FLOATPREC(fadeHandler.__getCurrentFadeFactor(), 0.00, 2);
  EXPECT_TRUE(fadeHandler.isActive());

  // finished
  fadeHandler.next();
  EXPECT_EQ(fadeHandler.__getCurrentFadeFactor(), 0);
  EXPECT_FALSE(fadeHandler.isActive());
}
