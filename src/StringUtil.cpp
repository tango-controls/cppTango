//
// StringUtil.cpp
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

#include "StringUtil.hh"
#include <iterator>
#include <stdio.h>

#if defined(_MSC_VER)
    #define VSNPRINTF _vsnprintf
#else
#ifdef LOG4TANGO_HAVE_SNPRINTF
    #define VSNPRINTF vsnprintf
#else
/* use alternative snprintf() from http://www.ijs.si/software/snprintf/ */

#define HAVE_SNPRINTF
#define PREFER_PORTABLE_SNPRINTF

#include <stdlib.h>
#include <stdarg.h>

extern "C" {
#include "snprintf.c"
}

#define VSNPRINTF portable_vsnprintf

#endif // LOG4TANGO_HAVE_SNPRINTF
#endif // _MSC_VER

namespace log4tango {

    std::string StringUtil::vform(const char* format, va_list args) {
	size_t size = 1024;
	char* buffer = new char[size];
            
	while (1) {
	    int n = VSNPRINTF(buffer, size, format, args);
                
	    // If that worked, return a string.
	    if ((n > -1) && (static_cast<size_t>(n) < size)) {
		std::string s(buffer);
		delete [] buffer;
		return s;
	    }
                
	    // Else try again with more space.
            size = (n > -1) ?
                n + 1 :   // ISO/IEC 9899:1999
                size * 2; // twice the old size
                
	    delete [] buffer;
	    buffer = new char[size];
	}
    }

    std::string StringUtil::trim(const std::string& s) {
        // test for null string
        if(s.empty())
            return s;

        // find first non-space character
        std::string::size_type b = s.find_first_not_of(" \t");
        if(b == std::string::npos) // No non-spaces
            return "";

        // find last non-space character
        std::string::size_type e = s.find_last_not_of(" \t");

        // return the remaining characters
        return std::string(s, b, e - b + 1);
    }

    unsigned int StringUtil::split(std::vector<std::string>& v,
				   const std::string& s,
				   char delimiter, unsigned int maxSegments) {
        v.clear();
        std::back_insert_iterator<std::vector<std::string> > it(v);
        return split(it, s, delimiter, maxSegments);
    }

}
