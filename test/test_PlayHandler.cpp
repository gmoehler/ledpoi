#include "test.h"
#include <PlayHandler.h>

TEST(PlayHandler_tests, afterDeclaration){
  ImageCache ic(32, MUTE);
  PlayHandler playHandler(ic);
  EXPECT_FALSE(playHandler.isActive());
  EXPECT_EQ(playHandler.getCurrentFrame(), 0);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  EXPECT_EQ(playHandler.getDelayMs(), 0);
}

TEST(PlayHandler_tests, afterInit){
  ImageCache ic(32, MUTE);
  PlayHandler playHandler(ic);
  playHandler.init(10, 50, 100, 3);
  EXPECT_TRUE(playHandler.isActive());
  EXPECT_EQ(playHandler.getCurrentFrame(), 10);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  EXPECT_EQ(playHandler.getDelayMs(), 100);
}


TEST(PlayHandler_tests, testNext){
  ImageCache ic(32, MUTE);
  PlayHandler playHandler(ic);
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

TEST(PlayHandler_tests, testNextWithRegister){
  ImageCache ic(3*N_FRAMES*N_PIXELS, MUTE);
  rgbVal* reg0= ic.getRegister(0);
  
  for (int p=0; p<N_PIXELS; p++){
    ic.setPixel(0, p, RED);
  }
  for (int p=0; p<N_PIXELS; p++){
    ic.setPixel(1, p, BLUE);
  }
  for (int p=0; p<N_PIXELS; p++){
    ic.setPixel(2 ,p, GREEN);
  }
  PlayHandler playHandler(ic);
  playHandler.init(0, 4, 100, 3);
  EXPECT_EQ(playHandler.getCurrentFrame(), 0);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
 rgbVal rgb0 = reg0[0];
  EXPECT_EQ(rgb0.r, 255);
  EXPECT_EQ(rgb0.g, 0);
  EXPECT_EQ(rgb0.b, 0);
 
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 1);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  rgb0 = reg0[0];
  EXPECT_EQ(rgb0.r, 0);
  EXPECT_EQ(rgb0.g, 0);
  EXPECT_EQ(rgb0.b, 255);
  
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 2);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  rgb0 = reg0[0];
  EXPECT_EQ(rgb0.r, 0);
  EXPECT_EQ(rgb0.g, 255);
  EXPECT_EQ(rgb0.b, 0);
 
  playHandler.next();
  EXPECT_EQ(playHandler.getCurrentFrame(), 3);
  EXPECT_EQ(playHandler.getCurrentLoop(), 0);
  EXPECT_EQ(rgb0.r, 0);
}

TEST(PlayHandler_tests, testFinished){
  ImageCache ic(32, MUTE);
  PlayHandler playHandler(ic);
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
  ImageCache ic(32, MUTE);
  PlayHandler playHandler(ic);
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
