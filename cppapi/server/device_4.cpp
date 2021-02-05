//+============================================================================
//
// file :               Device_4.cpp
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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include <tango.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <device_4.h>
#include <eventsupplier.h>
#include <device_3.tpp>


namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Device_4Impl::Device_4Impl
//
// description : 	constructors for the device_impl class from the
//			class object pointer, the device name,
//			the description field, the state and the status.
//			Device_4Impl inherits from DeviceImpl. These constructors
//			simply call the correct DeviceImpl class
//			constructor
//
//--------------------------------------------------------------------------

Device_4Impl::Device_4Impl(DeviceClass *device_class,std::string &dev_name):
Device_3Impl(device_class,dev_name),ext_4(nullptr)
{
	idl_version = 4;
}

Device_4Impl::Device_4Impl(DeviceClass *device_class,
			   std::string &dev_name,
			   std::string &desc):
Device_3Impl(device_class,dev_name,desc),ext_4(nullptr)
{
	idl_version = 4;
}

Device_4Impl::Device_4Impl(DeviceClass *device_class,
	           	   std::string &dev_name,std::string &desc,
	           	   Tango::DevState dev_state,std::string &dev_status):
Device_3Impl(device_class,dev_name,desc,dev_state,dev_status),ext_4(nullptr)
{
	idl_version = 4;
}

Device_4Impl::Device_4Impl(DeviceClass *device_class,
	           	   const char *dev_name,
			   const char *desc,
	           	   Tango::DevState dev_state,
	           	   const char *dev_status):
Device_3Impl(device_class,dev_name,desc,dev_state,dev_status),ext_4(nullptr)
{
	idl_version = 4;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_4Impl::read_attribute_history_4
//
// description : 	CORBA operation to read attribute value history from
//			the polling buffer.
//
// argument: in :	- name : attribute name
//			- n : history depth (in record number)
//
// This method returns a pointer to a DevAttrHistory_4 structure
//
//--------------------------------------------------------------------------

Tango::DevAttrHistory_4 *Device_4Impl::read_attribute_history_4(const char* name,CORBA::Long n)
{
	TangoMonitor &mon = get_poll_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_4Impl::read_attribute_history_4 arrived, requested history depth = " << n << std::endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Read_Attr_history_4);

	Tango::DevAttrHistory_4 *back = NULL;
	std::vector<PollObj *> &poll_list = get_poll_obj_list();
	long nb_poll = poll_list.size();

//
// Check that the device supports this attribute. This method returns an
// exception in case of unsupported attribute
//

	Attribute &att = dev_attr->get_attr_by_name(name);

	std::string attr_str(name);
	std::transform(attr_str.begin(),attr_str.end(),attr_str.begin(),::tolower);

//
// Check that the wanted attribute is polled
//

	long j;
	PollObj *polled_attr = NULL;
	for (j = 0;j < nb_poll;j++)
	{
		if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
			(poll_list[j]->get_name() == attr_str))
		{
			polled_attr = poll_list[j];
			break;
		}
	}
	if (polled_attr == NULL)
	{
		TangoSys_OMemStream o;
		o << "Attribute " << attr_str << " not polled" << std::ends;
		Except::throw_exception(API_AttrNotPolled,o.str(),"Device_4Impl::read_attribute_history_4");
	}

//
// Check that some data is available in cache
//

	if (polled_attr->is_ring_empty() == true)
	{
		TangoSys_OMemStream o;
		o << "No data available in cache for attribute " << attr_str << std::ends;
		Except::throw_exception(API_NoDataYet,o.str(),"Device_4Impl::read_attribute_history_4");
	}

//
// Set the number of returned records
//

	long in_buf = polled_attr->get_elt_nb_in_buffer();
	if (n > in_buf)
		n = in_buf;

//
// Allocate memory for the returned value
//

	try
	{
		back = new Tango::DevAttrHistory_4;
		back->dates.length(n);
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception(API_MemoryAllocation,
							"Can't allocate memory in server",
							"Device_4Impl::read_attribute_history_4");
	}

//
// Init attribute name in the returned structure
//

	back->name = Tango::string_dup(name);

//
// Get attribute value history
// Trick: To identify the state used as an attribute from a classical attribute with type
// DEV_STATE, use DEV_VOID for state as data type.
//

	if (att.get_name_lower() == "state")
		polled_attr->get_attr_history(n,back,Tango::DEV_VOID,att.get_data_format());
	else
		polled_attr->get_attr_history(n,back,att.get_data_type(),att.get_data_format());

	cout4 << "Leaving Device_4Impl::read_attribute_history_4 method" << std::endl;
	return back;

}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::command_inout_history_4
//
// description : 	CORBA operation to read command result history from
//			the polling buffer.
//
// argument: in :	- command : command name
//			- n : history depth (in record number)
//
// This method returns a pointer to a DevCmdHistoryList with one
// DevCmdHistory structure for each command record
//
//--------------------------------------------------------------------------

