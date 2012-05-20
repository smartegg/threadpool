#ifndef _NDSL_HANDLE_ERROR_H_
#define _NDSL_HANDLE_ERROR_H_
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace ndsl {


extern void handleError(const char* filename, const char* funcname,
                        const int line,
                        bool  issyscall,
                        int err,
                        bool needExit = true);

}//namespace ndsl

#define handle_error(issyserr, err) \
  do {ndsl::handleError(__FILE__, __FUNCTION__, __LINE__, issyserr, err);  } while (0)

#define NDSL_ASSERT(r) \
  do { if (r != 0) { handle_error(true, r);}} while(0)

#define NDSL_FAIL() \
    handle_error(false, 0)

#define NOTSUPPORT() \
    handle_error(false, 0)

#define NDSL_DEBUG(a,b) \
  std::cout <<#a <<'\t' << a << '\t' << #b << '\t' << b << std::endl

#define NDSL_STAB() \
  do {ndsl::handleError(__FILE__, __FUNCTION__, __LINE__, false, 0, false);  } while (0)


#endif //_NDSL_HANDLE_ERROR_H_
