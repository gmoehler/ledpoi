#include "test.h"
#include "ledpoi.h"
#include <AbstractHandler.h>
#include <DisplayIpHandler.h>

TEST(DisplayIpHandler_tests, afterDeclaration){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  DisplayIpHandler displayIpHandler(ic);
  EXPECT_FALSE(displayIpHandler.isActive());
}

TEST(DisplayIpHandler_tests, afterInit){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  DisplayIpHandler displayIpHandler(ic);
  displayIpHandler.init(5, false);
  // static: always false
  EXPECT_FALSE(displayIpHandler.isActive());
  EXPECT_EQ(displayIpHandler.getDelayMs(), 0);
  rgbVal* reg0= displayIpHandler.getDisplayFrame(); 
  for (int i=0; i< N_PIXELS; i++){
    if (i==5){
      EXPECT_EQ(reg0[i].b, 64);
    }
    else {
      EXPECT_EQ(reg0[i].b, 0);
    }
    EXPECT_EQ(reg0[i].r, 0);
    EXPECT_EQ(reg0[i].g, 0);
  }
}

TEST(DisplayIpHandler_tests, afterInitWithBAckground){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  DisplayIpHandler displayIpHandler(ic);
  displayIpHandler.init(5, true);
  // static: always false
  EXPECT_FALSE(displayIpHandler.isActive());
  EXPECT_EQ(displayIpHandler.getDelayMs(), 0);
  rgbVal* reg0= displayIpHandler.getDisplayFrame(); 
  for (int i=0; i< N_PIXELS; i++){
    if (i==5){
      //pale green
      EXPECT_EQ(reg0[i].r, 0);
      EXPECT_EQ(reg0[i].g, 0);
      EXPECT_EQ(reg0[i].b, 64);
    }
    else if (i<10) {
      // palewhite background
      EXPECT_EQ(reg0[i].r, 8);
      EXPECT_EQ(reg0[i].g, 8);
      EXPECT_EQ(reg0[i].b, 8);
    }
    else {
      // palewhite background
      EXPECT_EQ(reg0[i].r, 0);
      EXPECT_EQ(reg0[i].g, 0);
      EXPECT_EQ(reg0[i].b, 0);
    }
  }
}

TEST(DisplayIpHandler_tests, afterNext){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  DisplayIpHandler displayIpHandler(ic);
  displayIpHandler.init(5, false);
  AbstractHandler *a = &displayIpHandler;
  a->next();
  // static: always false
  EXPECT_FALSE(a->isActive());
  EXPECT_EQ(a->getDelayMs(), 0);
  rgbVal* reg0= a->getDisplayFrame(); 
  for (int i=0; i< N_PIXELS; i++){
    if (i==5){
      EXPECT_EQ(reg0[i].b, 64);
    }
    else {
      EXPECT_EQ(reg0[i].b, 0);
    }
    EXPECT_EQ(reg0[i].r, 0);
    EXPECT_EQ(reg0[i].g, 0);
  }
}
