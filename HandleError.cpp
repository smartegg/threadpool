#include "HandleError.hpp"

#include <string>
#include <cstring>
#include <sstream>

namespace ndsl {

void handleError(const char* filename,
                 const char* funcname,
                 const int line,
                 bool issyserr,
                 int err,
                 bool needExit) {
  std::stringstream stream;
  std::string result;
  stream << line - 1; //function failed is the line before it.
  stream >> result;


  std::string info("ERROR: ");
  info.append(filename);
  info.append(":");
  info.append(result);
  info.append(std::string("\tin function ") + funcname);
  info.append("() ");

  if (issyserr) {
    info.append(": ");
    info.append(strerror(err));
  }

  std::cerr << info << std::endl;

  if (needExit)
      exit(-1);
}



}//namespace ndsl

