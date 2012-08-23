static const char *RcsId = "$Id: device_4.cpp 20742 2012-06-21 14:20:20Z taurel $";

//+============================================================================
//
// file :               Device_5.cpp
//
// description :        C++ source code for the DeviceImpl and DeviceClass
//			classes. These classes
//			are the root class for all derived Device classes.
//			They are abstract classes. The DeviceImpl class is the
//			CORBA servant which is "exported" onto the network and
//			accessed by the client.
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
// $Revision: 20742 $
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <device_5.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Device_5Impl::Device_5Impl
//
// description : 	constructors for the device_impl class from the
//			class object pointer, the device name,
//			the description field, the state and the status.
//			Device_4Impl inherits from DeviceImpl. These constructors
//			simply call the correct DeviceImpl class
//			constructor
//
//--------------------------------------------------------------------------

Device_5Impl::Device_5Impl(DeviceClass *device_class,string &dev_name):
Device_4Impl(device_class,dev_name),ext_5(Tango_NullPtr)
{
	idl_version = 5;
}

Device_5Impl::Device_5Impl(DeviceClass *device_class,
			   string &dev_name,
			   string &desc):
Device_4Impl(device_class,dev_name,desc),ext_5(Tango_NullPtr)
{
	idl_version = 5;
}

Device_5Impl::Device_5Impl(DeviceClass *device_class,
	           	   string &dev_name,string &desc,
	           	   Tango::DevState dev_state,string &dev_status):
Device_4Impl(device_class,dev_name,desc,dev_state,dev_status),ext_5(Tango_NullPtr)
{
	idl_version = 5;
}

Device_5Impl::Device_5Impl(DeviceClass *device_class,
	           	   const char *dev_name,
			   const char *desc,
	           	   Tango::DevState dev_state,
	           	   const char *dev_status):
Device_4Impl(device_class,dev_name,desc,dev_state,dev_status),ext_5(Tango_NullPtr)
{
	idl_version = 5;
}



//+-------------------------------------------------------------------------
//
// method : 		Device_5Impl::get_attribute_config_5
//
// description : 	CORBA operation to get attribute configuration.
//
// argument: in :	- names: name of attribute(s)
//
// This method returns a pointer to a AttributeConfigList_5 with one
// AttributeConfig_5 structure for each atribute
//
//--------------------------------------------------------------------------

Tango::AttributeConfigList_5 *Device_5Impl::get_attribute_config_5(const Tango::DevVarStringArray& names)
{
	TangoMonitor &mon = get_att_conf_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_5Impl::get_attribute_config_5 arrived" << endl;
}

//+-------------------------------------------------------------------------
//
// method :		Device_5Impl::set_attribute_config_5
//
// description :	CORBA operation to set attribute configuration locally
//			and in the Tango database
//
// argument: in :	- new_conf: The new attribute(s) configuration. One
//				    AttributeConfig structure is needed for each
//				    attribute to update
//
//--------------------------------------------------------------------------

void Device_5Impl::set_attribute_config_5(const Tango::AttributeConfigList_5& new_conf,
										  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_5Impl::set_attribute_config_5 arrived" << endl;

//
// The attribute conf. is protected by two monitors. One protects access between
// get and set attribute conf. The second one protects access between set and
// usage. This is the classical device monitor
//

	TangoMonitor &mon1 = get_att_conf_monitor();
	AutoTangoMonitor sync1(&mon1);

}

} // End of Tango namespace