Tango::DevCmdHistory_4 *Device_4Impl::command_inout_history_4(const char* command,CORBA::Long n)
{
	TangoMonitor &mon = get_poll_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_4Impl::command_inout_history_4 arrived" << std::endl;
	Tango::DevCmdHistory_4 *back = NULL;

	std::string cmd_str(command);

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command_inout_history_4);

//
// Check that device supports this command. Also checks if the device
// implements IDL >=3 when state or status history is requested
//

	std::transform(cmd_str.begin(),cmd_str.end(),cmd_str.begin(),::tolower);
	check_command_exists(cmd_str);

	bool status_cmd = false;
	bool state_cmd = false;

	long vers = get_dev_idl_version();

	if (vers >= 3)
	{
		if (cmd_str == "state")
			state_cmd = true;
		else if (cmd_str == "status")
			status_cmd = true;
	}

//
// Check that the command is polled
//

	PollObj *polled_cmd = NULL;
	std::vector<PollObj *> &poll_list = get_poll_obj_list();
	unsigned long i;
	for (i = 0;i < poll_list.size();i++)
	{
		if (poll_list[i]->get_name() == cmd_str)
		{
			if ((state_cmd == true) || (status_cmd == true))
			{
				if (poll_list[i]->get_type() == Tango::POLL_ATTR)
				{
					polled_cmd = poll_list[i];
				}
			}
			else
			{
				if (poll_list[i]->get_type() == Tango::POLL_CMD)
				{
					polled_cmd = poll_list[i];
				}
			}
		}
	}

	if (polled_cmd == NULL)
	{
		TangoSys_OMemStream o;
		o << "Command " << cmd_str << " not polled" << std::ends;
		Except::throw_exception((const char *)API_CmdNotPolled,
					o.str(),
					(const char *)"Device_4Impl::command_inout_history_4");
	}

//
// Check that some data is available in cache
//

	if (polled_cmd->is_ring_empty() == true)
	{
		TangoSys_OMemStream o;
		o << "No data available in cache for command " << cmd_str << std::ends;
		Except::throw_exception((const char *)API_NoDataYet,
					o.str(),
					(const char *)"Device_4Impl::command_inout_history_4");
	}

//
// Set the number of returned records
//

	long in_buf = polled_cmd->get_elt_nb_in_buffer();
	if (n > in_buf)
		n = in_buf;

//
// Allocate memory for the returned value
//

	try
	{
		back = new Tango::DevCmdHistory_4;
		back->dates.length(n);
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_4Impl::command_inout_history_4");
	}

