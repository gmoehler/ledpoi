#include "ledpoi.h"

rgbVal makeRGBValue(uint8_t *rgb_array){
  return makeRGBVal(rgb_array[0], rgb_array[1], rgb_array[2]);
}
rgbVal makeRGBValue(Color color, uint8_t brightness){
  rgbVal rgb = makeRGBVal(0,0,0);
  uint8_t b = brightness;
  switch (color){
  	case WHITE:
      rgb = makeRGBVal(b,b,b);
      break;

  	case BLACK:
      rgb = makeRGBVal(0,0,0);
      break;

      case RED:
      rgb = makeRGBVal(b,0,0);
      break;

      case GREEN:
      rgb = makeRGBVal(0,b,0);
      break;

      case BLUE:
      rgb = makeRGBVal(0,0,b);
      break;

      case YELLOW:
      rgb = makeRGBVal(b,b,0);
      break;

      case LILA:
      rgb = makeRGBVal(b,0,b);
      break;

      case CYAN:
      rgb = makeRGBVal(0,b,b);
      break;

      case PALE_WHITE:
      rgb = makeRGBVal(8,8,8);
      break;

      default:
      //RAINBOW is not handled here -> black
      break;
      }
      return rgb;
}

