#include "test.h"
#include "ledpoi.h"
#include <player/ShowRgbAction.h>

TEST(showRgbAction_tests, afterDeclaration){
  ShowRgbAction showRgbAction;
  showRgbAction.printInfo("pre:");
  EXPECT_FALSE(showRgbAction.isActive());
}

TEST(showRgbAction_tests, afterInit){
  ShowRgbAction showRgbAction;
  RawPoiCommand rawCmd0 = {{SHOW_RGB, 32, 64, 128, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  showRgbAction.init(rawCmd0, &sframe, options);
  
  EXPECT_TRUE(showRgbAction.isActive());
  EXPECT_STREQ(showRgbAction.getActionName(),"Show RGB Action");
  
  EXPECT_TRUE(sframe.isLastFrame);
  for (int i=0; i< N_PIXELS; i++){
    EXPECT_EQ(sframe.pixel[i].r, 32);
    EXPECT_EQ(sframe.pixel[i].g, 64);
    EXPECT_EQ(sframe.pixel[i].b, 128);
  }
}


TEST(showRgbAction_tests, afterNext){
  ShowRgbAction showRgbAction;
  RawPoiCommand rawCmd0 = {{SHOW_RGB, 32, 64, 128, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  showRgbAction.init(rawCmd0, &sframe, options);
  AbstractAction *a = dynamic_cast<AbstractAction*>(&showRgbAction);
  a->next();
  showRgbAction.printState();
  // static: only one frame
  EXPECT_FALSE(a->isActive());
  
  for (int i=0; i< N_PIXELS; i++){
    EXPECT_EQ(sframe.pixel[i].r, 32);
    EXPECT_EQ(sframe.pixel[i].g, 64);
    EXPECT_EQ(sframe.pixel[i].b, 128);
  }
}
