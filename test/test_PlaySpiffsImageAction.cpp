#include "test.h"
#include "player/PlaySpiffsImageAction.h"

void setFrameDataNoCoding(int morePixels=0) {
  uint8_t headerVals = 3;
  uint8_t nFrames = 3;
  uint16_t nPixels = nFrames * N_PIXELS/2 + morePixels;
  uint16_t nValues = headerVals + 3 * nPixels;

  printf("Generating no coding data: %d x %d: %d pix %d vals\n", nFrames, N_PIXELS/2, nPixels, nValues);

  uint8_t* data = new uint8_t[nValues];
  data[0]=NO_CODEC; // no encoding
  data[1]=N_PIXELS/2; // height
  data[2]=nFrames; // width

  
  for (int p=0; p<nPixels; p++){
    for (int c=0; c<3; c++){
      uint16_t idx = headerVals + p*3 + c;
      data[idx] = p+c;
    }
  }
  setFileData(data, nValues);
}

void setFrameDataRuntimeCoding(int morePixels=0) {
  uint8_t headerVals = 3;
  uint8_t nFrames = 3;
  uint16_t nPixels = nFrames+2;
  uint16_t nValues = headerVals + 4 * nPixels;

  printf("Generating runtime coding data: %d x %d: %d pix %d vals\n", nFrames, N_PIXELS/2, nPixels, nValues);

  uint8_t* data = new uint8_t[nValues];
  uint16_t idx=0;
  data[idx++]=RUNTIME_CODEC; // no encoding
  data[idx++]=N_PIXELS/2; // height
  data[idx++]=nFrames; // width

  // frame 0, pixel 0
  data[idx++] = 10;
  data[idx++] = 11;
  data[idx++] = 12;
  data[idx++] = 1;

  // frame 0, all other pixels but last
  data[idx++] = 20;
  data[idx++] = 21;
  data[idx++] = 22;
  data[idx++] = N_PIXELS/2-2;

  // frame 0, last pixel
  data[idx++] = 30;
  data[idx++] = 31;
  data[idx++] = 32;
  data[idx++] = 1;

  // all other frames: same pixel everywhere
  for (int f=1; f<nFrames; f++){
    data[idx++] = 100;
    data[idx++] = 101;
    data[idx++] = 102;
    data[idx++] = N_PIXELS/2;
  }

  setFileData(data, nValues);
}

void checkFramePixel (PixelFrame *sframe, uint8_t idx, 
  uint8_t r, uint8_t g, uint8_t b){
  rgbVal val = sframe->pixel[idx];
  // printf("-> %d %d %d\n",val.r, val.g, val.b);
  EXPECT_EQ(val.r, r);
  EXPECT_EQ(val.g, g);
  EXPECT_EQ(val.b, b);
}

TEST(playSpiffsImageAction_tests, afterDeclaration){
  PlaySpiffsImageAction playSpiffsImageAction;
  playSpiffsImageAction.printInfo("pre:");
  EXPECT_FALSE(playSpiffsImageAction.isActive());
}

TEST(playSpiffsImageAction_tests, afterInit){
  setFrameDataNoCoding();

  PlaySpiffsImageAction playSpiffsImageAction;
  RawPoiCommand rawCmd0 = {{PLAY_SPIFFS_IMAGE, 0, 0, 0, 0, 0}};
  PixelFrame sframe;
  ActionOptions options;

  playSpiffsImageAction.init(rawCmd0, &sframe, options);
  EXPECT_TRUE(playSpiffsImageAction.isActive( ));
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 0);
  EXPECT_EQ(playSpiffsImageAction.__getDelay(), 10);  //default delay
}

TEST(playSpiffsImageAction_tests, wrongAction){
  setFrameDataNoCoding();

  PlaySpiffsImageAction playSpiffsImageAction;
  RawPoiCommand rawCmd0 = {{ANIMATE, 10, 50, 3, 0, 100}};
  PixelFrame sframe;
  ActionOptions options;

  playSpiffsImageAction.init(rawCmd0, &sframe, options);
}

