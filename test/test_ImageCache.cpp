#include "test.h"
#include "mock_ws2812.h"

#include "memory/ImageCache.h"

uint8_t tVal(uint8_t frame, uint8_t pixel, uint8_t rgb){
  return (frame*pixel*rgb) % 256;
}

TEST(ImageCache_tests, checkRegistersAfterDecl){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
     N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  
  for (int i=0; i< N_REGISTERS; i++){
	rgbVal* r = iCache.getRegister(i);
    for (int j=0; j< N_PIXELS; j+=10){
    	EXPECT_EQ(r[i].r, 0);
    	EXPECT_EQ(r[i].g, 0);
    	EXPECT_EQ(r[i].b, 0);
    }
  }
}

TEST(ImageCache_tests, checkImageMapAfterDecl){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = iCache.getPixel(i,j);
    	EXPECT_EQ(px.r, 0);
    	EXPECT_EQ(px.g, 0);
    	EXPECT_EQ(px.b, 0);
    }
  }

  iCache.printImageFrame(0, ESP_LOG_DEBUG);
}
    
TEST(ImageCache_tests, fillRegisters){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  rgbVal rgb = makeRGBValue(LILA);
  for (int i=0; i< N_REGISTERS; i++){
    iCache.fillRegister(i, rgb);
  }

  for (int i=0; i< N_REGISTERS; i++){
	rgbVal* r = iCache.getRegister(i);
    for (int j=0; j< N_PIXELS; j+=10){
    	EXPECT_EQ(r[i].r, rgb.r);
    	EXPECT_EQ(r[i].g, rgb.g);
    	EXPECT_EQ(r[i].b, rgb.b);
    }
  }
}

TEST(ImageCache_tests, fillImagemap){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = makeRGBValue(rgbArray);

  iCache.fillImageMap(rgb);

  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = iCache.getPixel(i,j);
    	EXPECT_EQ(px.r, 11);
    	EXPECT_EQ(px.g, 111);
    	EXPECT_EQ(px.b, 222);
    }
  }
}

TEST(ImageCache_tests, setClearImagemap){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      iCache.setPixel(i,j,
        tVal(i,j,1),
        tVal(i,j,2),
        tVal(i,j,3)
        );
    }
  }
 for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = iCache.getPixel(i,j);
    	EXPECT_EQ(px.r, tVal(i,j,1));
    	EXPECT_EQ(px.g, tVal(i,j,2));
    	EXPECT_EQ(px.b, tVal(i,j,3));
    }
  }
  iCache.clearImageMap();
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = iCache.getPixel(i,j);
    	EXPECT_EQ(px.r, 0);
    	EXPECT_EQ(px.g, 0);
    	EXPECT_EQ(px.b, 0);
    }
  }
}


TEST(ImageCache_tests, shiftRegisters){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = makeRGBValue(rgbArray);

  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = iCache.getRegister(i);
    r[0] = rgb;
  }

  // value rgb is shifted with each iteration j
  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = iCache.getRegister(i);
    for (int j=0; j< 4; j++){ // shift cycles
      for (int k=0; k< 5; k++){ // pixels 0 - 4
        // rgbVal val = r[k];
        // printf("%d %d %d: %d %d %d\n", 
        //  i, j, k, val.r, val.g, val.b);
        
        if (k==j && j<3){
    	  	EXPECT_EQ(r[k].r, 11);
    	    EXPECT_EQ(r[k].g, 111);
    	    EXPECT_EQ(r[k].b, 222);
        }
        else {
     	  	EXPECT_EQ(r[k].r, 0);
    	    EXPECT_EQ(r[k].g, 0);
    	    EXPECT_EQ(r[k].b, 0);         
        }
      }
      iCache.shiftRegister(i,3,false);
    }
  }
}
 
