/**
 * @file Event.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 20:55:08
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include "Event.hpp"
#include "HandleError.hpp"

namespace ndsl {

Event::Event(bool reset) : reset_(reset), state_(false) {
  int r(0);
  r = pthread_mutex_init(&mutex_, 0);
  NDSL_ASSERT(r);
  r = pthread_cond_init(&cond_, 0);
  NDSL_ASSERT(r);
}

Event::~Event() {
  int r(0);
  r = pthread_cond_destroy(&cond_);
  NDSL_ASSERT(r);
  r = pthread_mutex_destroy(&mutex_);
  NDSL_ASSERT(r);
}

void Event::wait() {
  //TODO: consider whether need to use  pthread_sigmask to block signals?
  int r(0);

  for (;;) {
    r = pthread_mutex_lock(&mutex_);
    NDSL_ASSERT(r);

    while (!state_) {
      r = pthread_cond_wait(&cond_, &mutex_);
      NDSL_ASSERT(r);
    }

    if (reset_) {
      state_ = false;
    }

    r = pthread_mutex_unlock(&mutex_);
    NDSL_ASSERT(r);
  }
}

void Event::set() {
  int r(0);

  r = pthread_mutex_lock(&mutex_);
  state_ = true;
  NDSL_ASSERT(r);

  r = pthread_mutex_unlock(&mutex_);
  NDSL_ASSERT(r);

  r = pthread_cond_broadcast(&cond_);
  NDSL_ASSERT(r);
}

}//namespace ndsl

