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
#include "Task.hpp"



namespace ndsl {

/**
 * @brief NaiveThread
 *  the worker thread managed by  the NaiveThreadpool
 */
class NaiveThread : public Task {
  public:
    /**
     * @brief NaiveThread
     *    when  create, this thread just  run, and wait for  task.
     */
    NaiveThread();
    virtual ~NaiveThread();

    /**
    * @brief getTask
    *
    * @param task the task need to run
    *
    * @returns whether successfully push the task,
    *            failed becase the thread is busy running.
    */
    bool getTask(Task& task);
    /**
     * @brief isIdle
     *
     * @returns   true if idle, false on busy.
     */
    bool isIdle() const;

    /**
     * @brief kill
     *   asyn notify  this  thread to kill it self
     */
    void kill();

    /**
     * @brief syncKill
     *  notify this thread to kill it self , just wait until
     *  successfully  killed
     */
    void syncKill();

    void start();

    /**
     * @brief stat
     * show some useful info about this thread.
     */
    void stat();


  private:
    void run();

    Thread thread_;
    //FIXME: volatile is enough, or use a atomic_t type instead?
    volatile bool kill_;
    Task* volatile task_;
    int invoked_;

    Event ready_;
    Event getTask_;

    NaiveThread(const NaiveThread&);
    const NaiveThread& operator=(const NaiveThread&);
};

NaiveThread::NaiveThread() : kill_(false), task_(0) , getTask_(true) ,
  invoked_(0) {

}

void NaiveThread::syncKill() {
  kill();
  thread_.join();
}

NaiveThread::~NaiveThread() {
}

void NaiveThread::stat() {
  printf("thread: %u\n  got %d tasks\n", (unsigned int)pthread_self(), invoked_);
}

bool NaiveThread::getTask(Task& task) {
  //only the master thread call this method, so need not consider thread-safe.

  if (task_ != 0) {
    return false;
  }

  invoked_++;
  task_ = &task;
  getTask_.set();
  return true;
}

bool NaiveThread::isIdle() const {
  return task_ == 0;
}

void NaiveThread::kill() {
  kill_ = true;
  getTask_.set(); // wake up itself if it is in waiting/blocking status.
}

void NaiveThread::start() {
  thread_.start(*this);
  ready_.wait();
}

void NaiveThread::run() {
  ready_.set();

  for (; !kill_;) {
    getTask_.wait();

    if (task_ == 0) {// no task, but wakeup , means maybe we want kill it.
      continue;
    }

    stat();

    task_->run();
    task_ = 0;
  }
}


NaiveThreadPoolImpl::NaiveThreadPoolImpl(int minCap, int maxCap)
  : ThreadPoolImpl(minCap, maxCap),
    minCap_(minCap),
    maxCap_(maxCap),
    stop_(false) ,
    receiveTask_(true) {

}


void NaiveThreadPoolImpl::start() {
  for (size_t i = 0; i < minCap_; ++i) {
    NaiveThread* pThread = new NaiveThread();
    threads_.push_back(pThread);
    pThread->start();
  }

  masterThread_.start(*this);
  ready_.wait();
}

NaiveThreadPoolImpl::~NaiveThreadPoolImpl() {

}

void NaiveThreadPoolImpl::syncStop() {


  stop();
  stopReady_.wait();

  {

    typedef std::list<NaiveThread*>::iterator Iter;
    CriticalRegion  threadsGuard(threadsLock_);
    for (Iter iter = threads_.begin(); iter != threads_.end(); ++iter) {
      (*iter)->syncKill();
      delete *iter;
    }
  }

  masterThread_.join();
}

int NaiveThreadPoolImpl::allocateThreads(size_t num) {
  CriticalRegion  threadsGuard(threadsLock_);
  int maxnum = maxCap_ - threads_.size();
  maxnum = std::min(maxnum, (int)num);

  for (size_t i = 0; i < maxnum; ++i) {
    NaiveThread* pThread = new NaiveThread();
    threads_.push_back(pThread);
    pThread->start();
  }

  return maxnum;
}

int NaiveThreadPoolImpl::numBusyThreads() const {
  CriticalRegion  threadsGuard(threadsLock_);
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
  CriticalRegion  threadsGuard(threadsLock_);

  int total = threads_.size();

  int numbusy = 0;

  for (std::list<NaiveThread*>::const_iterator it(threads_.begin());
       it != threads_.end();
       ++it) {
    if (!(*it)->isIdle()) {
      ++numbusy;
    }
  }

  return total - numbusy;
}

int NaiveThreadPoolImpl::numRunningThreads() const {
  CriticalRegion  threadsGuard(threadsLock_);
  return threads_.size();
}



void NaiveThreadPoolImpl::addTask(Task& task) {
  {
    CriticalRegion  guard(tasksLock_);
    tasks_.push_back(&task);
  }
  receiveTask_.set();
}

void NaiveThreadPoolImpl::run() {
  typedef std::list<NaiveThread*>::iterator Iter;

  ready_.set();

  for (;;) {
    int tasksSize = 0;

    {
      CriticalRegion  guard(tasksLock_);
      tasksSize = tasks_.size();
    }

    if (stop_ && !tasksSize) {
      stopReady_.set();
      break;
    }



    if (!tasksSize) {
      receiveTask_.wait();
      continue;
    }

    //lock order :
    //   1)threadsGuard ,
    //   2)tasksGuard
    {
      CriticalRegion  threadsGuard(threadsLock_);

      for (Iter it = threads_.begin(); it != threads_.end(); ++it) {
        if ((*it)->isIdle()) {
          Task* task;
          {
            //just pick one task from the front of the double-link-list
            CriticalRegion  guard(tasksLock_);

            if (tasks_.size() == 0) {
              break;
            }

            task = tasks_.front();
            tasks_.pop_front();
          }

          (*it)->getTask(*task);
        }
      }

    }

  }
}

int NaiveThreadPoolImpl::killIdleThreads(size_t num) {
  CriticalRegion threadsGuard(threadsLock_);

  typedef std::list<NaiveThread*>::iterator Iter;
  int numDeleted = 0;

  if (num = 0) {
    num = threads_.size();
  }

  for (Iter iter = threads_.begin();
       iter != threads_.end() && numDeleted < num;)  {
    if ((*iter)->isIdle()) {
      (*iter)->syncKill();
      delete *iter;
      ++numDeleted;
      iter = threads_.erase(iter);

    } else {
      ++iter;
    }
  }

  return numDeleted;
}



} //namespace ndsl
