/**
 * @file CriticalRegion.hpp
 * @brief
 * @author biyu<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 14:57:41
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#ifndef _NDSL_CRITICAL_REGION_HPP_
#define _NDSL_CRITICAL_REGION_HPP_

#include <pthread.h>
#include "Mutex.hpp"
namespace ndsl {

/**
 * @brief
 *   this class just be used as a critical region lock.
 */
class CriticalRegion {
  public:
    CriticalRegion(Mutex& mutex);
    virtual ~CriticalRegion();
  private:
    Mutex& mutex_;

    CriticalRegion(const CriticalRegion&);
    const CriticalRegion& operator=(const CriticalRegion&);
};
}//namespace ndsl
#endif//_NDSL_CRITICAL_REGION_HPP_
