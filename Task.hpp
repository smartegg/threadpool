/**
 * @file Task.hpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Fri, 04 May 2012 13:55:11
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#ifndef _NDSL_TASK_H_
#define _NDSL_TASK_H_

namespace  ndsl {

/** @brief Task
 *  this is the task interface that WorkerThreads need to run..
 * */
class Task {
  public:
    Task();
    virtual ~Task();

    /** @brief run
     *  people need to override this method,
     *   this method is called by WorkerThread.
     * */
    virtual void run() = 0;

  private:
    Task(const Task&);
    const Task& operator=(const Task&);

};
} //namespace ndsl

#endif // _NDSL_TASK_H_

