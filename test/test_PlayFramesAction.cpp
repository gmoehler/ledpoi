#include "test.h"
#include "player/PlayFramesAction.h"

TEST(playFramesAction_tests, afterDeclaration){
  PlayFramesAction playFramesAction;
  playFramesAction.printInfo("pre:");
  EXPECT_FALSE(playFramesAction.isActive());
}


TEST(playFramesAction_tests, afterInit){
  PlayFramesAction playFramesAction;
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 10, 50, 3, 0, 100}};
  PixelFrame sframe;
  ActionOptions options;

  playFramesAction.init(rawCmd0, &sframe, options);
  EXPECT_TRUE(playFramesAction.isActive( ));
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 10);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
}

TEST(playFramesAction_tests, printStateForStaticFrame){
  PlayFramesAction playFramesAction;
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 10, 10, 1, 0, 100}};
  PixelFrame sframe;
  ActionOptions options;

  playFramesAction.init(rawCmd0, &sframe, options);
  playFramesAction.printInfo("pre:");
  EXPECT_TRUE(playFramesAction.isActive());
  EXPECT_STREQ(playFramesAction.getActionName(), "Play Frame");
}

TEST(playFramesAction_tests, wrongAaction){
  PlayFramesAction playFramesAction;
  RawPoiCommand rawCmd0 = {{ANIMATE, 10, 50, 3, 0, 100}};
  PixelFrame sframe;
  ActionOptions options;

  playFramesAction.init(rawCmd0, &sframe, options);
}

TEST(playFramesAction_tests, testNext){
  PlayFramesAction playFramesAction;
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 10, 12, 3, 0, 100}};
  PixelFrame sframe;
  ActionOptions options;
  
  playFramesAction.init(rawCmd0, &sframe, options);
  EXPECT_FALSE(sframe.isLastFrame);
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 10);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  playFramesAction.next();
  playFramesAction.printState();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 11);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 12);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 10);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 1);
}

TEST(playFramesAction_tests, testNextWithPixelFrame){
 
  // 3 frames: red, blue, green
  for (int p=0; p<N_PIXELS; p++){
    imageCache.setPixel(0, p, RED);
  }
  for (int p=0; p<N_PIXELS; p++){
    imageCache.setPixel(1, p, BLUE);
  }
  for (int p=0; p<N_PIXELS; p++){
    imageCache.setPixel(2 ,p, GREEN);
  }
  
  PixelFrame sframe;
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 0, 4, 3, 0, 100}};
  PlayFramesAction playFramesAction;
  ActionOptions options;
  
  playFramesAction.init(rawCmd0, &sframe, options);
  
  // test the first 3 frames
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 0);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  rgbVal rgb0 = sframe.pixel[0];
  EXPECT_EQ(rgb0.r, 254);
  EXPECT_EQ(rgb0.g, 0);
  EXPECT_EQ(rgb0.b, 0);
 
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 1);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  rgb0 = sframe.pixel[0];
  EXPECT_EQ(rgb0.r, 0);
  EXPECT_EQ(rgb0.g, 0);
  EXPECT_EQ(rgb0.b, 254);

  playFramesAction.setDimFactor(0.1);
  
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 2);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  rgb0 = sframe.pixel[0];
  EXPECT_EQ(rgb0.r, 0);
  EXPECT_EQ(rgb0.g, 25); // 254/10
  EXPECT_EQ(rgb0.b, 0);
 
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 3);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  EXPECT_EQ(rgb0.r, 0);
}

TEST(playFramesAction_tests, testWithDimFactor){
 
  // 3 frames: red, blue, green
  for (int p=0; p<N_PIXELS; p++){
    imageCache.setPixel(0, p, RED);
  }
  for (int p=0; p<N_PIXELS; p++){
    imageCache.setPixel(1, p, BLUE);
  }
  for (int p=0; p<N_PIXELS; p++){
    imageCache.setPixel(2 ,p, GREEN);
  }
  
  PixelFrame sframe;
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 0, 4, 3, 0, 100}};
  PlayFramesAction playFramesAction;
  ActionOptions options;
  options.dimFactor = 0.1;
  
  playFramesAction.init(rawCmd0, &sframe, options);
  playFramesAction.printInfo("");
  
  // test the first 3 frames
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 0);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  rgbVal rgb0 = sframe.pixel[0];
  EXPECT_EQ(rgb0.r, 25); // 254/10
  EXPECT_EQ(rgb0.g, 0);
  EXPECT_EQ(rgb0.b, 0);
 
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 1);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  rgb0 = sframe.pixel[0];
  EXPECT_EQ(rgb0.r, 0);
  EXPECT_EQ(rgb0.g, 0);
  EXPECT_EQ(rgb0.b, 25); // 254/10
  
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 2);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  rgb0 = sframe.pixel[0];
  EXPECT_EQ(rgb0.r, 0);
  EXPECT_EQ(rgb0.g, 25); // 254/10
  EXPECT_EQ(rgb0.b, 0);
 
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 3);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  EXPECT_EQ(rgb0.r, 0);
}

TEST(playFramesAction_tests, testFinishedAbstract){
  
  PixelFrame sframe;
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 10, 12, 3, 0, 100}};
  PlayFramesAction playFramesAction;
  ActionOptions options;
  
  playFramesAction.init(rawCmd0, &sframe, options);
  AbstractAction *a = dynamic_cast<AbstractAction*>(&playFramesAction);
 
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 10);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  EXPECT_FALSE(sframe.isLastFrame);

  for (int i=0; i<3*3-2; i++) {
    a->next();
    EXPECT_FALSE(sframe.isLastFrame);
  }
  // last iteration
  a->next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 12);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 2);
  EXPECT_TRUE(playFramesAction.isActive());
  EXPECT_FALSE(sframe.isLastFrame);

  // finished
  a->next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 12);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 2);
  EXPECT_FALSE(playFramesAction.isActive());
  EXPECT_TRUE(sframe.isLastFrame);
}


TEST(playFramesAction_tests, testBackwardComplete){

  PixelFrame sframe; 
  RawPoiCommand rawCmd0 = {{PLAY_FRAMES, 12, 10, 3, 0, 100}};
  PlayFramesAction playFramesAction;
  ActionOptions options;
  
  EXPECT_FALSE(playFramesAction.isActive());
  
  playFramesAction.init(rawCmd0, &sframe, options);

  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 12);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);

  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 11);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 0);
  EXPECT_FALSE(sframe.isLastFrame);

  for (int i=0; i<3*3-3; i++) {
    playFramesAction.next();
    EXPECT_FALSE(sframe.isLastFrame);
  }
  // last iteration
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 10);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 2);
  EXPECT_TRUE(playFramesAction.isActive());
  EXPECT_FALSE(sframe.isLastFrame);

  // finished
  playFramesAction.next();
  EXPECT_EQ(playFramesAction.__getCurrentFrame(), 10);
  EXPECT_EQ(playFramesAction.__getCurrentLoop(), 2);
  EXPECT_FALSE(playFramesAction.isActive());
  EXPECT_TRUE(sframe.isLastFrame);
}
