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
// $Revision$
//
// $Log$
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
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
	friend ostream &operator<<(ostream &,const AttrManip&);
	friend void execute_manip(ostream &,string &str);
public:
	AttrManip(const char *f):format(f)
	{
		transform(format.begin(),format.end(),format.begin(),tolower);	
	}
	
private:
	string format;	
};


} // End of Tango namespace

#endif
