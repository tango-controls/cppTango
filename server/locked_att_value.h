//=============================================================================
//
// file :		locked_att_value.h
//
// description :	Include for the LockedAttributeValue class
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2009,2010,2011,2012
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
// $Log$
//
//=============================================================================


#ifndef _LOCKEDATTVALUE_H
#define _LOCKEDATTVALUE_H

#include <tango.h>

namespace Tango
{

struct LockedAttributeValue_4: Tango::AttributeValue_4
{
	LockedAttributeValue_4():Tango::AttributeValue_4() {mut_ptr=NULL;}
	~LockedAttributeValue_4() {if (mut_ptr != NULL)mut_ptr->unlock();}

	void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}
	void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}
};

} // End of Tango namespace

#endif /* _LOCKEDATTVALUE */
