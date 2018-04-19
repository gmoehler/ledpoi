#include "test.h"
#include "ledpoi.h"
#include <player/NoAction.h>

TEST(noAction_tests, afterDeclaration){
  NoAction noAction;
  EXPECT_FALSE(noAction.isActive());
}

TEST(noAction_tests, afterInit){
  NoAction noAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 5, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  noAction.init(rawCmd0, &sframe, options);
  // never active
  EXPECT_FALSE(noAction.isActive());
  EXPECT_STREQ(noAction.getActionName(), "No Action");
  noAction.printInfo("pre:");
  
  for (int i=0; i< N_PIXELS; i++){
    EXPECT_EQ(sframe.pixel[i].b, 0);
    EXPECT_EQ(sframe.pixel[i].r, 0);
    EXPECT_EQ(sframe.pixel[i].g, 0);
  }
}

TEST(noAction_tests, afterNext){
  NoAction noAction;
  RawPoiCommand rawCmd0 = {{DISPLAY_IP, 5, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;
  
  noAction.init(rawCmd0, &sframe, options);
  AbstractAction *a = dynamic_cast<AbstractAction*>(&noAction);
  a->next();
  noAction.printState();
  EXPECT_FALSE(a->isActive());
  
  for (int i=0; i< N_PIXELS; i++){
    EXPECT_EQ(sframe.pixel[i].b, 0);
    EXPECT_EQ(sframe.pixel[i].r, 0);
    EXPECT_EQ(sframe.pixel[i].g, 0);
  }
}
