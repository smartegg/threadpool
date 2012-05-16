/**
 * @file Thread.hpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Sun, 06 May 2012 15:09:29
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#ifndef _NDSL_THREAD_HPP_
#define _NDSL_THREAD_HPP_

#include <pthread.h>
#include "Event.hpp"


namespace ndsl {

class Task;

class Thread {
  public:
    Thread ();
    virtual ~Thread ();

    void start(Task& task);


    pthread_t tid() const;

  protected:
    static void* run(void * arg);

  private:
    pthread_t thread_;
    Task* task_;

    Event ready_;

    Thread(const Thread &);
    const Thread& operator=(const Thread &);

};

inline pthread_t Thread::tid() const {
  return pthread_self();
}

} //namespace ndsl


#endif // _NDSL_THREAD_HPP_

