static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               attrmanip.cpp
//
// description :        C++ source code for the tango attribute manipulator
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attrmanip.h>

namespace Tango
{

void execute_manip(ostream &o_str, string &manip)
{

//
// Set the ostream according to the manipulator
//

	if (manip == "fixed")
	{
		o_str.setf(ios::fixed,ios::floatfield);
		return;
	}
	else if (manip == "scientific")
	{
		o_str.setf(ios::scientific,ios::floatfield);
		return;
	}
	else if (manip == "uppercase")
	{
		o_str.setf(ios::uppercase);
		return;
	}
	else if (manip == "showpoint")
	{
		o_str.setf(ios::showpoint);
		return;
	}
	else if (manip == "showpos")
	{
		o_str.setf(ios::showpos);
		return;
	}
	else if (manip.substr(0,13) == "setprecision(")
	{
		string num_str = manip.substr(13,manip.size() - 14);
		TangoSys_MemStream o;
		long num;
		o << num_str;
		o >> num;

		o_str.precision(num);
	}
	else if (manip.substr(0,5) == "setw(")
	{
		string num_str = manip.substr(5,manip.size() - 6);
		TangoSys_MemStream o;
		long num;
		o << num_str;
		o >> num;

		o_str.width(num);
	}
}

//#ifndef TANGO_HAS_LOG4TANGO

ostream &operator<<(ostream &o_str,const AttrManip &manip)
{

//
// Extract each manipulator (; separated) and call the execute_manip
// for each one
//

	string::size_type start = 0;
	string str;
	string::size_type pos;

	while ((pos = manip.format.find(';',start)) != string::npos)
	{
		str = manip.format.substr(start,pos - start);
		start = pos + 1;
		execute_manip(o_str,str);

	}
	if (start != manip.format.size())
	{
		str = manip.format.substr(start);
		execute_manip(o_str,str);
	}

	return o_str;
}

//#endif // TANGO_HAS_LOG4TANGO

} // End of tango namespace
