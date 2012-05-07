/**
 * @file TestThreadPool.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Mon, 07 May 2012 12:41:37
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include <gtest/gtest.h>
#include "ThreadPool.hpp"

using namespace ndsl;

namespace {

class MyTask : public ndsl::Task {
  public:
    virtual void run() {
      //do nothing
    }
};
}


TEST(TestThreadPool,  InitalStatus) {
  ThreadPool pool(2, 16);
  EXPECT_EQ(2, pool.numIdleThreads());
  EXPECT_EQ(0, pool.numBusyThreads());
  EXPECT_EQ(2, pool.numRunningThreads());

}

TEST(TestThreadPool, allocateThreads) {
  ThreadPool pool(2, 16);
  EXPECT_EQ(10, pool.allocateThreads(10));
  EXPECT_EQ(4, pool.allocateThreads(10));
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
