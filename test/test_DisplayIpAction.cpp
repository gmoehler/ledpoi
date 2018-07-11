#include "test.h"
#include "ledpoi.h"
#include <player/DisplayIpAction.h>

TEST(displayIpAction_tests, afterDeclaration){
  DisplayIpAction displayIpAction;
  displayIpAction.printInfo("pre:");
  EXPECT_FALSE(displayIpAction.isActive());
}

TEST(displayIpAction_tests, afterInitWithOddIp){
  DisplayIpAction displayIpAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 5, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  displayIpAction.init(rawCmd0, &sframe, options);
  
  EXPECT_TRUE(displayIpAction.isActive());
  EXPECT_STREQ(displayIpAction.getActionName(),"Display IP Action");
  
  EXPECT_TRUE(sframe.isLastFrame);
  for (int i=0; i< N_PIXELS; i++){
    if (i==5){
      EXPECT_EQ(sframe.pixel[i].b, 64);
    }
    else {
      EXPECT_EQ(sframe.pixel[i].b, 0);
    }
    EXPECT_EQ(sframe.pixel[i].r, 0);
    EXPECT_EQ(sframe.pixel[i].g, 0);
  }
}

TEST(displayIpAction_tests, afterInitWithEvenIp){
  DisplayIpAction displayIpAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 6, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  displayIpAction.init(rawCmd0, &sframe, options);
  
  EXPECT_TRUE(displayIpAction.isActive());
  EXPECT_STREQ(displayIpAction.getActionName(),"Display IP Action");
  
  for (int i=0; i< N_PIXELS; i++){
    if (i==6){
      EXPECT_EQ(sframe.pixel[i].r, 8);
      EXPECT_EQ(sframe.pixel[i].g, 8);
    }
    else {
      EXPECT_EQ(sframe.pixel[i].r, 0);
      EXPECT_EQ(sframe.pixel[i].g, 0);
    }
    EXPECT_EQ(sframe.pixel[i].b, 0);
  }
}


TEST(displayIpAction_tests, afterInitWithBackground){
  DisplayIpAction displayIpAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 5, 1, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  displayIpAction.init(rawCmd0, &sframe, options);
  EXPECT_TRUE(displayIpAction.isActive());

  for (int i=0; i< N_PIXELS; i++){
    if (i==5){
      //pale green
      EXPECT_EQ(sframe.pixel[i].r, 0);
      EXPECT_EQ(sframe.pixel[i].g, 0);
      EXPECT_EQ(sframe.pixel[i].b, 64);
    }
    else if (i<N_POIS) {
      // palewhite background
      EXPECT_EQ(sframe.pixel[i].r, 8);
      EXPECT_EQ(sframe.pixel[i].g, 8);
      EXPECT_EQ(sframe.pixel[i].b, 8);
    }
    else {
      // black
      EXPECT_EQ(sframe.pixel[i].r, 0);
      EXPECT_EQ(sframe.pixel[i].g, 0);
      EXPECT_EQ(sframe.pixel[i].b, 0);
    }
  }
}

TEST(displayIpAction_tests, afterNext){
  DisplayIpAction displayIpAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 5, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  displayIpAction.init(rawCmd0, &sframe, options);
  AbstractAction *a = dynamic_cast<AbstractAction*>(&displayIpAction);
  a->next();
  displayIpAction.printState();
  // static: only one frame
  EXPECT_FALSE(a->isActive());
  
  for (int i=0; i< N_PIXELS; i++){
    if (i==5){
      EXPECT_EQ(sframe.pixel[i].b, 64);
    }
    else {
      EXPECT_EQ(sframe.pixel[i].b, 0);
    }
    EXPECT_EQ(sframe.pixel[i].r, 0);
    EXPECT_EQ(sframe.pixel[i].g, 0);
  }
}

TEST(displayIpAction_tests, highIp){
  DisplayIpAction displayIpAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 254, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  displayIpAction.init(rawCmd0, &sframe, options);
  AbstractAction *a = dynamic_cast<AbstractAction*>(&displayIpAction);
  a->next();
  displayIpAction.printState();
  // static: only one frame
  EXPECT_FALSE(a->isActive());
  // for high ip incr all is black
  for (int i=0; i< N_PIXELS; i++){
    EXPECT_EQ(sframe.pixel[i].b, 0);
    EXPECT_EQ(sframe.pixel[i].r, 0);
    EXPECT_EQ(sframe.pixel[i].g, 0);
  }
}
