#include "HandleError.hpp"

#include <string>
#include <sstream>

namespace ndsl {

void handleError(const char* filename,
                 const char* funcname,
                 const int line,
                 bool issyserr) {
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
    perror(info.c_str());

  } else {
    std::cerr << info << std::endl;
  }

  exit(-1);
}



}//namespace ndsl

