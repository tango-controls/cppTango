//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
//						Synchrotron SOLEIL
//                		L'Orme des Merisiers
//                		Saint-Aubin - BP 48 - France
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.

#ifndef XSTRING_H
#define XSTRING_H

#include <sstream>
#include <string>

template<class T>   class XString{

public:	
	
	static	T convertFromString(const std::string& s)
	{
		std::istringstream in(s);
		T x;
		if (in >> x)
			return x;
		// some sort of error handling goes here...
		return 0;
	} 
//
	static	std::string convertToString(const T & t)
	{
		std::ostringstream out ;
		
		if (out << std::fixed << t   )
			return out.str();
		// some sort of error handling goes here...
		return 0;
	} 

};
#endif
