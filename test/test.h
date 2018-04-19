#ifndef TEST_H
#define TEST_H

#include "stdio.h"
#include "gtest/gtest.h"

#include "mock_Arduino.h"
#include "PoiCommand.h"
#include "ledpoi_utils.h"

// just to make errors on unit functions disappear
#undef WITHIN_UNITTEST
#define WITHIN_UNITTEST

// set log level high to prevent output
// does not work yet...
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_ERROR

// registers are currently not used in code, but we test them anyway
#define N_REGISTERS 2

// compare with a particular precision
void EXPECT_EQ_FLOATPREC(float a, float b, int8_t prec);

#define EXPECT_EQ_COLOR(rgbVal1, rgbVal2) \
    {EXPECT_EQ(rgbVal1.r, rgbVal2.r);} \
    {EXPECT_EQ(rgbVal1.g, rgbVal2.g);} \
    {EXPECT_EQ(rgbVal1.b, rgbVal2.b);}

#endif
