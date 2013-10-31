static const char *RcsId = "$Id$\n$Name$";

//====================================================================================================================
//
// file :               fwdattribute.cpp
//
// description :        C++ source code for the FwdAttribute class. This class is used to manage attribute.
//						A Tango Device object instance has one MultiAttribute object which is an aggregate of
//						Attribute, FwdAttribute, WAttribute and FwdWAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2013
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
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
//====================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <fwdattribute.h>

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::FwdAttribute
//
// description :
//		Constructor for the FwdAttribute class from the attribute property vector, its type and the device name
//
// argument :
//		in :
//			- prop_list : The attribute property list
//			- tmp_attr :
//			- dev_name : The device name
//			- idx :
//
//--------------------------------------------------------------------------------------------------------------------

FwdAttribute::FwdAttribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx)
:Attribute(prop_list,tmp_attr,dev_name,idx)
{
	FwdAttr &attr = static_cast<FwdAttr &>(tmp_attr);

//
// Init forwarded attribute specific data
//

	fwd_wrongly_conf = attr.is_correctly_configured();
	fwd_dev_name = attr.get_fwd_dev_name();
	fwd_att_name = attr.get_fwd_root_att();

//
// Move this object in root attributes registry from Attrdesc to Attribute
//

	RootAttRegistry &fdp = Util::instance()->get_root_att_reg();
	fdp.clear_attrdesc(fwd_dev_name,fwd_att_name);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::~FwdAttribute
//
// description :
//		Destructor for the FwdAttribute class
//
//--------------------------------------------------------------------------------------------------------------------
FwdAttribute::~FwdAttribute()
{
	RootAttRegistry &fdp = Util::instance()->get_root_att_reg();
	fdp.remove_root_att(fwd_dev_name,fwd_att_name);
}

} // End of Tango namespace
