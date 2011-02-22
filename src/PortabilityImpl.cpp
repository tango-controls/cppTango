/*
 * PortabilityImpl.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"

#ifndef LOG4TANGO_HAVE_SSTREAM

namespace std {
  std::string ostringstream::str() { 
      (*this) << '\0'; 
      std::string msg(ostrstream::str()); 
      ostrstream::freeze(false); //unfreeze stream 
      return msg;         
  } 

  void ostringstream::str (const char* s) { 
    //TODO -> release current content, set content to s
  } 

}

#endif // LOG4TANGO_HAVE_SSTREAM