//
// Get command result history
// Warning : Starting with Tango V5 (IDL 3), state and status are polled
// as attributes but could also be retrieved as commands. In this case,
// retrieved the history as attributes and transfer this as command
//

	if ((state_cmd == true) || (status_cmd == true))
	{
		Tango::DevAttrHistory_4 *back_attr = NULL;
		try
		{
			back_attr = new Tango::DevAttrHistory_4;
			back_attr->dates.length(n);
		}
		catch (std::bad_alloc &)
		{
			Except::throw_exception((const char *)API_MemoryAllocation,
				        	(const char *)"Can't allocate memory in server",
				        	(const char *)"Device_4Impl::command_inout_history_4");
		}

		if (status_cmd == true)
		{
			polled_cmd->get_attr_history(n,back_attr,Tango::DEV_STRING,SCALAR);

			back->dates = back_attr->dates;
			back->errors = back_attr->errors;
			back->errors_array = back_attr->errors_array;
			back->dims = back_attr->r_dims;
			back->dims_array = back_attr->r_dims_array;
			back->cmd_type = Tango::DEV_STRING;
			back->value = back_attr->value;
		}
		else
		{

//
// Trick: To identify the state used as an attribute from a classical attribute with type
// DEV_STATE, use DEV_VOID for state as data type.
//

			polled_cmd->get_attr_history(n,back_attr,Tango::DEV_VOID,SCALAR);

			back->dates = back_attr->dates;
			back->errors = back_attr->errors;
			back->errors_array = back_attr->errors_array;
			back->dims = back_attr->r_dims;
			back->dims_array = back_attr->r_dims_array;
			back->cmd_type = Tango::DEV_STATE;
			back->value = back_attr->value;
		}
		delete back_attr;
	}
	else
	{
//
// Get command output data type and fill returned structure
//

		std::string cmd(command);
		std::transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);

		Tango::CmdArgType cmd_type;
		std::vector<Tango::Command *> &cmd_list = device_class->get_command_list();
		for (unsigned int loop = 0;loop < cmd_list.size();loop++)
		{
			std::string cmd_name(cmd_list[loop]->get_name());
			std::transform(cmd_name.begin(),cmd_name.end(),cmd_name.begin(),::tolower);
			if (cmd_name == cmd)
			{
				cmd_type = cmd_list[loop]->get_out_type();
				break;
			}
		}
		polled_cmd->get_cmd_history(n,back,cmd_type);
	}

	cout4 << "Leaving Device_4Impl::command_inout_history_4 method" << std::endl;
	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_4Impl::command_inout_4
//
// description : 	Method called for each command_inout operation executed
//			from any client on a Tango device version 4.
//
//--------------------------------------------------------------------------


CORBA::Any *Device_4Impl::command_inout_4(const char *in_cmd,
					  const CORBA::Any &in_data,
					  Tango::DevSource source,
					  const Tango::ClntIdent &cl_id)
{
	cout4 << "Device_4Impl::command_inout_4 arrived, source = " << source << ", command = " << in_cmd << std::endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_cmd_cl_ident(in_cmd,cl_id,4,source);

//
// Do not check lock validity if State or Status is requested using command
//

	bool state_status_cmd = false;
	if ((TG_strcasecmp(in_cmd,"state") == 0) || (TG_strcasecmp(in_cmd,"status") == 0))
		state_status_cmd = true;

//
// Check if the device is locked and if it is valid
// If the lock is not valid any more, clear it
//

	if (state_status_cmd == false)
	{
		check_lock("command_inout4",in_cmd);
	}

//
// Call the Device_2Impl command_inout
//

	store_in_bb = false;
	return (command_inout_2(in_cmd,in_data,source));

}

//+-------------------------------------------------------------------------
//
// method : 		Device_4Impl::read_attributes_4
//
// description : 	Method called for each read_attributes operation executed
//			from any client on a Tango device version 4.
//
//--------------------------------------------------------------------------

