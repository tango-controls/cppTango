
//====================================================================================================================
//
//  file :  			event.tpp
//
//	description : 		C++ classes for implementing the template method(s) of the EventConsumer class
//
//	author(s) : 		E. Taurel
//
//	original : 			March 2014
//
//  Copyright (C) :     2014,2015
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision: 25249 $
//
//
//====================================================================================================================


using namespace CORBA;

namespace Tango {


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::attr_to_device()
//
// description :
//		Method to initialize in the DeviceAttribute instance given to the user the attribute value which are received
//      in a AttrValUnion (or in a class inheriting from)
//
// argument :
//		in :
//			- attr_value :
//		out :
//			- dev_attr :
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void EventConsumer::base_attr_to_device(const T *attr_value,DeviceAttribute *dev_attr)
{
	dev_attr->name = attr_value->name;
	dev_attr->quality = attr_value->quality;
	dev_attr->time = attr_value->time;
	dev_attr->dim_x = attr_value->r_dim.dim_x;
	dev_attr->dim_y = attr_value->r_dim.dim_y;
	dev_attr->set_w_dim_x(attr_value->w_dim.dim_x);
	dev_attr->set_w_dim_y(attr_value->w_dim.dim_y);
	dev_attr->err_list = new DevErrorList(attr_value->err_list);
	dev_attr->data_format = attr_value->data_format;

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
	    att_union_to_device(&attr_value->zvalue,dev_attr);
	}
}

} /* End of Tango namespace */

