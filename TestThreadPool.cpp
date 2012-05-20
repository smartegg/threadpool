/**
 * @file TestThreadPool.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Mon, 07 May 2012 12:41:37
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include <gtest/gtest.h>
#include <unistd.h>
#include "ThreadPool.hpp"
#include "Mutex.hpp"
#include "CriticalRegion.hpp"

using namespace ndsl;

namespace {

volatile int f = 0;
ndsl::Mutex fMutex;

class MyTask : public ndsl::Task {
  public:
    virtual void run() {
      CriticalRegion guard(fMutex);
      f++;
      usleep(1000 * 10);//suspend 10ms to allow other thread to get more tasks.
      printf("%d\n", ::f);
    }
};
}

TEST(TestThreadPool, runSimpleTask) {
  ThreadPool *pool = new ThreadPool(2,16);

  pool->start();
  pool->allocateThreads(8);

  for (size_t i = 0; i < 100; ++i) {
    Task* task = new MyTask();
    pool->addTask(*task);
  }
  //block and stop until all task is done, just called  you want to delete this threadpool
  pool->syncStop();
  delete pool;
  EXPECT_EQ(100, ::f);

}


TEST(TestThreadPool,  InitalStatus) {
  ThreadPool pool(2, 16);
  EXPECT_EQ(0, pool.numRunningThreads());

  pool.start();

  EXPECT_EQ(2, pool.numIdleThreads());
  EXPECT_EQ(0, pool.numBusyThreads());
  EXPECT_EQ(2, pool.numRunningThreads());

  pool.stop();

}

TEST(TestThreadPool, allocateMaxThreads) {
  ThreadPool pool(2, 16);
  pool.start();

  EXPECT_EQ(10, pool.allocateThreads(10));
  EXPECT_EQ(4, pool.allocateThreads(10));

  pool.stop();
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
