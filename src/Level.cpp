/*
 * Level.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4tango/Level.hh>

namespace log4tango {

#define NUM_LEVELS 7

#define kMAX_LEVEL DEBUG

/*static const std::string names[NUM_LEVELS] = {
    std::string("OFF"),
    std::string("FATAL"), 
    std::string("ERROR"), 
    std::string("WARN"),
    std::string("INFO"), 
    std::string("DEBUG"),
    std::string("UNKNOWN") 
};*/


//-----------------------------------------------------------------
//
// WARNING
// -------
//
// With gcc and Solaris, the above static array is not initialised !!!
// That's why it is copied within the two methods get_name and
// get_value. It is an horible hack but it works....
//
//------------------------------------------------------------------


const std::string& Level::get_name (Value level) {
static const std::string names[NUM_LEVELS] = {
    std::string("OFF"),
    std::string("FATAL"), 
    std::string("ERROR"), 
    std::string("WARN"),
    std::string("INFO"), 
    std::string("DEBUG"),
    std::string("UNKNOWN") 
};

  level /= 100;
  level--;
  int max = NUM_LEVELS - 1;
  return names[((level < 0) || (level > max)) ? max : level];
}

Level::Value Level::get_value(const std::string& level_name) 
    throw(std::invalid_argument) 
{
static const std::string names[NUM_LEVELS] = {
    std::string("OFF"),
    std::string("FATAL"), 
    std::string("ERROR"), 
    std::string("WARN"),
    std::string("INFO"), 
    std::string("DEBUG"),
    std::string("UNKNOWN") 
};	
	Level::Value value = -1;
	for (unsigned int i = 0; i < NUM_LEVELS; i++) {
	  if (level_name == names[i]) {
		  value = i * 100;
		  break;
	  }
	}
	if (value == -1) {
		char* end_pointer;
		value = std::strtoul(level_name.c_str(), &end_pointer, 10);
		if (*end_pointer != 0) {
		  throw std::invalid_argument(std::string("unknown level name: '") 
                                  + level_name + "'");
		}
	}
	return value;
}

} // namespace log4tango
