/**
 * @file ThreadPool.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 22:11:59
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include "ThreadPool.hpp"

#include "HandleError.hpp"
#include "NaiveThreadPoolImpl.hpp"

namespace ndsl {

ThreadPool::ThreadPool(int minCap, int maxCap, THREAD_POOL_TYPE type) {
  if(type == NAIVE_POOL) {
    pImpl_ = new NaiveThreadPoolImpl(minCap, maxCap);
  } else {
    NOTSUPPORT();
  }
}

ThreadPool::~ThreadPool() {
  delete pImpl_;
}
} //namespace ndsl

