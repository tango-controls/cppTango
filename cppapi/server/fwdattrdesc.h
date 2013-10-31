//===================================================================================================================
//
// file :		fwdattrdesc.h
//
// description :	Include file for the FwdAttr classes hierarchy. Three classes are declared in this file :
//				The FwdAttr class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//===================================================================================================================

#ifndef _FWDATTRDESC_H
#define _FWDATTRDESC_H

#include <tango.h>

namespace Tango
{

class FwdAttr: public Attr
{
public:
	FwdAttr(const char *name,const string &root_attribute=RootAttNotDef);

	virtual bool is_fwd() {return true;}

	string &get_fwd_root_att() {return fwd_root_att;}
	string &get_fwd_dev_name() {return fwd_dev_name;}
	bool is_correctly_configured() {return fwd_wrongly_conf;}

	virtual void read(DeviceImpl *,Attribute &);
	virtual void write(DeviceImpl *,WAttribute &);
	virtual bool is_allowed(DeviceImpl *,AttReqType) {return true;}

	void init_conf(AttributeInfoEx *);
	bool validate_fwd_att(vector<AttrProperty> &);
	void get_root_conf(string &);

protected:
	string					full_root_att;			// Root att (dev_name/att_name)
	string					fwd_dev_name;			// Root att device name (lower case)
	string					fwd_root_att;			// Root att (lower case)
	bool					fwd_wrongly_conf;

private:
    class FwdAttrExt
    {
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<FwdAttrExt>    	ext;           // Class extension
#else
	FwdAttrExt		        	*ext;
#endif
};

} // End of Tango namespace

#endif /* _FWDATTRDESC_H */
