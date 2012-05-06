/**
 * @file Thread.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Sun, 06 May 2012 15:17:24
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include "Thread.hpp"

#include "HandleError.hpp"
#include "Task.hpp"
#include "Mutex.hpp"
#include "CriticalRegion.hpp"

namespace ndsl {

Thread::Thread() : task_(0) , kill_(false) {

  int err = pthread_create(&thread_, NULL, run, this);
  NDSL_ASSERT(err);
  err = pthread_detach(thread_);
  NDSL_ASSERT(err);
}

Thread::~Thread() {

}

void* Thread::run(void* arg) {
  Thread* pThread = reinterpret_cast<Thread*>(arg);

  for (; !pThread->kill_;) {
    pThread->ready_.wait();
    pThread->task_->run();
    pThread->task_ = 0;
  }

  return 0;
}

void Thread::start(Task& task) {

  {
    CriticalRegion  region(taskLock_);

    if (task_ != 0) {
      NDSL_FAIL();
    }
    task_ = &task;
  }
  ready_.set();
}


} //namespace ndsl
