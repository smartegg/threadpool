/**
 * @file Mutex.cpp
 * @brief
 * @author biyu<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 19:49:23
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include "Mutex.hpp"

#include "HandleError.hpp"

namespace ndsl {

Mutex::Mutex() {
  int r = pthread_mutex_init(&mutex_, 0);
  NDSL_ASSERT(r);
}

Mutex::~Mutex() {
  int r = pthread_mutex_destroy(&mutex_);
  NDSL_ASSERT(r);
}

void Mutex::lock() {
  int r = pthread_mutex_lock(&mutex_);
  NDSL_ASSERT(r);
}

void Mutex::unlock() {
  int r = pthread_mutex_unlock(&mutex_);
  NDSL_ASSERT(r);
}

}//namespace ndsl
