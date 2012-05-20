/**
 * @file Event.hpp
 * @brief
 * @author biyu<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 19:53:39
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#ifndef _NDSL_EVENT_HPP_H
#define _NDSL_EVENT_HPP_H

#include <pthread.h>

namespace ndsl  {
/**
 * @brief
 *  this class is just used for synchronise between multiple threads
 */
class Event {
  public:
    /**
     * @brief Event
     *
     * @Param reset   whether the event is restart after event occured.
     */
    Event(bool reset = false);
    virtual ~Event();

    /**
     * @brief set()
     *   this method tell the waiting threads  that the event occured.
     *   if reset = false ,  all waiting threads wake up, or only one thread is waked up.
     */
    void set();

    /**
     * @brief wait()
     *   this method make the calling thread to sleep until the event occur.
     */
    void wait();
  private:
    pthread_mutex_t mutex_;
    pthread_cond_t  cond_;
    bool reset_;
    volatile bool state_;
};

}//namespace ndsl
#endif // _NDSL_EVENT_HPP_H
