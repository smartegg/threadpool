/**
 * @file TestEvent.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Sun, 20 May 2012 17:22:21
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include <gtest/gtest.h>
#include "Event.hpp"


using namespace ndsl;

TEST(TestEvent, simpleTest) {
   Event ev;
   ev.set();
   ev.wait();
}
