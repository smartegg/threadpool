/**
 * @file ThreadPool.hpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 21:32:46
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#ifndef _NDSL_THREAD_POOL_HPP_
#define _NDSL_THREAD_POOL_HPP_

#include "ThreadPoolImpl.hpp"
#include "Task.hpp"
namespace ndsl {


/**
 * @brief ThreadPool
 *  this thread pool manage many threads with scalability, impl with  impl idiom.
 *    Our goal is to  not to burden the system and can run tasks effiently.
 *    (1)when the thread is in idle status, user can delete it.
 *    (2)we manage the number of threads between @minCap and @maxCap
 *    (3) if the thread pool is full,  the tasks will just be filled in a queue and waiting for the thread.
 *  TODO:
 *  consider  impl:
 *     1)a  naive master/slave ThreadPool  :  NaiveThreadPoolImpl
 *     2)a Leader/Follower ThreadPool      :  LFThreadPoolImpl
 *     3)a Half Sync/Half Async ThreadPool :  HSHAThreadPoolImpl
 */
class ThreadPool {
  public:
    enum THREAD_POOL_TYPE {
      NAIVE_POOL = 1,
      LF_POOL = 2,
      HSHA_POOL = 3
    };


    /**
     * @brief ThreadPool
     *
     * @param minCap  the inital  threads you want to create when start,
     *                the threadpool will always has at leat @minCap threads running
     * @param maxCap  the max threads the threadpool  can allocated.
     * @param type    the inner-impl method you want to use  to use the  ThreadPool
     */
    ThreadPool(int minCap = 2, int maxCap = 16, THREAD_POOL_TYPE type = NAIVE_POOL);


    /**
     * @brief ~ThreadPool
     *  wait until  numBusyThreads() = 0, then free all allocated threads in the pool.
     * @caution:  so if user made a mistake and cause the task runniing in dead loop , we just can wait forever!
     */
    virtual ~ThreadPool();

    /**
     * @brief numIdleThreads
     *
     * @returns    how many threads that are running but has no jobs to work (  in idle status.)
     */
    int numIdleThreads() const ;

    /**
     * @brief numBusyThreads
     *
     * @returns  the number of threads that are running task.
     */
    int numBusyThreads() const;

    /**
     * @brief numRunningThreads
     *
     * @returns   the number of threads that are running currently.
     */
    int numRunningThreads() const;


    /**
     * @brief addTask
     *  any time, any thread can add a task to this thread pool.
     * @param task  the task want to run
     */
    void addTask(Task& task);

    /**
     * @brief allocateThreads
     *  allocate new threads to handle the tasks.
     * @param num
     * @return the num of threads actually successfully allocated.
     */
    int allocateThreads(size_t num);


    /**
     * @brief KillIdleThreads
     *
     * @param num        the num of idle threads  you want to kill(for  busy threads, you did't has no ability to kill.)
     *
     * @return the num of threads actually successfully killed by you.
     */
    int killIdleThreads(size_t num);

    void stop();


  private:
    ThreadPoolImpl* pImpl_;

    ThreadPool(const ThreadPool&);
    const ThreadPool& operator=(const ThreadPool&);
};


inline void ThreadPool::addTask(Task& task) {
  pImpl_->addTask(task);
}



inline int ThreadPool::numBusyThreads() const {
  return pImpl_->numBusyThreads();
}

inline int ThreadPool::numIdleThreads() const {
  return pImpl_->numIdleThreads();
}

inline int ThreadPool::numRunningThreads() const {
  return pImpl_->numRunningThreads();
}


inline int ThreadPool::allocateThreads(size_t num) {
  return pImpl_->allocateThreads(num);
}

inline void ThreadPool::stop() {
  pImpl_->stop();
}

}//namespace ndsl
#endif//_NDSL_THREAD_POOL_HPP_
