//
// PatternLayout.hh
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

#ifndef _LOG4TANGO_PATTERNLAYOUT_H
#define _LOG4TANGO_PATTERNLAYOUT_H

#include <log4tango/Portability.hh>
#include <log4tango/Layout.hh>
#include <vector>
#ifdef LOG4TANGO_HAVE_SSTREAM
# include <sstream>
#endif

namespace log4tango {

//-----------------------------------------------------------------------------
// class : PatternLayout (a simple fixed format Layout implementation)
//-----------------------------------------------------------------------------  
class LOG4TANGO_EXPORT PatternLayout : public Layout 
{
public:

  /**
  A conversion pattern equivalent to the BasicLayout.
  **/
  static const char* BASIC_CONVERSION_PATTERN;

  PatternLayout();

  virtual ~PatternLayout();
        
  // NOTE: 
  // All double percentage signs ('%%') followed by a character
  // in the following comments should actually be a single char.
  // The doubles are included so that doxygen will print them correctly.

  /**
   * Formats the LoggingEvent in the style set by
   * the set_conversion_pattern call. By default, set
   * to "%%m%%n"
   **/
  virtual std::string format(const LoggingEvent& event);

  /**
   * Sets the format of log lines handled by this
   * PatternLayout. By default, set to "%%m%%n".<br>
   * Format characters are as follows:<br>
   * <ul>
   * <li><b>%%%%</b> - a single percent sign</li>
   * <li><b>%%c</b> - the logger</li>
   * <li><b>%%d</b> - the date\n
   *  Date format: The date format character may be followed by a date format 
   *  specifier enclosed between braces. For example, %%d{%%H:%%M:%%S,%%l} or %%d{%%d %%m %%Y %%H:%%M:%%S,%%l}.
   *  If no date format specifier is given then the following format is used:
   *  "Wed Jan 02 02:03:55 1980". The date format specifier admits the same syntax 
   *  as the ANSI C function strftime, with 1 addition. The addition is the specifier
   *  %%l for milliseconds, padded with zeros to make 3 digits.</li>
   * <li><b>%%m</b> - the message</li>
   * <li><b>%%n</b> - the platform specific line separator</li>
   * <li><b>%%p</b> - the level</li>
   * <li><b>%%r</b> - milliseconds since this layout was created.</li>
   * <li><b>%%R</b> - seconds since Jan 1, 1970</li>
   * <li><b>%%u</b> - clock ticks since process start</li>
   * <li><b>%%x</b> - the NDC</li>
   * </ul>
   * @param conversionPattern the conversion pattern
   * @exception ConfigureFailure if the pattern is invalid
   **/
  virtual int set_conversion_pattern (const std::string& conversionPattern);

  virtual std::string get_conversion_pattern() const;

  virtual void clear_conversion_pattern();

  class LOG4TANGO_EXPORT PatternComponent {
    public:
      inline virtual ~PatternComponent() {};
      virtual void append(std::ostringstream& out, const LoggingEvent& event) = 0;
  };

  private:
    typedef std::vector<PatternComponent*> ComponentVector; 
    ComponentVector _components;
    std::string _conversionPattern;
}; 
       
} // namespace log4tango 

#endif // _LOG4TANGO_PATTERNLAYOUT_H
