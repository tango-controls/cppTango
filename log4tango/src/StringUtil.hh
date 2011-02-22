//
// StringUtil.hh
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

#ifndef _LOG4TANGO_STRINGUTIL_H
#define _LOG4TANGO_STRINGUTIL_H

#include "PortabilityImpl.hh"
#include <string>
#include <vector>
#include <climits>
#include <stdarg.h>

namespace log4tango {

class StringUtil 
{
public:

  /**
  Returns a string contructed from the a format specifier
  and a va_list of arguments, analogously to vprintf(3).
  @param format the format specifier.
  @param args the va_list of arguments.
  **/
  static std::string vform(const char* format, va_list args);

  /**
  Returns a string identical to the given string but without leading
  or trailing HTABs or spaces.
  **/
  static std::string trim(const std::string& s);

  /**
  splits a string into a vector of string segments based on the
  given delimiter.
  @param v The vector in which the segments will be stored. The vector
  will be emptied before storing the segments
  @param s The string to split into segments.
  @param delimiter The delimiter character
  @param maxSegments the maximum number of segments. Upon return
  v.size() <= maxSegments.  The string is scanned from left to right
  so v[maxSegments - 1] may contain a string containing the delimiter
  character.
  @return The actual number of segments (limited by maxSegments).
  **/
  static unsigned int split(std::vector<std::string>& v, 
				                    const std::string& s, char delimiter,
				                    unsigned int maxSegments = INT_MAX);

  /**
  splits a string into string segments based on the given delimiter
  and assigns the segments through an output_iterator.
  @param output The output_iterator through which to assign the string
  segments. Typically this will be a back_insertion_iterator.
  @param s The string to split into segments.
  @param delimiter The delimiter character
  @param maxSegments The maximum number of segments.
  @return The actual number of segments (limited by maxSegments).
  **/
  template<typename T> 
  static unsigned int split(T& output,
                            const std::string& s, char delimiter,
                            unsigned int maxSegments = INT_MAX) 
  {
    std::string::size_type left = 0;
    unsigned int i;
    for (i = 1; i < maxSegments; i++) {
      std::string::size_type right = s.find(delimiter, left);
      if (right == std::string::npos) {
          break;
      }
      *output++ = s.substr(left, right - left);
      left = right + 1;
    }
    *output++ = s.substr(left);
    return i;
  }
};

} // namespace log4tango

#endif // _LOG4TANGO_STRINGUTIL_HH

