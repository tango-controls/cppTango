//=============================================================================
//
// file :		attrmanip.h
//
// description :	Include for the Tango attribute manipulator
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//
//=============================================================================

#ifndef _ATTRMANIP_H
#define _ATTRMANIP_H

#include <tango.h>
#include <algorithm>

namespace Tango
{

class AttrManip
{
	friend std::ostream &operator<<(std::ostream &,const AttrManip&);
	friend void execute_manip(std::ostream &, std::string &str);

public:
	AttrManip(const char *f):format(f) {
    std::transform(format.begin(),format.end(),format.begin(),::tolower);
	}

	AttrManip(const std::string &str):format(str) {
    std::transform(format.begin(),format.end(),format.begin(),::tolower);
	}

  inline const std::string& to_string (void) const {
    return format;
  }

private:
	std::string format;
};


} // End of Tango namespace

#endif