Tango::AttributeValueList_4* Device_4Impl::read_attributes_4(const Tango::DevVarStringArray& names,
					     Tango::DevSource source,const Tango::ClntIdent &cl_id)
{
	cout4 << "Device_4Impl::read_attributes_4 arrived for dev " << get_name() << ", att[0] = " << names[0] << std::endl;

//
// Record operation request in black box
//

	if (store_in_bb == true)
		blackbox_ptr->insert_attr(names,cl_id,4,source);
	store_in_bb = true;

//
// Build a sequence with the names of the attribute to be read.
// This is necessary in case of the "AllAttr" shortcut is used
// If all attributes are wanted, build this list
//

	unsigned long nb_names = names.length();
	unsigned long nb_dev_attr = dev_attr->get_attr_nb();
	Tango::DevVarStringArray real_names(nb_names);
	unsigned long i;

	if (nb_names == 1)
	{
		std::string att_name(names[0]);
		if (att_name == AllAttr)
		{
			real_names.length(nb_dev_attr);
			for (i = 0;i < nb_dev_attr;i++)
			{
				real_names[i] = dev_attr->get_attr_by_ind(i).get_name().c_str();
			}
		}
		else
		{
			real_names = names;
		}
	}
	else
	{
		real_names = names;
	}
	nb_names = real_names.length();

//
// Allocate memory for the AttributeValue structures
//

	AttributeIdlData aid;

	try
	{
		Tango::AttributeValue_4 *l_back = new Tango::AttributeValue_4 [nb_names];
		aid.data_4 = new Tango::AttributeValueList_4(nb_names,nb_names,l_back,true);

		for (unsigned long loop = 0;loop < nb_names;loop++)
			(*aid.data_4)[loop].value.union_no_data(true);
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_4Impl::read_attributes_4");
	}

//
// Store source parameter (used in case of forwarded attribute(s))
//

	set_call_source(source);

//
// If the source parameter specifies device, call the read_attributes method
// which does not throw exception except for major fault (cant allocate
// memory,....)
//

	std::vector <long> idx_in_back;

	if (source == Tango::DEV)
	{
		try
		{
			AutoTangoMonitor sync(this);
			read_attributes_no_except(real_names, aid);
		}
		catch (...)
		{
			delete aid.data_4;
			throw;
		}
	}
	else if (source == Tango::CACHE)
	{

//
// If the device has some forwarded attribute, do not try to get data from the local cache but get it
// from the root device
// Create two arrays with local attribute names and fwd attribute names
//

		bool fwd_att_in_call = false;
		Tango::DevVarStringArray local_names(nb_names);
		Tango::DevVarStringArray fwd_names(nb_names);

		if (with_fwd_att == true)
		{
			for (size_t loop = 0;loop < nb_names;loop++)
			{
				Attribute &att = dev_attr->get_attr_by_name(real_names[loop]);

				if (att.is_fwd_att() == true)
				{
					size_t nb_fwd = 0;
					fwd_att_in_call = true;
					nb_fwd++;
					fwd_names.length(nb_fwd);
					fwd_names[nb_fwd - 1] = real_names[loop];
					idx_in_back.push_back(loop);
				}
				else
				{
					size_t nb_local = 0;
					nb_local++;
					local_names.length(nb_local);
					local_names[nb_local - 1] = real_names[loop];
				}
			}
		}

		if (fwd_att_in_call == false)
		{

//
// No fwd attributes in call, get values from local cache
//

			try
			{
				TangoMonitor &mon = get_poll_monitor();
				AutoTangoMonitor sync(&mon);
				read_attributes_from_cache(real_names,aid);
			}
			catch (...)
			{
				delete aid.data_4;
				throw;
			}
		}
		else
		{

//
// Some fwd attributes in call: First get data for local attributes then get data for forwarded attributes from
// root devices
//

			try
			{
				TangoMonitor &mon = get_poll_monitor();
				{
					AutoTangoMonitor sync(&mon);
					read_attributes_from_cache(local_names,aid);
				}

				{
					AutoTangoMonitor sync(this);
					read_attributes_no_except(fwd_names, aid, idx_in_back);
					idx_in_back.clear();
				}
			}
			catch (...)
			{
				delete aid.data_4;
				throw;
			}
		}
	}
	else
	{

//
// It must be now CACHE_DEVICE (no other choice), first try to get
// values from cache
//

		try
		{
			TangoMonitor &mon = get_poll_monitor();
			AutoTangoMonitor sync(&mon);
			read_attributes_from_cache(real_names,aid);
		}
		catch (...)
		{
			delete aid.data_4;
			throw;
		}

//
// Now, build the list of attributes which it was not possible
// to get their value from cache
//

		Tango::DevVarStringArray names_from_device(nb_names);
		long nb_attr = 0;

       	for (i = 0;i < nb_names;i++)
       	{
       		long nb_err = (*aid.data_4)[i].err_list.length();
       		if (nb_err != 0)
       		{
       			nb_err--;
       			if ((strcmp((*aid.data_4)[i].err_list[nb_err].reason,API_AttrNotPolled) == 0) ||
       					(strcmp((*aid.data_4)[i].err_list[nb_err].reason,API_NoDataYet) == 0) ||
       					(strcmp((*aid.data_4)[i].err_list[nb_err].reason,API_NotUpdatedAnyMore) == 0) ||
       					(strcmp((*aid.data_4)[i].err_list[nb_err].origin,"DServer::add_obj_polling") == 0))
       			{
       				nb_attr++;
       				names_from_device.length(nb_attr);
       				names_from_device[nb_attr - 1] = real_names[i];
       				idx_in_back.push_back(i);

       				(*aid.data_4)[i].err_list.length(0);
       			}
       		}
       	}

		if (nb_attr != 0)
		{

//
// Try to get their values from device
//

			try
			{
				AutoTangoMonitor sync(this);
				read_attributes_no_except(names_from_device, aid, idx_in_back);
			}
			catch (...)
			{
				delete aid.data_4;
				throw;
			}
		}
	}

	return aid.data_4;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_4Impl::write_attributes_4
//
// description : 	CORBA operation to write attribute(s) value
//
// argument: in :	- values: The new attribute(s) value to be set.
//
//--------------------------------------------------------------------------

void Device_4Impl::write_attributes_4(const Tango::AttributeValueList_4 & values,
									  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_4Impl::write_attributes_4 arrived" << std::endl;

//
// Record operation request in black box
//

	if (store_in_bb == true)
		blackbox_ptr->insert_attr(values,cl_id,4);
	store_in_bb = true;

//
// Check if the device is locked and by who
//

	check_lock("write_attributes_4");

//
// Call the Device_3Impl write_attributes
//

	return write_attributes_34(NULL,&values);
}


//+-------------------------------------------------------------------------
//
// method :		Device_4Impl::set_attribute_config_4
//
// description :	CORBA operation to set attribute configuration locally
//			and in the Tango database
//
// argument: in :	- new_conf: The new attribute(s) configuration. One
//				    AttributeConfig structure is needed for each
//				    attribute to update
//
//--------------------------------------------------------------------------

void Device_4Impl::set_attribute_config_4(const Tango::AttributeConfigList_3& new_conf,
										  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_4Impl::set_attribute_config_4 arrived" << std::endl;

//
// The attribute conf. is protected by two monitors. One protects access between
// get and set attribute conf. The second one protects access between set and
// usage. This is the classical device monitor
//

	TangoMonitor &mon1 = get_att_conf_monitor();
	AutoTangoMonitor sync1(&mon1);

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Set_Attr_Config_4,cl_id);

//
// Check if the device is locked and by who
//

	check_lock("set_attribute_config_4");

//
// Call the Device_3Impl set_attribute_config
//

	store_in_bb = false;
	return set_attribute_config_3_local(new_conf,new_conf[0],false,4);
}

