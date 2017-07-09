#include "test.h"
#include "ledpoi.h"
#include <AbstractHandler.h>
#include <StaticRgbHandler.h>

TEST(StaticRgbHandler_tests, afterDeclaration){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  StaticRgbHandler rgbHandler(ic);
  EXPECT_FALSE(rgbHandler.isActive());
}

TEST(StaticRgbHandler_tests, afterInit){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  StaticRgbHandler rgbHandler(ic);
  rgbHandler.init(255, 0, 255, 10);
  // static: always false
  EXPECT_FALSE(rgbHandler.isActive());
  EXPECT_EQ(rgbHandler.getDelayMs(), 0);
  rgbVal* reg0= rgbHandler.getDisplayFrame(); 
  for (int i=0; i< N_PIXELS; i++){
    if (i<10){
      EXPECT_EQ(reg0[i].r, 255);
      EXPECT_EQ(reg0[i].b, 255);
    }
    else {
      EXPECT_EQ(reg0[i].r, 0);
      EXPECT_EQ(reg0[i].b, 0);
    }
    EXPECT_EQ(reg0[i].g, 0);
  }
}

TEST(StaticRgbHandler_tests, testNextWithDisplayFrameAbstractHandler){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  StaticRgbHandler rgbHandler(ic);
  rgbHandler.init(255, 0, 255, 10);

  EXPECT_FALSE(rgbHandler.isActive());
  EXPECT_EQ(rgbHandler.getDelayMs(), 0);

  AbstractHandler *a = &rgbHandler;
  rgbVal* reg0= a->getDisplayFrame(); 
  for (int i=0; i< N_PIXELS; i++){
    if (i<10){
      EXPECT_EQ(reg0[i].r, 255);
      EXPECT_EQ(reg0[i].b, 255);
    }
    else {
      EXPECT_EQ(reg0[i].r, 0);
      EXPECT_EQ(reg0[i].b, 0);
    }
    EXPECT_EQ(reg0[i].g, 0);
  }
}

