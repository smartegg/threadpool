#ifndef _NDSL_HANDLE_ERROR_H_
#define _NDSL_HANDLE_ERROR_H_
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace ndsl {


extern void handleError(const char* filename, const char* funcname,
                        const int line,
                        bool  issyscall);

}//namespace ndsl

#define handle_error(issyserr) \
  do {ndsl::handleError(__FILE__, __FUNCTION__, __LINE__, issyserr);  } while (0)

#define NDSL_ASSERT(r) \
  if (r!=0) handle_error(true)

#define dbg(a,b) \
  std::cout <<#a <<'\t' << a << '\t' << #b << '\t' << b << std::endl

#endif //_NDSL_HANDLE_ERROR_H_
