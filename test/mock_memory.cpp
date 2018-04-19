#include "test.h"
#include "memory/ImageCache.h"
#include "memory/ProgramCache.h"

// image cache can hold 3 images of size N_PIXELS * N_FRAMES
ImageCache imageCache(N_PIXELS, N_FRAMES, 
  N_PIXELS * N_FRAMES * 3, N_REGISTERS);
ProgramCache programCache;