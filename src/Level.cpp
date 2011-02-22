//
// Level.cpp
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010
//					Synchrotron SOLEIL
//                	L'Orme des Merisiers
//                	Saint-Aubin - BP 48 - France
//
// This file is part of log4tango.
//
// Log4ango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Log4tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with Log4Tango.  If not, see <http://www.gnu.org/licenses/>.

#include "PortabilityImpl.hh"
#include <log4tango/Level.hh>

#include <cstdlib>

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
