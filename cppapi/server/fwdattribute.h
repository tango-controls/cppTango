
//===================================================================================================================
//
// file :		fwdattribute.h
//
// description :	Include file for the FwdAttribute classes.
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2013
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//====================================================================================================================

#ifndef _FWDATTRIBUTE_H
#define _FWDATTRIBUTE_H

#include <tango.h>

namespace Tango
{

class FwdAttribute: public Attribute
{
public:
	FwdAttribute(vector<AttrProperty> &,Attr &,string &,long);
	~FwdAttribute();

	virtual bool is_fwd_wrongly_conf() {return fwd_wrongly_conf;}
	virtual bool is_fwd_att() {return true;}
	string &get_fwd_dev_name() {return fwd_dev_name;}
	string &get_fwd_att_name() {return fwd_att_name;}

protected:
	bool				fwd_wrongly_conf;				// Flag set to true if the att is forwarded and wrongly conf.

	string				fwd_dev_name;					// Root dev name for fwd attribute
	string				fwd_att_name;					// Root att name for fwd attribute
};

} // End of Tango namespace

#endif // _FWDATTRIBUTE_H
