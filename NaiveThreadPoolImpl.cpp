/**
 * @file NaiveThreadPoolImpl.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Fri, 04 May 2012 14:45:14
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include "NaiveThreadPoolImpl.hpp"

#include <algorithm>
#include <cassert>
#include <pthread.h>
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "CriticalRegion.hpp"
#include "HandleError.hpp"



namespace ndsl {

/**
 * @brief NaiveThread
 *  the thread which the NaiveThreadpool manage
 */
class NaiveThread {
  public:
    /**
     * @brief NaiveThread
     *    when  create, this thread just  run, and wait for  task.
     */
    NaiveThread();
    virtual ~NaiveThread();

    /**
    * @brief receiveTask
    *
    * @param task the task need to run
    *
    * @returns whether successfully push the task,
    *            failed becase the thread is busy running.
    */
    bool receiveTask(Task& task);
    /**
     * @brief isIdle
     *
     * @returns   true if idle, false on busy.
     */
    bool isIdle() const;

    /**
     * @brief kill
     *   notify  this  thread to kill it self, stop its running
     */
    void kill();


  private:
    Thread thread_;

    NaiveThread(const NaiveThread&);
    const NaiveThread& operator=(const NaiveThread&);
};

NaiveThread::NaiveThread() {

}


NaiveThread::~NaiveThread() {

}

bool NaiveThread::receiveTask(Task& task) {
  if (thread_.isIdle()) {
    return false;

  } else {
    thread_.start(task);
    return true;
  }
}

bool NaiveThread::isIdle() const {
  return thread_.isIdle();
}

void NaiveThread::kill() {
  //TODO: think if we need to join the thread to die ?
  //       we just notify to kill by it self.
  thread_.kill();
}


NaiveThreadPoolImpl::NaiveThreadPoolImpl(int minCap, int maxCap)
  : ThreadPoolImpl(minCap, maxCap),
    minCap_(minCap),
    maxCap_(maxCap),
    stop_(false) {
  for (size_t i = 0; i < minCap; ++i) {
    threads_.push_back(new NaiveThread());
  }
  pthread_t tid;
  int err = pthread_create(&tid, 0, start, this);
  NDSL_ASSERT(err);
}

NaiveThreadPoolImpl::~NaiveThreadPoolImpl() {
  typedef std::list<NaiveThread*>::iterator Iter;

  for(Iter iter= threads_.begin(); iter != threads_.end(); ++iter) {
    delete *iter;
  }
}

int NaiveThreadPoolImpl::allocateThreads(size_t num) {
  int maxnum = maxCap_ - threads_.size();
  maxnum = std::min(maxnum, (int)num);

  for (size_t i = 0; i < maxnum; ++i) {
    threads_.push_back(new NaiveThread());
  }


  return maxnum;
}

int NaiveThreadPoolImpl::numBusyThreads() const {
  int numbusy = 0;

  for (std::list<NaiveThread*>::const_iterator it(threads_.begin());
       it != threads_.end();
       ++it) {
    if (!(*it)->isIdle()) {
      ++numbusy;
    }
  }

  return numbusy;
}

int NaiveThreadPoolImpl::numIdleThreads() const {
  return threads_.size() - numBusyThreads();
}

int NaiveThreadPoolImpl::numRunningThreads() const {
  return threads_.size();
}

void NaiveThreadPoolImpl::addTask(Task& task) {
  {
    CriticalRegion  region(tasksLock_);
    tasks_.push_back(&task);
  }
  receiveTask_.set();
}

void* NaiveThreadPoolImpl::start(void* arg) {
  typedef std::list<NaiveThread*>::iterator Iter;
  NaiveThreadPoolImpl* pool = reinterpret_cast<NaiveThreadPoolImpl*> (arg);

  for (; !pool->stop_;) {
    if (pool->tasks_.size() == 0) {
      //TODO: think this method is right or not ?
      pool->receiveTask_.wait();
    }

    for (Iter it = pool->threads_.begin(); it != pool->threads_.end(); ++it) {

      if (pool->tasks_.size() == 0) {
        break;
      }

      if ((*it)->isIdle()) {
        Task* task;
        {
          CriticalRegion  region(pool->tasksLock_);
          task =pool->tasks_.front();
          pool->tasks_.pop_front();
        }

        (*it)->receiveTask(*task);
      }
    }
  }
}

int NaiveThreadPoolImpl::killIdleThreads(size_t num) {
  typedef std::list<NaiveThread*>::iterator Iter;
  int numDeleted = 0;

  if (num = 0) {
    num = threads_.size();
  }

  for (Iter iter = threads_.begin();
       iter != threads_.end() && numDeleted < num;)  {
    if ((*iter)->isIdle()) {
      (*iter)->kill();
      ++numDeleted;
      iter = threads_.erase(iter);

    } else {
      ++iter;
    }
  }

  return numDeleted;
}



} //namespace ndsl
