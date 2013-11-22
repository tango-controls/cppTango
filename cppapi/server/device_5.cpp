static const char *RcsId = "$Id: device_4.cpp 20742 2012-06-21 14:20:20Z taurel $";

//+===================================================================================================================
//
// file :               Device_5.cpp
//
// description :        C++ source code for the Device_5Impl class. This class is the root class for all derived Device
//						classes starting with Tango 9
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
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
// $Revision: 20742 $
//
//-===================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <device_5.h>
#include <eventsupplier.h>
#include <device_3.tpp>


namespace Tango
{

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::Device_5Impl
//
// description :
//		Constructors for the Device_5impl class from the class object pointer, the device name, the description field,
// 		the state and the status. Device_5Impl inherits from DeviceImpl. These constructors simply call the correct
//		DeviceImpl class constructor
//
//--------------------------------------------------------------------------------------------------------------------

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



//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::get_attribute_config_5
//
// description :
//		CORBA operation to get attribute configuration.
//
// 		WARNING !!!!!!!!!!!!!!!!!!
//
// 		This is the release 3 of this CORBA operation which returns much more parameter than in release 2
// 		The code has been duplicated in order to keep it clean (avoid many "if" on version number in a common method)
//
// argument:
//		in :
//			- names: name of attribute(s)
//
// return :
//		Pointer to a AttributeConfigList_5 with one AttributeConfig_5 structure for each attribute
//
//-------------------------------------------------------------------------------------------------------------------

Tango::AttributeConfigList_5 *Device_5Impl::get_attribute_config_5(const Tango::DevVarStringArray& names)
{
	TangoMonitor &mon = get_att_conf_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_5Impl::get_attribute_config_5 arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList_5 *back = NULL;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config_5);

//
// Get attribute number and device version
//

	long nb_dev_attr = dev_attr->get_attr_nb();

//
// Check if the caller want to get config for all attribute.
// If the device implements IDL 3 or above  (State and status as attributes) and the client is an old one (not able to
// read state/status as attribute), decrement attribute number
//

	string in_name(names[0]);
	if (nb_attr == 1)
	{
		if (in_name == AllAttr_3)
		{
			all_attr = true;
			nb_attr = nb_dev_attr;
		}
	}

//
// Allocate memory for the AttributeConfig structures
//

	try
	{
		back = new Tango::AttributeConfigList_5(nb_attr);
		back->length(nb_attr);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_5Impl::get_attribute_config_5");
	}

//
// Fill in these structures
//

	for (long i = 0;i < nb_attr;i++)
	{
		try
		{
			if (all_attr == true)
			{
				Attribute &attr = dev_attr->get_attr_by_ind(i);
				attr.get_properties_5((*back)[i]);
			}
			else
			{
				Attribute &attr = dev_attr->get_attr_by_name(names[i]);
				attr.get_properties_5((*back)[i]);
			}
		}
		catch (Tango::DevFailed &)
		{
			delete back;
			throw;
		}
	}

//
// Return to caller
//

	cout4 << "Leaving Device_3Impl::get_attribute_config_5" << endl;

	return back;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Device_5Impl::set_attribute_config_5
//
// description :
//		CORBA operation to set attribute configuration locally and in the Tango database
//
// argument:
//		in :
//			- new_conf: The new attribute(s) configuration. One AttributeConfig structure is needed for each
//				    	attribute to update
//
//--------------------------------------------------------------------------------------------------------------------

void Device_5Impl::set_attribute_config_5(const Tango::AttributeConfigList_5& new_conf,
										  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_5Impl::set_attribute_config_5 arrived" << endl;

//
// The attribute conf. is protected by two monitors. One protects access between get and set attribute conf.
// The second one protects access between set and usage. This is the classical device monitor
//

	TangoMonitor &mon1 = get_att_conf_monitor();
	AutoTangoMonitor sync1(&mon1);

//
// Is it from the fwd attribute callback ?
//

	bool from_fwd_cb = false;
	Tango::LockerLanguage cl_lang = cl_id._d();
	if (cl_lang == Tango::CPP && cl_id.cpp_clnt() == 0)
	{
		from_fwd_cb = true;
	}

//
// Record operation request in black box
//

	if (from_fwd_cb == false)
		blackbox_ptr->insert_op(Op_Set_Attr_Config_5,cl_id);

//
// Check if the device is locked and by who
//

	check_lock("set_attribute_config_5");

//
// Call the Device_3Impl set_attribute_config
//

	store_in_bb = false;
	return set_attribute_config_3_local(new_conf,new_conf[0],from_fwd_cb,5);
}

} // End of Tango namespace