TEST(playSpiffsImageAction_tests, testNext){
  setFrameDataNoCoding();

  PlaySpiffsImageAction playSpiffsImageAction;
  RawPoiCommand rawCmd0 = {{PLAY_SPIFFS_IMAGE, 0, 0, 0, 10, 1}};
  PixelFrame sframe;
  ActionOptions options;
  
  playSpiffsImageAction.init(rawCmd0, &sframe, options);
  EXPECT_EQ(playSpiffsImageAction.__getDelay(), 2561);
  
  EXPECT_FALSE(sframe.isLastFrame);
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 0);

  playSpiffsImageAction.next();
  playSpiffsImageAction.printState();
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 1);

  playSpiffsImageAction.next();
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 2);
  EXPECT_FALSE(sframe.isLastFrame);
  
  checkFramePixel(&sframe, N_PIXELS/2,  3*N_PIXELS/2-1,  3*N_PIXELS/2,  3*N_PIXELS/2+1 );

  playSpiffsImageAction.next();
  EXPECT_FALSE(playSpiffsImageAction.isActive());
  EXPECT_TRUE(sframe.isLastFrame);

  // should still be the same since we exactly have 90 pixels
  checkFramePixel(&sframe, N_PIXELS/2,  3*N_PIXELS/2-1,  3*N_PIXELS/2,  3*N_PIXELS/2+1 );
}

TEST(playSpiffsImageAction_tests, testNextMissingPixels){
  setFrameDataNoCoding(-10);

  PlaySpiffsImageAction playSpiffsImageAction;
  RawPoiCommand rawCmd0 = {{PLAY_SPIFFS_IMAGE, 0, 0, 0, 10, 1}};
  PixelFrame sframe;
  ActionOptions options;
  
  playSpiffsImageAction.init(rawCmd0, &sframe, options);
  EXPECT_EQ(playSpiffsImageAction.__getDelay(), 2561);
  
  EXPECT_FALSE(sframe.isLastFrame);
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 0);

  playSpiffsImageAction.next();
  playSpiffsImageAction.printState();
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 1);

  checkFramePixel(&sframe, N_PIXELS/2,  2*N_PIXELS/2-1,  2*N_PIXELS/2,  2*N_PIXELS/2+1 );

  playSpiffsImageAction.next();
  EXPECT_FALSE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 1);
  EXPECT_TRUE(sframe.isLastFrame);

  checkFramePixel(&sframe, 0,  2*N_PIXELS/2,  2*N_PIXELS/2+1,  2*N_PIXELS/2+2 );
  checkFramePixel(&sframe, N_PIXELS/2,  0, 0, 0);

  playSpiffsImageAction.next();
  EXPECT_FALSE(playSpiffsImageAction.isActive());
  EXPECT_TRUE(sframe.isLastFrame);
}

TEST(playSpiffsImageAction_tests, testNextRuntimeCoding){
  setFrameDataRuntimeCoding();

  PlaySpiffsImageAction playSpiffsImageAction;
  RawPoiCommand rawCmd0 = {{PLAY_SPIFFS_IMAGE, 0, 0, 0, 10, 1}};
  PixelFrame sframe;
  ActionOptions options;
  
  playSpiffsImageAction.init(rawCmd0, &sframe, options);
  EXPECT_EQ(playSpiffsImageAction.__getDelay(), 2561);
  
  EXPECT_FALSE(sframe.isLastFrame);
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 0);

  checkFramePixel(&sframe, 0,  10, 11, 12 );
  for (int i=1; i< N_PIXELS/2-1; i++) {
    checkFramePixel(&sframe, i,  20, 21, 22 );
  }
  checkFramePixel(&sframe, N_PIXELS/2,  30, 31, 32 );

  playSpiffsImageAction.next();
  playSpiffsImageAction.printState();
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 1);

  checkFramePixel(&sframe, 1,  100, 101, 102 );
  checkFramePixel(&sframe, N_PIXELS/2,  100, 101, 102 );

  playSpiffsImageAction.next();
  EXPECT_TRUE(playSpiffsImageAction.isActive());
  EXPECT_EQ(playSpiffsImageAction.__getCurrentFrame(), 2);
  EXPECT_FALSE(sframe.isLastFrame);

  checkFramePixel(&sframe, 1,  100, 101, 102 );
  checkFramePixel(&sframe, N_PIXELS/2,  100, 101, 102 );

  playSpiffsImageAction.next();
  EXPECT_FALSE(playSpiffsImageAction.isActive());
  EXPECT_TRUE(sframe.isLastFrame);
}
