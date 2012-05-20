/**
 * @file NaiveThreadPoolImpl.hpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Fri, 04 May 2012 14:34:08
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#ifndef _NDSL_NAIVETHREAD_POOL_IMPL_HPP_
#define _NDSL_NAIVETHREAD_POOL_IMPL_HPP_

#include <list>
#include <queue>
#include "ThreadPoolImpl.hpp"
#include "Task.hpp"
#include "Event.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"


namespace ndsl {

class NaiveThread;
/**
 * @brief NaiveThreadPoolImpl
 * this class impl a master/slave threadpool.
 * */
class NaiveThreadPoolImpl : public  Task, public ThreadPoolImpl {
  public:
    NaiveThreadPoolImpl(int minCap, int maxCap);
    virtual ~NaiveThreadPoolImpl();
    /**
     * @brief numIdleThreads
     *
     * @returns    how many threads that are running but has no jobs to work (  in idle status.)
     */
    virtual int numIdleThreads() const;

    /**
     * @brief numBusyThreads
     *
     * @returns  the number of threads that are running task.
     */
    virtual int numBusyThreads() const;

    /**
     * @brief numRunningThreads
     *
     * @returns   the number of threads that are running currently.
     */
    virtual int numRunningThreads() const;

    /**
     * @brief start
     *   block untill all worker/master threads  in the pool start to run.
     */
    virtual void start() ;


    /**
     * @brief addTask
     *  any time, any thread can add a task to this thread pool.
     * @param task  the task want to run
     */
    virtual void addTask(Task& task);


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
     *                   num =0, if you want to kill all thread.
     *
     * @return the num of threads actually successfully killed by you.
     */
    virtual int killIdleThreads(size_t num = 0);

    /**
     * @brief stop
     *  just  notify the threadpool to destroy all their threads , we didn't  wait it successful running
     *  caution: you cannot  invoke this function then immediately delete this threadpool ,
     *          because we just notify the threadpool to kill itself in a time,
     *          but we don't know when it is killed (after all tasks left done)
     *          so, prefer use the syncStop() version for safety
     */
    virtual void stop();

    /**
     * @brief syncStop
     *   we block  the  threadpool until all tasks left are done  and the threads in the threadpool all killed
     *   caution: you can invoke this function if you really want to kill this threadpool.
     */
    virtual void syncStop();

  private:
    /**
     * @brief run
     *  start runnning this threadpool, the thread will be in a dead loop,
     *    manage the tasks and dispatch tasks to his worker threads.
     *    threadpool will create @minCap number threads at first.
     *    use must use allocateThreads() method to increase the threads if
     *    they want to more threads, or use killIdleThreads()  to delete idle threads.
     */
    void run();

    NaiveThreadPoolImpl(const NaiveThreadPoolImpl&);
    const NaiveThreadPoolImpl& operator=(const NaiveThreadPoolImpl&);

    int minCap_;
    int maxCap_;
    bool stop_;

    std::list<NaiveThread*> threads_;
    std::deque<Task*> tasks_;
    Mutex tasksLock_;
    Event receiveTask_;

    mutable Thread masterThread_;
    Event ready_;
    Event stopReady_;
};

inline void  NaiveThreadPoolImpl::stop() {
  stop_ = true;
  receiveTask_.set();
}

} //namespace ndsl



#endif // _NDSL_NAIVETHREAD_POOL_IMPL_HPP_

