/**
 * @file Mutex.hpp
 * @brief
 * @author biyu<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 19:33:08
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#ifndef _NDSL_MUTEX_HPP_H_
#define _NDSL_MUTEX_HPP_H_

#include <pthread.h>

namespace ndsl {

class Mutex {
  public:
    Mutex();
    virtual ~Mutex();
    void lock();
    void unlock();
  private:
    pthread_mutex_t mutex_;

    Mutex(const Mutex&);
    const Mutex& operator=(const Mutex& mutex);
};

}//namespace ndsl
#endif //_NDSL_MUTEX_HPP_H_