//+-------------------------------------------------------------------------
//
// method : 		Device_4Impl::write_read_attributes_4
//
// description : 	CORBA operation to write then read attribute(s) value
//					Nowadays, it is possible to write then read attribute for
//					only 1 attribute at a time even if the IDL is written for
//					several attribute
//
// argument: in :	- values: The new attributes value to be set.
//
//--------------------------------------------------------------------------

Tango::AttributeValueList_4* Device_4Impl::write_read_attributes_4(const Tango::AttributeValueList_4& values,
									  const Tango::ClntIdent &cl_id)
{
	AutoTangoMonitor sync(this,true);
	cout4 << "Device_4Impl::write_read_attributes_4 arrived" << std::endl;

//
// Record operation request in black box
//

	Tango::DevVarStringArray dvsa;
	dvsa.length(1);
	dvsa[0] = Tango::string_dup(values[0].name);

	blackbox_ptr->insert_wr_attr(values,dvsa,cl_id,4);

//
// Check if the device is locked and by who
//

	check_lock("write_read_attributes_4");

//
// Check the attribute write type (only READ_WRITE or READ_WITH_WRITE allowed)
//

	Tango::Attribute &att = dev_attr->get_attr_by_name(values[0].name);
	Tango::AttrWriteType awt = att.get_writable();
	if ((awt == Tango::READ) || (awt == Tango::WRITE))
	{
		TangoSys_OMemStream o;
		o << "Attribute " << values[0].name << " is not a READ_WRITE or READ_WITH_WRITE attribute" << std::ends;

		Except::throw_exception((const char *)API_AttrNotWritable,o.str(),
								(const char *)"Device_4Impl::write_read_attribute_4");
	}

	Tango::AttributeValueList_4 *read_val_ptr;

//
// First, write the attribute
//

	store_in_bb = false;
	write_attributes_4(values,cl_id);

//
// Now, read the attribute
//

	Tango::DevVarStringArray att_name(1);
	att_name.length(1);
	att_name[0] = Tango::string_dup(values[0].name);
	Tango::ClntIdent dummy_cl_id;
	Tango::CppClntIdent cci = 0;
	dummy_cl_id.cpp_clnt(cci);

	store_in_bb = false;
	read_val_ptr = read_attributes_4(att_name,Tango::DEV,dummy_cl_id);

	return read_val_ptr;
}


} // End of Tango namespace
