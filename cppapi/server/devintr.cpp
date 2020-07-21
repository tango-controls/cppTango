//+==================================================================================================================
//
// file :               devintr.cpp
//
// description :        C++ source code for the DevIntr class
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2014,2015
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
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntr::get_interface
//
// description :
//		Get a device interface
//
// argument :
//		in :
//			- dev: Device ptr
//
//--------------------------------------------------------------------------------------------------------------------

void DevIntr::get_interface(DeviceImpl *dev)
{
	build_cmd_interfaces(dev,cmds);
	build_att_interfaces(dev,atts);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntr::build_cmd_interfaces
//
// description :
//		Build a ordered list of device commands interface
//
// argument :
//		in :
//			- dev: Device ptr
//		out :
//			- cmds: reference to vector where commands interface must be stored
//
//--------------------------------------------------------------------------------------------------------------------

void DevIntr::build_cmd_interfaces(DeviceImpl *dev,std::vector<CmdIntr> &cmds)
{

//
// Get commands interface at class and device levels
//

	cmds.clear();

	size_t cmd_class = dev->get_device_class()->get_command_list().size();
	size_t cmd_dev = dev->get_local_command_list().size();
	size_t nb_cmd = cmd_class + cmd_dev;

	cmds.reserve(nb_cmd);

	for(size_t loop = 0;loop < cmd_class;loop++)
	{
		CmdIntr ci;
		Command *cmd_ptr = (dev->get_device_class()->get_command_list())[loop];
		ci.name = cmd_ptr->get_lower_name();
		ci.in_type = cmd_ptr->get_in_type();
		ci.out_type = cmd_ptr->get_out_type();
		cmds.push_back(ci);
	}

	for(size_t loop = 0;loop < cmd_dev;loop++)
	{
		CmdIntr ci;
		Command *cmd_ptr = (dev->get_local_command_list())[loop];
		ci.name = cmd_ptr->get_lower_name();
		ci.in_type = cmd_ptr->get_in_type();
		ci.out_type = cmd_ptr->get_out_type();
		cmds.push_back(ci);
	}

//
// Sort this list
//

	sort(cmds.begin(),cmds.end());

}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntr::build_att_interfaces
//
// description :
//		Build a ordered list of device attributes interface
//
// argument :
//		in :
//			- dev: Device ptr
//		out :
//			- atts: reference to vector where commands interface must be stored
//
//--------------------------------------------------------------------------------------------------------------------

void DevIntr::build_att_interfaces(DeviceImpl *dev,std::vector<AttrIntr> &atts)
{

//
// Get attribute(s) interface
//

	atts.clear();

	size_t nb_attr = dev->get_device_attr()->get_attribute_list().size();
	atts.reserve(nb_attr);

	for (size_t loop = 0;loop < nb_attr;loop++)
	{
		AttrIntr ai;
		Attribute *att_ptr = (dev->get_device_attr()->get_attribute_list())[loop];
		ai.name  = att_ptr->get_impl().get_name_lower();
		ai.writable = att_ptr->get_writable();
		ai.data_type = att_ptr->get_data_type();
		ai.data_format = att_ptr->get_data_format();
		ai.max_x = att_ptr->get_max_dim_x();
		ai.max_y = att_ptr->get_max_dim_y();
		ai.enum_labels = att_ptr->get_impl().get_enum_labels();

		if (ai.writable == READ_WRITE || ai.writable == WRITE)
		{
			WAttribute *w_att_ptr = static_cast<WAttribute *>(att_ptr);
			ai.mem = w_att_ptr->get_impl().get_memorized();
			ai.mem_init = w_att_ptr->get_impl().get_memorized_init();
			ai.writable_attr_name = w_att_ptr->get_assoc_name();
		}
		else
		{
			ai.mem = false;
			ai.mem_init = false;
		}

		atts.push_back(ai);
	}

//
// Sort the list
//

	sort(atts.begin(),atts.end());

}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntr::has_changed
//
// description :
//		Check if the device interface has changed
//
// argument :
//		in :
//			- dev: Device ptr
//
// return:
//		True if the device interface has changed
//
//--------------------------------------------------------------------------------------------------------------------

bool DevIntr::has_changed(DeviceImpl *dev)
{
	bool ret = false;
//
// First, check for commands
//

	std::vector<CmdIntr> v_ci;
	build_cmd_interfaces(dev,v_ci);

	if (v_ci != cmds)
		ret = true;
	else
	{

//
// Check attributes
//

		std::vector<AttrIntr> v_ai;
		build_att_interfaces(dev,v_ai);

		if (v_ai != atts)
			ret = true;
	}

	return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntr::== operator
//
// description :
//		Check the equality of inner structure
//
// argument :
//		in :
//			- rhs: The right hand side of the equality
//
// return:
//		True if the two instances are equals
//
//--------------------------------------------------------------------------------------------------------------------

bool DevIntr::CmdIntr::operator==(const struct CmdIntr &rhs) const
{
	bool ret = true;
	if (name != rhs.name)
		ret = false;
	else
	{
		if (in_type != rhs.in_type)
			ret = false;
		else
		{
			if (out_type != rhs.out_type)
				ret = false;
		}
	}
	return ret;
}

bool DevIntr::AttrIntr::operator==(const struct AttrIntr &rhs) const
{
	bool ret = true;
	if (name != rhs.name)
		ret = false;
	else
	{
		if (writable != rhs.writable)
			ret = false;
		else
		{
			if (data_type != rhs.data_type)
				ret = false;
			else
			{
				if (data_format != rhs.data_format)
					ret = false;
				else
				{
					if (max_x != rhs.max_x)
						ret = false;
					else
					{
						if (max_y != rhs.max_y)
							ret = false;
						else
						{
							if (mem != rhs.mem)
								ret = false;
							else
							{
								if (mem_init != rhs.mem_init)
									ret = false;
								else
								{
									if (writable_attr_name != rhs.writable_attr_name)
										ret = false;
									else
									{
										if (enum_labels.size() != rhs.enum_labels.size())
											ret = false;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

} // End of Tango namespace
