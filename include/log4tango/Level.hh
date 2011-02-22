/*
 * Level.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LEVEL_H
#define _LOG4TANGO_LEVEL_H

#include <log4tango/Portability.hh>
#include <string>
#include <stdexcept>

/*
 * Optionally work around rudeness in windows.h on Win32.
 */
#ifdef ERROR
#ifdef LOG4TANGO_FIX_ERROR_COLLISION

namespace log4tango {
    static const int _tmpERRORValue = ERROR;
}

#undef ERROR
    static const int ERROR = log4tango::_tmpERRORValue;
#define ERROR ERROR

#else  // LOG4TANGO_FIX_ERROR_COLLISION
#error Naming collision for 'ERROR' detected. Please read the FAQ for a \
       workaround. 
#endif // LOG4TANGO_FIX_ERROR_COLLISION 

#endif // ERROR

/*
 * Other Win32 rudeness in EDK.h
 */
#ifdef DEBUG

#ifdef LOG4TANGO_FIX_ERROR_COLLISION

#undef DEBUG
#define DEBUG DEBUG

#else  // LOG4TANGO_FIX_ERROR_COLLISION
#error Naming collision for 'DEBUG' detected. Please read the FAQ for a \
       workaround. 
#endif // LOG4TANGO_FIX_ERROR_COLLISION 

#endif // DEBUG


namespace log4tango {

//-----------------------------------------------------------------------------
// class : Level
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT Level 
{
public:

  /**
   * Levels of Priorities.
   **/
  typedef enum {
    OFF         =  100,
    FATAL       =  200,
    ERROR       =  300, 
    WARN        =  400,
    INFO        =  500,
    DEBUG       =  600
  } LevelLevel;

  /**
   * The type of Level Values
   **/
  typedef int Value;

  /**
   * Returns the name of the given level value.
   * Currently, if the value is not one of the LevelLevel values,
   * the method returns the name of the largest level smaller 
   * the given value.
   * @param level the numeric value of the level.
   * @returns a string representing the name of the level.
   **/
  static const std::string& get_name (Value level);
	
	/**
	 * Returns the value of the given level name. 
	 * This can be either one of "OFF", "ERRROR", ... or a 
	 * decimal string representation of the value, e.g. '500' for DEBUG.
	 * @param level_name the string containing the the of the level
	 * @return the value corresponding with the level name
	 * @throw std::invalid_argument if the level_name does not 
	 * correspond with a known Level name or a number
	 **/
   static Value get_value (const std::string& level_name)
	    throw(std::invalid_argument);
};

} // namespace log4tango

#endif // _LOG4TANGO_LEVEL_H
