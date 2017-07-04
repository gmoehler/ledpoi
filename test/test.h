#ifndef test_h
#define test_h

#include "stdio.h"
#include "gtest/gtest.h"

#include "mock_Arduino.h"

// compare with a particular precision
void EXPECT_EQ_FLOATPREC(float a, float b, int8_t prec);

#define EXPECT_EQ_COLOR(rgbVal1, rgbVal2) \
    {EXPECT_EQ(rgbVal1.r, rgbVal2.r);} \
    {EXPECT_EQ(rgbVal1.g, rgbVal2.g);} \
    {EXPECT_EQ(rgbVal1.b, rgbVal2.b);}

#endif
