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
 *  the thread which the NaiveThreadpool manage
 */
class NaiveThread : private Task {
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
     *   notify  this  thread to kill it self, stop its running
     */
    void kill();

    void start();


  private:
    void run();

    Thread thread_;
    volatile bool kill_;//or use atomic_t  instead of volatile ?  need  to lock kill_ ?
    Task* volatile task_;

    Event ready_;
    Event getTask_;

    NaiveThread(const NaiveThread&);
    const NaiveThread& operator=(const NaiveThread&);
};

NaiveThread::NaiveThread() : kill_(false), task_(0) {

}


NaiveThread::~NaiveThread() {
  kill();
}

bool NaiveThread::getTask(Task& task) {

  if (task_ != 0) {
    return false;
  }

  task_ = &task;
  getTask_.set();
  return true;
}

bool NaiveThread::isIdle() const {
  return task_ == 0;
}

void NaiveThread::kill() {
  kill_ = true;
}

void NaiveThread::start() {
  thread_.start(*this);
  ready_.wait();
}

void NaiveThread::run() {
  ready_.set();

  for (; !kill_;) {
    getTask_.wait();
    task_->run();
    task_ = 0;
  }
}


NaiveThreadPoolImpl::NaiveThreadPoolImpl(int minCap, int maxCap)
  : ThreadPoolImpl(minCap, maxCap),
    minCap_(minCap),
    maxCap_(maxCap),
    stop_(false) {
  for (size_t i = 0; i < minCap; ++i) {
    NaiveThread* pThread = new NaiveThread();
    threads_.push_back(pThread);
    pThread->start();
  }
}


void NaiveThreadPoolImpl::start() {
  typedef std::list<NaiveThread*>::const_iterator Iter;
  for (Iter iter = threads_.begin(); iter != threads_.end(); ++iter) {
    (*iter)->start();
  }
  masterThread_.start(*this);
  ready_.wait();
}

NaiveThreadPoolImpl::~NaiveThreadPoolImpl() {
  typedef std::list<NaiveThread*>::iterator Iter;

  for (Iter iter = threads_.begin(); iter != threads_.end(); ++iter) {
    delete *iter;
  }
}

int NaiveThreadPoolImpl::allocateThreads(size_t num) {
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
    CriticalRegion  guard(tasksLock_);
    tasks_.push_back(&task);
  }
  receiveTask_.set();
}

void NaiveThreadPoolImpl::run() {
  typedef std::list<NaiveThread*>::iterator Iter;

  ready_.set();

  for (; !stop_;) {
    if (tasks_.size() == 0) {
      //FIXME: rethink this solution is right
      receiveTask_.wait();
    }

    for (Iter it = threads_.begin(); it != threads_.end(); ++it) {

      if (tasks_.size() == 0) {
        break;
      }

      if ((*it)->isIdle()) {
        Task* task;
        {//pick one task from the front of the double-link-list
          CriticalRegion  guard(tasksLock_);
          task = tasks_.front();
          tasks_.pop_front();
        }

        (*it)->getTask(*task);
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
      //FIXME:consider sync or async here?
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
