#include "test.h"
#include "mock_ws2812.h"
#include "ImageCache.h"

uint8_t tVal(uint8_t frame, uint8_t pixel, uint8_t rgb){
  return (frame*pixel*rgb) % 256;
}


TEST(ImageCache_tests, checkRegistersAfterDecl){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
  
  for (int i=0; i< N_REGISTERS; i++){
	rgbVal* r = icache.getRegister(i);
    for (int j=0; j< N_PIXELS; j+=10){
    	EXPECT_EQ(r[i].r, 0);
    	EXPECT_EQ(r[i].g, 0);
    	EXPECT_EQ(r[i].b, 0);
    }
  }
}

TEST(ImageCache_tests, checkImageMapAfterDecl){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
  
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = icache._getPixel(i,j);
    	EXPECT_EQ(px.r, 0);
    	EXPECT_EQ(px.g, 0);
    	EXPECT_EQ(px.b, 0);
    }
  }
}
    
TEST(ImageCache_tests, fillRegisters){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
  rgbVal rgb = icache._makeRGBValue(LILA);
  for (int i=0; i< N_REGISTERS; i++){
    icache._fillRegister(i, rgb);
  }

  for (int i=0; i< N_REGISTERS; i++){
	rgbVal* r = icache.getRegister(i);
    for (int j=0; j< N_PIXELS; j+=10){
    	EXPECT_EQ(r[i].r, rgb.r);
    	EXPECT_EQ(r[i].g, rgb.g);
    	EXPECT_EQ(r[i].b, rgb.b);
    }
  }
}

TEST(ImageCache_tests, fillImagemap){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = icache._makeRGBValue(rgbArray);

  icache._fillImageMap(rgb);

  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = icache._getPixel(i,j);
    	EXPECT_EQ(px.r, 11);
    	EXPECT_EQ(px.g, 111);
    	EXPECT_EQ(px.b, 222);
    }
  }
}

TEST(ImageCache_tests, setClearImagemap){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
 
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      icache._setPixel(i,j,
        tVal(i,j,1),
        tVal(i,j,2),
        tVal(i,j,3)
        );
    }
  }
 for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = icache._getPixel(i,j);
    	EXPECT_EQ(px.r, tVal(i,j,1));
    	EXPECT_EQ(px.g, tVal(i,j,2));
    	EXPECT_EQ(px.b, tVal(i,j,3));
    }
  }
  icache.clearImageMap();
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j+=10){
      rgbVal px = icache._getPixel(i,j);
    	EXPECT_EQ(px.r, 0);
    	EXPECT_EQ(px.g, 0);
    	EXPECT_EQ(px.b, 0);
    }
  }
}


TEST(ImageCache_tests, shiftRegisters){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = icache._makeRGBValue(rgbArray);

  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = icache.getRegister(i);
    r[0] = rgb;
  }

  // value rgb is shifted with each iteration j
  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = icache.getRegister(i);
    for (int j=0; j< 4; j++){ // shift cycles
      //icache.printRegister(i);
      for (int k=0; k< 5; k++){ // pixels 0 - 4
        //printf("%d %d %d\n", i, j, k);
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
      icache._shiftRegister(i,3,false);
    }
  }
}
  
TEST(ImageCache_tests, shiftRegistersCyclic){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
  uint8_t rgbArray[3] = {11,111,222};
  rgbVal rgb = icache._makeRGBValue(rgbArray);

  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = icache.getRegister(i);
    r[0] = rgb;
  }

  // value rgb is shifted with each iteration j
  for (int i=0; i< N_REGISTERS; i++){
    rgbVal* r = icache.getRegister(i);
    for (int j=0; j< 5; j++){ // shift cycles
      //icache.printRegister(i);
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
      icache._shiftRegister(i,3,true);
    }
  }
}
  
  
TEST(ImageCache_tests, copyFrameToRegisterToRegister){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
 
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j++){
      icache._setPixel(i,j,
        tVal(i,j,1),
        tVal(i,j,2),
        tVal(i,j,3)
        );
    }
  }

  for (int i=0; i< N_REGISTERS; i++){
    uint8_t frame = 11+10*i;
    icache._copyFrameToRegister(i,frame);
    //icache.printRegister(i);
	  rgbVal* r = icache.getRegister(i);
    for (int j=0; j< N_PIXELS; j++){
    	EXPECT_EQ(r[j].r, tVal(frame,j,1));
    	EXPECT_EQ(r[j].g, tVal(frame,j,2));
    	EXPECT_EQ(r[j].b, tVal(frame,j,3));
    }
  }

  icache._copyRegisterToRegister(0,1);
  uint8_t frame = 11;
  //icache.printRegister(1);
  rgbVal* r = icache.getRegister(1);
  for (int j=0; j< N_PIXELS; j++){
    EXPECT_EQ(r[j].r, tVal(frame,j,1));
    EXPECT_EQ(r[j].g, tVal(frame,j,2));
    EXPECT_EQ(r[j].b, tVal(frame,j,3));
  }

}

TEST(ImageCache_tests, copyFrameToRegisterToRegisterFactor){

  ImageCache icache(3*N_FRAMES*N_PIXELS, MUTE);
 
  for (int i=0; i< N_FRAMES; i++){
    for (int j=0; j< N_PIXELS; j++){
      icache._setPixel(i,j,
        tVal(i,j,1),
        tVal(i,j,2),
        tVal(i,j,3)
        );
    }
  }

  for (int i=0; i< N_REGISTERS; i++){
    uint8_t frame = 11+10*i;
    icache._copyFrameToRegister(i,frame, 0.5);
    //icache.printRegister(i);
	  rgbVal* r = icache.getRegister(i);
    for (int j=0; j< N_PIXELS; j++){
    	EXPECT_EQ(r[j].r, tVal(frame,j,1)/2);
    	EXPECT_EQ(r[j].g, tVal(frame,j,2)/2);
    	EXPECT_EQ(r[j].b, tVal(frame,j,3)/2);
    }
  }

  icache._copyRegisterToRegister(0,1);
  uint8_t frame = 11;
  //icache.printRegister(1);
  rgbVal* r = icache.getRegister(1);
  for (int j=0; j< N_PIXELS; j++){
    EXPECT_EQ(r[j].r, tVal(frame,j,1)/2);
    EXPECT_EQ(r[j].g, tVal(frame,j,2)/2);
    EXPECT_EQ(r[j].b, tVal(frame,j,3)/2);
  }

}
