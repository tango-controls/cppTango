static const char *RcsId = "$Header$";

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
// $Revision$
//
// $Log$
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <attrmanip.h>

namespace Tango
{

void execute_manip(ostream &o_str,string &manip)
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
		strstream o;
		long num;
		o << num_str;
		o >> num;
		
		o_str.precision(num);
	}
	else if (manip.substr(0,5) == "setw(")
	{
		string num_str = manip.substr(5,manip.size() - 6);
		strstream o;
		long num;
		o << num_str;
		o >> num;
		
		o_str.width(num);
	}
}

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

} // End of tango namespace
