/*
 * Level.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4tango/Level.hh>

namespace log4tango {

#define NUM_LEVELS 7

static const std::string names[NUM_LEVELS] = {
    std::string("OFF"),
    std::string("FATAL"), 
    std::string("ERROR"), 
    std::string("WARN"),
    std::string("INFO"), 
    std::string("DEBUG"),
    std::string("UNKNOWN") 
};

#define kMAX_LEVEL DEBUG

const std::string& Level::get_name (Value level) {
  level /= 100;
  level--;
  int max = NUM_LEVELS - 1;
  return names[((level < 0) || (level > max)) ? max : level];
}

Level::Value Level::get_value(const std::string& level_name) 
    throw(std::invalid_argument) 
{
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
