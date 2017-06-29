#include "test.h"
//#include "mock_wd2812.h"
#include "ImageCache.h"

TEST(ImageCache_tests, afterDecl){

  ImageCache icache(N_REGISTERS*N_PIXELS, MUTE);
  
  for (int i=0; i< N_REGISTERS; i++){
	rgbVal* r = icache.getRegister(i);
    for (int j=0; j< N_PIXELS; j++){
    	EXPECT_EQ(r[i].r, 0);
    	EXPECT_EQ(r[i].g, 0);
    	EXPECT_EQ(r[i].b, 0);
    }
  }
}
  

  
