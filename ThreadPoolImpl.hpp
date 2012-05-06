/**
 * @file ThreadPoolImpl.hpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Thu, 03 May 2012 17:13:46
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#ifndef _NDSL_THREAD_POOL_IMPL_H_
#define _NDSL_THREAD_POOL_IMPL_H_

#include <stddef.h>

namespace ndsl {

class Task;

class ThreadPoolImpl {
  public:
    ThreadPoolImpl(int minCap, int maxCap);
    virtual ~ThreadPoolImpl();

    /**
         * @brief numIdleThreads
         *
         * @returns    how many threads that are running but has no jobs to work (  in idle status.)
         */
    virtual int numIdleThreads() const   = 0;

    /**
     * @brief numBusyThreads
     *
     * @returns  the number of threads that are running task.
     */
    virtual int numBusyThreads() const = 0;

    /**
     * @brief numRunningThreads
     *
     * @returns   the number of threads that are running currently.
     */
    virtual int numRunningThreads() const = 0;


    /**
     * @brief addTask
     *  any time, any thread can add a task to this thread pool.
     * @param task  the task want to run
     */
    virtual void addTask(Task& task) = 0;

    /**
     * @brief start
     *  start runnning this threadpool, the thread will be in a dead loop,
     *    manage the tasks and dispatch tasks to his worker threads.
     *    threadpool will create @minCap number threads at first.
     *    use must use allocateThreads() method to increase the threads if
     *    they want to more threads, or use killIdleThreads()  to delete idle threads.
     */
    virtual void start() = 0;


    /**
     * @brief KillIdleThreads
     *
     * @param num        the num of idle threads  you want to kill(for  busy threads, you did't has no ability to kill.)
     *
     * @return the num of threads actually successfully killed by you.
     */
    virtual int killIdleThreads(size_t num) = 0;

    /**
     * @brief stop
     *   stop this threadpool
     */
    virtual void stop() = 0;

  private:
    ThreadPoolImpl(const ThreadPoolImpl&);
    const ThreadPoolImpl& operator=(const ThreadPoolImpl&);
    /* data */
};

}//namespace ndsl




#endif //_NDSL_THREAD_POOL_IMPL_H_
