#ifndef test_h
#define test_h

#include "stdio.h"
#include "gtest/gtest.h"

#include "mock_Arduino.h"

// compare with a particular precision
void EXPECT_EQ_FLOATPREC(float a, float b, int8_t prec);

#endif
