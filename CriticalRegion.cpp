/**
 * @file CriticalRegion.cpp
 * @brief
 * @author biyu<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Wed, 02 May 2012 14:57:59
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include "CriticalRegion.hpp"

#include <pthread.h>
#include "HandleError.hpp"

namespace ndsl {

CriticalRegion::CriticalRegion(Mutex& mutex): mutex_(mutex) {
    mutex_.lock();
}

CriticalRegion::~CriticalRegion() {
    mutex_.unlock();
}

} //namespace ndsl