TEST(ImageCache_tests, shiftRegistersCyclic){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = makeRGBValue(rgbArray);

  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = iCache.getRegister(i);
    r[0] = rgb;
  }

  // value rgb is shifted with each iteration j
  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = iCache.getRegister(i);
    for (int j=0; j< 5; j++){ // shift cycles
      // iCache.printRegister(i, ESP_LOG_DEBUG);
      for (int k=0; k< 5; k++){ // pixels 0 - 4
        //printf("%d %d %d\n", i, j, k);
        if ((k==j && j<3)||(k==j-3 && j>=3)){ // cyclic
    	  	EXPECT_EQ(r[k].r, 11);
    	    EXPECT_EQ(r[k].g, 111);
    	    EXPECT_EQ(r[k].b, 222);
        }
        else {
     	  	EXPECT_EQ(r[k].r, 0);
    	    EXPECT_EQ(r[k].g, 0);
    	    EXPECT_EQ(r[k].b, 0);         
        }
      }
      iCache.shiftRegister(i,3,true);
    }
  }
}
  
TEST(ImageCache_tests, copyFrameToRegisterToRegister){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);
 
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j++){
      iCache.setPixel(i,j,
        tVal(i,j,1),
        tVal(i,j,2),
        tVal(i,j,3)
        );
    }
  }

  for (int i=0; i< N_REGISTERS; i++){
    uint8_t frame = 11+10*i;
    iCache.copyFrameToRegister(i,frame);
    //iCache.printRegister(i, ESP_LOG_DEBUG);
	  rgbVal* r = iCache.getRegister(i);
    for (int j=0; j< N_PIXELS; j++){
    	EXPECT_EQ(r[j].r, tVal(frame,j,1));
    	EXPECT_EQ(r[j].g, tVal(frame,j,2));
    	EXPECT_EQ(r[j].b, tVal(frame,j,3));
    }
  }

  iCache.copyRegisterToRegister(0,1);
  uint8_t frame = 11;
  //iCache.printRegister(1, ESP_LOG_DEBUG);
  rgbVal* r = iCache.getRegister(1);
  for (int j=0; j< N_PIXELS; j++){
    EXPECT_EQ(r[j].r, tVal(frame,j,1));
    EXPECT_EQ(r[j].g, tVal(frame,j,2));
    EXPECT_EQ(r[j].b, tVal(frame,j,3));
  }

}

TEST(ImageCache_tests, copyFrameToRegisterToRegisterFactor){
  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);
 
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j++){
      iCache.setPixel(i,j,
        tVal(i,j,1),
        tVal(i,j,2),
        tVal(i,j,3)
        );
    }
  }

  for (int i=0; i< N_REGISTERS; i++){
    uint8_t frame = 11+10*i;
    iCache.copyFrameToRegister(i,frame, 0.5);
    //iCache.printRegister(i, ESP_LOG_DEBUG);
	  rgbVal* r = iCache.getRegister(i);
    for (int j=0; j< N_PIXELS; j++){
    	EXPECT_EQ(r[j].r, tVal(frame,j,1)/2);
    	EXPECT_EQ(r[j].g, tVal(frame,j,2)/2);
    	EXPECT_EQ(r[j].b, tVal(frame,j,3)/2);
    }
  }

  iCache.copyRegisterToRegister(0,1);
  uint8_t frame = 11;
  //iCache.printRegister(1, ESP_LOG_DEBUG);
  rgbVal* r = iCache.getRegister(1);
  for (int j=0; j< N_PIXELS; j++){
    EXPECT_EQ(r[j].r, tVal(frame,j,1)/2);
    EXPECT_EQ(r[j].g, tVal(frame,j,2)/2);
    EXPECT_EQ(r[j].b, tVal(frame,j,3)/2);
  }

}

TEST(ImageCache_tests, saveToFlash) {

  ImageCache iCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);

  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = makeRGBValue(rgbArray);

  iCache.fillImageMap(rgb);

  // save to flash
  PoiFlashMemory flash;
  bool ok = flash.saveImage(0, iCache.getRawImageData());
  EXPECT_TRUE(ok);

  // check what was saved
  iCache.clearImageMap();
  ok = flash.loadImage(0, iCache.getRawImageData());
  EXPECT_TRUE(ok);
  
  for (int i=0; i< 2; i++){
    for (int j=0; j< N_PIXELS; j++){
      rgbVal px = iCache.getPixel(i,j);
   	EXPECT_EQ(px.r, 11);
   	EXPECT_EQ(px.g, 111);
       EXPECT_EQ(px.b, 222);
    }
  }
}
