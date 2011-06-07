static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               Device_2.cpp
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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <device_2.h>
#include <new>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */


namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::Device_2Impl
//
// description : 	constructors for the device_impl class from the
//			class object pointer, the device name,
//			the description field, the state and the status.
//			Device_2Impl inherits from DeviceImpl. These constructors
//			simply call the correct DeviceImpl class
//			constructor
//
//--------------------------------------------------------------------------

Device_2Impl::Device_2Impl(DeviceClass *device_class,string &dev_name):
DeviceImpl(device_class,dev_name),ext_2(NULL)
{
	ext->idl_version = 2;
}

Device_2Impl::Device_2Impl(DeviceClass *device_class,
			   string &dev_name,
			   string &desc):
DeviceImpl(device_class,dev_name,desc),ext_2(NULL)
{
	ext->idl_version = 2;
}

Device_2Impl::Device_2Impl(DeviceClass *device_class,
	           	   string &dev_name,string &desc,
	           	   Tango::DevState dev_state,string &dev_status):
DeviceImpl(device_class,dev_name,desc,dev_state,dev_status),ext_2(NULL)
{
	ext->idl_version = 2;
}

Device_2Impl::Device_2Impl(DeviceClass *device_class,
	           	   const char *dev_name,
			   const char *desc,
	           	   Tango::DevState dev_state,
	           	   const char *dev_status):
DeviceImpl(device_class,dev_name,desc,dev_state,dev_status),ext_2(NULL)
{
	ext->idl_version = 2;
}



//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::command_inout_2
//
// description : 	Method called for each command_inout operation executed
//			from any client on a Tango device version 2.
//
//--------------------------------------------------------------------------


CORBA::Any *Device_2Impl::command_inout_2(const char *in_cmd,
					  const CORBA::Any &in_data,
					  Tango::DevSource source)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "Device_2Impl::command_inout_2 arrived, source = " << source << ", command = " << in_cmd << endl;
	PollObj *polled_cmd = NULL;
	bool polling_failed = false;
	CORBA::Any *ret = NULL;

//
// Record operation request in black box
// If this method is executed with the request to store info in
// blackbox (store_in_bb == true), this means that the request arrives
// through a Device_2 CORBA interface. Check locking feature in this
// case. Otherwise the request has arrived through Device_4 and the check
// is already done
//

	if (ext->store_in_bb == true)
	{
		blackbox_ptr->insert_cmd(in_cmd,2,source);

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
			check_lock("command_inout2",in_cmd);
		}
	}
	ext->store_in_bb = true;

//
// If the source parameter specifies device, call the command_inout method
// implemented by the DeviceImpl class
//

	if (source == Tango::DEV)
	{
		AutoTangoMonitor sync(this);
		ext->store_in_bb = false;
		return command_inout(in_cmd,in_data);
	}
	else
	{
		bool status_cmd = false;
		bool state_cmd = false;

		TangoMonitor &p_mon = get_poll_monitor();
		AutoTangoMonitor sync(&p_mon);
		try
		{
			string cmd_str(in_cmd);
			transform(cmd_str.begin(),cmd_str.end(),cmd_str.begin(),::tolower);

//
// Check that device supports this command and also check
// if it is state or status
//

			check_command_exists(cmd_str);

			long vers = get_dev_idl_version();

			if (vers >= 3)
			{
				if (cmd_str == "state")
					state_cmd = true;
				else if (cmd_str == "status")
					status_cmd = true;
			}

//
// Check that the command is polled.
// Warning : Since IDL 3 (Tango V5), state and status are polled as attributes
//

			bool found = false;
			vector<PollObj *> &poll_list = get_poll_obj_list();
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
							found = true;
						}
					}
					else
					{
						if (poll_list[i]->get_type() == Tango::POLL_CMD)
						{
							polled_cmd = poll_list[i];
							found = true;
						}
					}
				}
			}

//
// Throw exception if the command is not polled
//

			if (found == false)
			{
                TangoSys_OMemStream o;
                o << "Command " << in_cmd << " not polled" << ends;
                Except::throw_exception((const char *)"API_CmdNotPolled",
							o.str(),
							(const char *)"Device_2Impl::command_inout");
			}

/*

//
// If the command is not polled but its polling update period is defined,
// and the command is not in the device list of command which should not be
// polled, start to poll it
//

                else
				{
					found = false;
					vector<string> &napc = get_non_auto_polled_cmd();
					for (i = 0;i < napc.size();i++)
					{
						if (napc[i] == cmd_str)
							found = true;
					}

					if (found == true)
					{
						TangoSys_OMemStream o;
						o << "Command " << in_cmd << " not polled" << ends;
						Except::throw_exception((const char *)"API_CmdNotPolled",
								o.str(),
								(const char *)"Device_2Impl::command_inout");
					}
					else
					{
						Tango::Util *tg = Tango::Util::instance();
						DServer *adm_dev = tg->get_dserver_device();

						DevVarLongStringArray *send = new DevVarLongStringArray();
						send->lvalue.length(1);
						send->svalue.length(3);

						send->lvalue[0] = poll_period;
						send->svalue[0] = device_name.c_str();
						send->svalue[1] = CORBA::string_dup("command");
						send->svalue[2] = in_cmd;

						get_poll_monitor().rel_monitor();
						adm_dev->add_obj_polling(send,false);

						delete send;

//
// Wait for first polling. Release monitor during this sleep in order
// to give it to the polling thread
//

#ifdef _TG_WINDOWS_
						Sleep((DWORD)600);
						get_poll_monitor().get_monitor();
#else
						struct timespec to_wait,inter;
						to_wait.tv_sec = 0;
						to_wait.tv_nsec = 600000000;
						nanosleep(&to_wait,&inter);
						get_poll_monitor().get_monitor();
#endif

//
// Update the polled-cmd pointer to the new polled object
//

						for (i = 0;i < poll_list.size();i++)
						{
							if (poll_list[i]->get_name() == cmd_str)
							{
								polled_cmd = poll_list[i];
								break;
							}
						}
					}
				}
			}*/

//
// Check that some data is available in cache
//

			if (polled_cmd->is_ring_empty() == true)
			{
				TangoSys_OMemStream o;
				o << "No data available in cache for command " << in_cmd << ends;
				Except::throw_exception((const char *)"API_NoDataYet",
						o.str(),
						(const char *)"Device_2Impl::command_inout");
			}

//
// Check that data are still refreshed by the polling thread
// Skip this test for object with external polling triggering (upd = 0)
//

			long tmp_upd = polled_cmd->get_upd();
			if (tmp_upd != 0)
			{
				double last = polled_cmd->get_last_insert_date();
				struct timeval now;
#ifdef _TG_WINDOWS_
				struct _timeb now_win;
				_ftime(&now_win);
				now.tv_sec = (unsigned long)now_win.time;
				now.tv_usec = (long)now_win.millitm * 1000;
#else
				gettimeofday(&now,NULL);
#endif
				now.tv_sec = now.tv_sec - DELTA_T;
				double now_d = (double)now.tv_sec + ((double)now.tv_usec / 1000000);
				double diff_d = now_d - last;
				if (diff_d > polled_cmd->get_authorized_delta())
				{
					TangoSys_OMemStream o;
					o << "Data in cache for command " << in_cmd;
					o << " not updated any more" << ends;
					Except::throw_exception((const char *)"API_NotUpdatedAnyMore",
						o.str(),
						(const char *)"Device_2Impl::command_inout");
				}
			}
		}
		catch (Tango::DevFailed &)
		{
			if (source == Tango::CACHE)
			{
				throw;
			}
			polling_failed = true;
		}

//
// Get cmd result (or exception)
//

		if (source == Tango::CACHE)
		{
			cout4 << "Device_2Impl: Returning data from polling buffer" << endl;
			if ((state_cmd == true) || (status_cmd == true))
			{
				omni_mutex_lock sync(*polled_cmd);
				if (get_dev_idl_version() < 4)
				{
					Tango::AttributeValue_3 &att_val = polled_cmd->get_last_attr_value_3(false);
					ret = attr2cmd(att_val,state_cmd,status_cmd);
				}
				else
				{
					Tango::AttributeValue_4 &att_val = polled_cmd->get_last_attr_value_4(false);
					ret = attr2cmd(att_val,state_cmd,status_cmd);
				}
			}
			else
				ret = polled_cmd->get_last_cmd_result();
		}
		else
		{
			if (polling_failed == false)
			{
				cout4 << "Device_2Impl: Returning data from polling buffer" << endl;
				if ((state_cmd == true) || (status_cmd == true))
				{
					omni_mutex_lock sync(*polled_cmd);
					if (get_dev_idl_version() < 4)
					{
						Tango::AttributeValue_3 &att_val = polled_cmd->get_last_attr_value_3(false);
						ret = attr2cmd(att_val,state_cmd,status_cmd);
					}
					else
					{
						Tango::AttributeValue_4 &att_val = polled_cmd->get_last_attr_value_4(false);
						ret = attr2cmd(att_val,state_cmd,status_cmd);
					}
				}
				else
					ret = polled_cmd->get_last_cmd_result();
			}
		}

	}

	if ((source != Tango::CACHE) && (polling_failed == true))
	{
		AutoTangoMonitor sync(this);
		ext->store_in_bb = false;
		ret = command_inout(in_cmd,in_data);
	}

	return ret;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::read_attributes_2
//
// description : 	Method called for each read_attributes operation executed
//			from any client on a Tango device version 2.
//
//--------------------------------------------------------------------------

Tango::AttributeValueList* Device_2Impl::read_attributes_2(const Tango::DevVarStringArray& names,
					     Tango::DevSource source)
throw (Tango::DevFailed, CORBA::SystemException)
{
//	AutoTangoMonitor sync(this);

	cout4 << "Device_2Impl::read_attributes_2 arrived" << endl;

	bool att_in_fault = false;
	bool polling_failed = false;
	Tango::AttributeValueList *back = NULL;


//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//
	SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
	string last_associated_device = sub.get_associated_device();
	sub.set_associated_device(get_name());

// Catch all execeptions to set back the associated device after
// execution
	try
	{

//
// Record operation request in black box
//

		blackbox_ptr->insert_attr(names,2,source);

//
// If the source parameter specifies device, call the read_attributes method
// implemented by the DeviceImpl class
//

		if (source == Tango::DEV)
		{
			AutoTangoMonitor sync(this,true);
			ext->store_in_bb = false;
			return read_attributes(names);
		}
		else
		{

			TangoMonitor &p_mon = get_poll_monitor();
			AutoTangoMonitor sync(&p_mon);

			try
			{

//
// Build a sequence with the names of the attribute to be read.
// This is necessary in case of the "AllAttr" shortcut is used
// If all attributes are wanted, build this list
//

				unsigned long i,j;
				bool all_attr = false;
				vector<PollObj *> &poll_list = get_poll_obj_list();
				unsigned long nb_poll = poll_list.size();
				unsigned long nb_names = names.length();
				Tango::DevVarStringArray real_names(nb_names);
				if (nb_names == 1)
				{
					string att_name(names[0]);
					if (att_name == AllAttr)
					{
						all_attr = true;
						for (i = 0;i < nb_poll;i++)
						{
							if (poll_list[i]->get_type() == Tango::POLL_ATTR)
							{
								long  nb_in_seq = real_names.length();
								real_names.length(nb_in_seq + 1);
								real_names[nb_in_seq] = poll_list[i]->get_name().c_str();
							}
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

//
// Check that device supports the wanted attribute
//

				if (all_attr == false)
				{
					for (i = 0;i < real_names.length();i++)
						dev_attr->get_attr_ind_by_name(real_names[i]);
				}

//
// Check that all wanted attributes are polled. If some are non polled, store
// their index in the real_names sequence in a vector
//

				vector<long> non_polled;
				if (all_attr == false)
				{
					for (i = 0;i < real_names.length();i++)
					{
						for (j = 0;j < nb_poll;j++)
						{
#ifdef _TG_WINDOWS_
							if (_stricmp(poll_list[j]->get_name().c_str(),real_names[i]) == 0)
#else
							if (strcasecmp(poll_list[j]->get_name().c_str(),real_names[i]) == 0)
#endif
								break;
						}
						if (j == nb_poll)
						{
							non_polled.push_back(i);
						}
					}
				}

//
// If some attributes are not polled but their polling update period is defined,
// and the attribute is not in the device list of attr which should not be
// polled, start to poll them
//

				bool found;
				vector<long> poll_period;
				if (non_polled.size() != 0)
				{

//
// Check that it is possible to start polling for the non polled attribute
//

					for (i = 0;i < non_polled.size();i++)
					{
						Attribute &att = dev_attr->get_attr_by_name(real_names[non_polled[i]]);
						poll_period.push_back(att.get_polling_period());

						if (poll_period.back() == 0)
						{
							TangoSys_OMemStream o;
							o << "Attribute " << real_names[non_polled[i]] << " not polled" << ends;
							Except::throw_exception((const char *)"API_AttrNotPolled",
									o.str(),
									(const char *)"Device_2Impl::read_attributes");
						}
						else
						{
							found = false;
							vector<string> &napa = get_non_auto_polled_attr();
							for (j = 0;j < napa.size();j++)
							{
#ifdef _TG_WINDOWS_
								if (_stricmp(napa[j].c_str(),real_names[non_polled[i]]) == 0)
#else
								if (strcasecmp(napa[j].c_str(),real_names[non_polled[i]]) == 0)
#endif
									found = true;
							}

							if (found == true)
							{
								TangoSys_OMemStream o;
								o << "Attribute " << real_names[non_polled[i]] << " not polled" << ends;
								Except::throw_exception((const char *)"API_AttrNotPolled",
										o.str(),
										(const char *)"Device_2Impl::read_attributes");
							}
						}
					}

//
// Start polling
//

					Tango::Util *tg = Tango::Util::instance();
					DServer *adm_dev = tg->get_dserver_device();

					DevVarLongStringArray *send = new DevVarLongStringArray();
					send->lvalue.length(1);
					send->svalue.length(3);
					send->svalue[0] = device_name.c_str();
					send->svalue[1] = CORBA::string_dup("attribute");

					for (i = 0;i < non_polled.size();i++)
					{
						send->lvalue[0] = poll_period[i];
						send->svalue[2] = real_names[non_polled[i]];

						adm_dev->add_obj_polling(send,false);
					}

					delete send;

//
// Wait for first polling
//

#ifdef _TG_WINDOWS_
					get_poll_monitor().rel_monitor();
					Sleep((DWORD)600);
					get_poll_monitor().get_monitor();
#else
					struct timespec to_wait,inter;
					to_wait.tv_sec = 0;
					to_wait.tv_nsec = 600000000;
					get_poll_monitor().rel_monitor();
					nanosleep(&to_wait,&inter);
					get_poll_monitor().get_monitor();
#endif

				}

//
// Allocate memory for the AttributeValue structures
//

				unsigned long nb_attr = real_names.length();
				try
				{
					back = new Tango::AttributeValueList(nb_attr);
					back->length(nb_attr);
				}
				catch (bad_alloc)
				{
					back = NULL;
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        		(const char *)"Can't allocate memory in server",
				        		(const char *)"DeviceImpl_2::read_attributes");
				}

//
// For each attribute, check that some data are available in cache and that they
// are not too old
//

				for (i = 0;i < nb_attr;i++)
				{
					vector<PollObj *> &poll_list = get_poll_obj_list();
					PollObj *polled_attr = NULL;
					unsigned long j;
					for (j = 0;j < poll_list.size();j++)
					{
#ifdef _TG_WINDOWS_
						if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
			    	    	    	(_stricmp(poll_list[j]->get_name().c_str(),real_names[i]) == 0))
#else
						if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
			    	    	    	(strcasecmp(poll_list[j]->get_name().c_str(),real_names[i]) == 0))
#endif
						{
							polled_attr = poll_list[j];
							break;
						}
					}

//
// Check that some data is available in cache
//

					if (polled_attr->is_ring_empty() == true)
					{
						delete back;
						back = NULL;

						TangoSys_OMemStream o;
						o << "No data available in cache for attribute " << real_names[i] << ends;
						Except::throw_exception((const char *)"API_NoDataYet",
								o.str(),
								(const char *)"Device_2Impl::read_attributes");
					}

//
// Check that data are still refreshed by the polling thread
// Skip this test for object with external polling triggering (upd = 0)
//

					long tmp_upd = polled_attr->get_upd();
					if (tmp_upd != 0)
					{
						double last = polled_attr->get_last_insert_date();
						struct timeval now;
#ifdef _TG_WINDOWS_
						struct _timeb now_win;
						_ftime(&now_win);
						now.tv_sec = (unsigned long)now_win.time;
						now.tv_usec = (long)now_win.millitm;
#else
						gettimeofday(&now,NULL);
#endif
						now.tv_sec = now.tv_sec - DELTA_T;
						double now_d = (double)now.tv_sec + ((double)now.tv_usec / 1000000);
						double diff_d = now_d - last;
						if (diff_d > polled_attr->get_authorized_delta())
						{
							delete back;
							back = NULL;

							TangoSys_OMemStream o;
							o << "Data in cache for attribute " << real_names[i];
							o << " not updated any more" << ends;
							Except::throw_exception((const char *)"API_NotUpdatedAnyMore",
									o.str(),
									(const char *)"Device_2Impl::read_attributes");
						}
					}

//
// Get attribute data type
//

					Attribute &att = dev_attr->get_attr_by_name(real_names[i]);
					long type = att.get_data_type();

//
// In IDL release 3, possibility to write spectrum and
// images attributes have been added. This implies some
// changes in the struture returned for a read_attributes
// For server recompiled with this new release, the polling
// thread uses this new structure type to store attribute
// value. For some cases, it is possible to pass data from the
// new type to the old one, but for other cases, it is not
// possible. Throw exception in these cases
//

					Tango::AttrWriteType w_type = att.get_writable();
					Tango::AttrDataFormat format_type = att.get_data_format();
					if ((format_type == Tango::SPECTRUM) || (format_type == Tango::IMAGE))
					{
						if (w_type != Tango::READ)
						{
							delete back;
							back = NULL;

							TangoSys_OMemStream o;
							o << "Client too old to get data for attribute " << real_names[i].in();
							o << ".\nPlease, use a client linked with Tango V5";
							o << " and a device inheriting from Device_3Impl" << ends;
							Except::throw_exception((const char *)"API_NotSupportedFeature",
								o.str(),
								(const char *)"Device_2Impl::read_attributes");
						}
					}

//
// Finally, after all these checks, get value and store it in the sequence
// sent back to user
// In order to avoid unnecessary copy, don't use the assignement operator of the
// AttributeValue structure which copy each element and therefore also copy
// the Any object. The Any assignement operator is a deep copy!
// Create a new sequence using the attribute buffer and insert it into the
// Any. The sequence inside the source Any has been created using the attribute
// data buffer.
//

					try
					{
						{
							omni_mutex_lock sync(*polled_attr);

							long vers = get_dev_idl_version();
							if (vers >= 4)
							{
								AttributeValue_4 &att_val_4 = polled_attr->get_last_attr_value_4(false);
								if (att_val_4.quality != Tango::ATTR_INVALID)
								{
									if (type == Tango::DEV_ENCODED)
									{
										delete back;
										back = NULL;

										TangoSys_OMemStream o;
										o << "Data type for attribute " << real_names[i] << " is DEV_ENCODED.";
										o << " It's not possible to retrieve this data type through the interface you are using (IDL V2)" << ends;
										Except::throw_exception((const char *)"API_NotSupportedFeature",
												o.str(),
												(const char *)"Device_2Impl::read_attributes");
									}

									Polled_2_Live(type,att_val_4.value,(*back)[i].value);

									(*back)[i].quality= att_val_4.quality;
									(*back)[i].time = att_val_4.time;
									(*back)[i].dim_x = att_val_4.r_dim.dim_x;
									(*back)[i].dim_y = att_val_4.r_dim.dim_y;
									(*back)[i].name = CORBA::string_dup(att_val_4.name);
								}
							}
							else if (vers == 3)
							{
								AttributeValue_3 &att_val_3 = polled_attr->get_last_attr_value_3(false);
								if (att_val_3.quality != Tango::ATTR_INVALID)
								{
									if (type == Tango::DEV_ENCODED)
									{
										delete back;
										back = NULL;

										TangoSys_OMemStream o;
										o << "Data type for attribute " << real_names[i] << " is DEV_ENCODED.";
										o << " It's not possible to retrieve this data type through the interface you are using (IDL V2)" << ends;
										Except::throw_exception((const char *)"API_NotSupportedFeature",
												o.str(),
												(const char *)"Device_2Impl::read_attributes");
									}

									Polled_2_Live(type,att_val_3.value,(*back)[i].value);

									(*back)[i].quality= att_val_3.quality;
									(*back)[i].time = att_val_3.time;
									(*back)[i].dim_x = att_val_3.r_dim.dim_x;
									(*back)[i].dim_y = att_val_3.r_dim.dim_y;
									(*back)[i].name = CORBA::string_dup(att_val_3.name);
								}
							}
							else
							{
								AttributeValue &att_val = polled_attr->get_last_attr_value(false);

								if (att_val.quality != Tango::ATTR_INVALID)
								{
									if (type == Tango::DEV_ENCODED)
									{
										delete back;
										back = NULL;

										TangoSys_OMemStream o;
										o << "Data type for attribute " << real_names[i] << " is DEV_ENCODED.";
										o << " It's not possible to retrieve this data type through the interface you are using (IDL V2)" << ends;
										Except::throw_exception((const char *)"API_NotSupportedFeature",
												o.str(),
												(const char *)"Device_2Impl::read_attributes");
									}

									Polled_2_Live(type,att_val.value,(*back)[i].value);

									(*back)[i].quality= att_val.quality;
									(*back)[i].time = att_val.time;
									(*back)[i].dim_x = att_val.dim_x;
									(*back)[i].dim_y = att_val.dim_y;
									(*back)[i].name = CORBA::string_dup(att_val.name);
								}

							}
						}
					}
					catch (Tango::DevFailed &)
					{
						delete back;
						att_in_fault = true;
						throw;
					}
				}
			}
			catch (Tango::DevFailed &)
			{

//
// Re-throw exception if the source parameter is CACHE or if the source param.
// is CACHE_DEV but one of the attribute is really in fault (not the polling)
//

				if (source == Tango::CACHE)
					throw;
				if (att_in_fault == true)
					throw;
				polling_failed = true;
			}
		}

//
// If the source parameter is CACHE, returned data, else called method reading
// attribute from device
//

		if ((source == Tango::CACHE_DEV) && (polling_failed == true))
		{
			if (back != NULL)
				delete back;

			AutoTangoMonitor sync(this,true);
			ext->store_in_bb = false;
			//return read_attributes(names);
			back = read_attributes(names);
		}
		else
		{
			cout4 << "Device_2Impl: Returning attribute(s) value from polling buffer" << endl;
			//return back;
		}
	}
	catch (...)
	{
		// set back the device attribution for the thread
		// and rethrow the exception.
		sub.set_associated_device(last_associated_device);
		throw;
	}

	// set back the device attribution for the thread
	sub.set_associated_device(last_associated_device);

	return back;

}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::command_list_query
//
// description : 	CORBA operation to read the device command list.
//			This method returns command info in a sequence of
//			DevCmdInfo_2
//
//			WARNING !!!!!!!!!!!!!!!!!!
//
//			This is the release 2 of this CORBA operation which
//			returns one more parameter than in release 1
//			The code has been duplicated in order to keep it clean
//			(avoid many "if" on version number in a common method)
//
//--------------------------------------------------------------------------


Tango::DevCmdInfoList_2 *Device_2Impl::command_list_query_2()
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "Device_2Impl::command_list_query_2 arrived" << endl;

//
// Retrieve number of command and allocate memory to send back info
//

	long nb_cmd = device_class->get_command_list().size();
	cout4 << nb_cmd << " command(s) for device" << endl;
	Tango::DevCmdInfoList_2 *back;

	try
	{
		back = new Tango::DevCmdInfoList_2(nb_cmd);
                back->length(nb_cmd);

//
// Populate the vector
//

		for (long i = 0;i < nb_cmd;i++)
		{
			Tango::DevCmdInfo_2 tmp;
			tmp.cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
			tmp.cmd_tag = 0;
			tmp.level = (device_class->get_command_list())[i]->get_disp_level();
			tmp.in_type = (long)((device_class->get_command_list())[i]->get_in_type());
			tmp.out_type = (long)((device_class->get_command_list())[i]->get_out_type());
			string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
			if (str_in.size() != 0)
				tmp.in_type_desc = CORBA::string_dup(str_in.c_str());
			else
				tmp.in_type_desc = CORBA::string_dup(DescNotSet);
			string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
			if (str_out.size() != 0)
				tmp.out_type_desc = CORBA::string_dup(str_out.c_str());
			else
				tmp.out_type_desc = CORBA::string_dup(DescNotSet);

			(*back)[i] = tmp;
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_2Impl::command_list_query_2");
	}

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command_list_2);

//
// Return to caller
//

	cout4 << "Leaving Device_2Impl::command_list_query_2" << endl;
	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::command_query_2
//
// description : 	CORBA operation to read a device command info.
//			This method returns command info for a specific
//			command.
//
//			WARNING !!!!!!!!!!!!!!!!!!
//
//			This is the release 2 of this CORBA operation which
//			returns one more parameter than in release 1
//			The code has been duplicated in order to keep it clean
//			(avoid many "if" on version number in a common method)
//
//--------------------------------------------------------------------------

Tango::DevCmdInfo_2 *Device_2Impl::command_query_2(const char *command)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::command_query_2 arrived" << endl;

	Tango::DevCmdInfo_2 *back;
	string cmd(command);
	transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

	try
	{
		back = new Tango::DevCmdInfo_2();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_2Impl::command_query_2");
	}

//
// Try to retrieve the command in the command list
//

	long i;
	long nb_cmd = device_class->get_command_list().size();
	for (i = 0;i < nb_cmd;i++)
	{
		if (device_class->get_command_list()[i]->get_lower_name() == cmd)
		{
			back->cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
			back->cmd_tag = 0;
			back->level = (device_class->get_command_list())[i]->get_disp_level();
			back->in_type = (long)((device_class->get_command_list())[i]->get_in_type());
			back->out_type = (long)((device_class->get_command_list())[i]->get_out_type());
			string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
			if (str_in.size() != 0)
				back->in_type_desc = CORBA::string_dup(str_in.c_str());
			else
				back->in_type_desc = CORBA::string_dup(DescNotSet);
			string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
			if (str_out.size() != 0)
				back->out_type_desc = CORBA::string_dup(str_out.c_str());
			else
				back->out_type_desc = CORBA::string_dup(DescNotSet);
			break;
		}
	}

	if (i == nb_cmd)
	{
		delete back;
		cout3 << "Device_2Impl::command_query_2(): operation " << command << " not found" << endl;

//
// throw an exception to client
//

		TangoSys_OMemStream o;

		o << "Command " << command << " not found" << ends;
		Except::throw_exception((const char *)"API_CommandNotFound",
				        o.str(),
				        (const char *)"Device_2Impl::command_query_2");
	}

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command_2);

//
// Return to caller
//

	cout4 << "Leaving Device_2Impl::command_query_2" << endl;
	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::get_attribute_config_2
//
// description : 	CORBA operation to get attribute configuration.
//
// argument: in :	- names: name of attribute(s)
//
// This method returns a pointer to a AttributeConfigList_2 with one
// AttributeConfig_2 structure for each atribute
//
//
// WARNING !!!!!!!!!!!!!!!!!!
//
// This is the release 2 of this CORBA operation which
// returns one more parameter than in release 1
// The code has been duplicated in order to keep it clean
// (avoid many "if" on version number in a common method)
//
//--------------------------------------------------------------------------

Tango::AttributeConfigList_2 *Device_2Impl::get_attribute_config_2(const Tango::DevVarStringArray& names)
throw(Tango::DevFailed, CORBA::SystemException)
{
	TangoMonitor &mon = get_att_conf_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_2Impl::get_attribute_config_2 arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList_2 *back;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config_2);

//
// Get attribute number and device version
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	long vers = get_dev_idl_version();

//
// Check if the caller want to get config for all attribute
// If the device implements IDL 3 (State and status as attributes)
// and the client is an old one (not able to read state/status as
// attribute), decrement attribute number
//

	string in_name(names[0]);
	if (nb_attr == 1)
	{
		if (in_name == AllAttr)
		{
			all_attr = true;
			if (vers < 3)
				nb_attr = nb_dev_attr;
			else
				nb_attr = nb_dev_attr - 2;
		}
		else if (in_name == AllAttr_3)
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
		back = new Tango::AttributeConfigList_2(nb_attr);
		back->length(nb_attr);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_2Impl::get_attribute_config_2");
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
				attr.get_properties_2((*back)[i]);
			}
			else
			{
				Attribute &attr = dev_attr->get_attr_by_name(names[i]);
				attr.get_properties_2((*back)[i]);
			}
		}
		catch (Tango::DevFailed e)
		{
			delete back;
			throw;
		}
	}

//
// Return to caller
//

	cout4 << "Leaving Device_2Impl::get_attribute_config_2" << endl;

	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::command_inout_history_2
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

Tango::DevCmdHistoryList *Device_2Impl::command_inout_history_2(const char* command,
								CORBA::Long n)
throw(Tango::DevFailed, CORBA::SystemException)
{
	TangoMonitor &mon = get_poll_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_2Impl::command_inout_history_2 arrived" << endl;
	Tango::DevCmdHistoryList *back;

	string cmd_str(command);

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command_inout_history_2);

//
// Check that device supports this command. Also checks if the device
// implements IDL 3 (Tango V5) and state or status history is requested
//

	transform(cmd_str.begin(),cmd_str.end(),cmd_str.begin(),::tolower);
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
	vector<PollObj *> &poll_list = get_poll_obj_list();
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
		o << "Command " << cmd_str << " not polled" << ends;
		Except::throw_exception((const char *)"API_CmdNotPolled",
					o.str(),
					(const char *)"Device_2Impl::command_inout_history_2");
	}

//
// Check that some data is available in cache
//

	if (polled_cmd->is_ring_empty() == true)
	{
		TangoSys_OMemStream o;
		o << "No data available in cache for command " << cmd_str << ends;
		Except::throw_exception((const char *)"API_NoDataYet",
					o.str(),
					(const char *)"Device_2Impl::command_inout_history_2");
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
		back = new Tango::DevCmdHistoryList(n);
		back->length(n);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_2Impl::command_inout_history_2");
	}

//
// Get command result history
// Warning : Starting with Tango V5 (IDL 3), state and status are polled
// as attributes but could also be retrieved as commands. In this case,
// retrieved the history as attributes and transfer this as command
//

	if ((state_cmd == true) || (status_cmd == true))
	{
		Tango::DevAttrHistoryList_3 *back_attr;
		try
		{
			back_attr = new Tango::DevAttrHistoryList_3(n);
			back_attr->length(n);
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)"API_MemoryAllocation",
				        	(const char *)"Can't allocate memory in server",
				        	(const char *)"Device_2Impl::command_inout_history_2");
		}

		if (status_cmd == true)
		{
			if (vers >= 4)
				polled_cmd->get_attr_history_43(n,back_attr,Tango::DEV_STRING);
			else
				polled_cmd->get_attr_history(n,back_attr,Tango::DEV_STRING);

			for (int k = 0;k < n;k++)
			{
				(*back)[k].time = (*back_attr)[k].value.time;
				(*back)[k].cmd_failed = (*back_attr)[k].attr_failed;
				Tango::DevVarStringArray *dvsa;
				(*back_attr)[k].value.value >>= dvsa;
				(*back)[k].value <<= (*dvsa)[0];
				(*back)[k].errors = (*back_attr)[k].value.err_list;
			}
		}
		else
		{
			if (vers >= 4)
				polled_cmd->get_attr_history_43(n,back_attr,Tango::DEV_STATE);
			else
				polled_cmd->get_attr_history(n,back_attr,Tango::DEV_STATE);

			for (int k = 0;k < n;k++)
			{
				(*back)[k].time = (*back_attr)[k].value.time;
				(*back)[k].cmd_failed = (*back_attr)[k].attr_failed;
				(*back)[k].errors = (*back_attr)[k].value.err_list;
				if ((*back)[k].cmd_failed == false)
				{
					Tango::DevState sta;
					(*back_attr)[k].value.value >>= sta;
					(*back)[k].value <<= sta;
				}
			}
		}
		delete back_attr;
	}
	else
		polled_cmd->get_cmd_history(n,back);

	cout4 << "Leaving Device_2Impl::command_inout_history_2 method" << endl;
	return back;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::read_attribute_history_2
//
// description : 	CORBA operation to read attribute alue history from
//			the polling buffer.
//
// argument: in :	- name : attribute name
//			- n : history depth (in record number)
//
// This method returns a pointer to a DevAttrHistoryList with one
// DevAttrHistory structure for each attribute record
//
//--------------------------------------------------------------------------

Tango::DevAttrHistoryList *Device_2Impl::read_attribute_history_2(const char* name,
								  CORBA::Long n)
throw(Tango::DevFailed, CORBA::SystemException)
{
	TangoMonitor &mon = get_poll_monitor();
	AutoTangoMonitor sync(&mon);

	cout4 << "Device_2Impl::read_attribute_history_2 arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Read_Attr_history_2);

	long vers = get_dev_idl_version();
	Tango::DevAttrHistoryList *back;
	Tango::DevAttrHistoryList_3 *back_3;
	vector<PollObj *> &poll_list = get_poll_obj_list();
	long nb_poll = poll_list.size();

//
// Check that the device supports this attribute. This method returns an
// exception in case of unsupported attribute
//

	Attribute &att = dev_attr->get_attr_by_name(name);

	string attr_str(name);
	transform(attr_str.begin(),attr_str.end(),attr_str.begin(),::tolower);

//
// Check that the wanted attribute is polled.
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
		o << "Attribute " << attr_str << " not polled" << ends;
		Except::throw_exception((const char *)"API_AttrNotPolled",
					o.str(),
					(const char *)"Device_2Impl::read_attribute_history_2");
	}

//
// Check that some data is available in cache
//

	if (polled_attr->is_ring_empty() == true)
	{
		TangoSys_OMemStream o;
		o << "No data available in cache for attribute " << attr_str << ends;
		Except::throw_exception((const char *)"API_NoDataYet",
					o.str(),
					(const char *)"Device_2Impl::read_attribute_history_2");
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
		back = new Tango::DevAttrHistoryList(n);
                back->length(n);
		if (vers >= 3)
		{
			back_3 = new Tango::DevAttrHistoryList_3(n);
			back_3->length(n);
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Device_2Impl::read_attribute_history_2");
	}

//
// Get attribute value history
//

	if (vers < 3)
		polled_attr->get_attr_history(n,back,att.get_data_type());
	else
	{
		polled_attr->get_attr_history(n,back_3,att.get_data_type());
		Hist_32Hist(back_3,back);
		delete back_3;
	}

	cout4 << "Leaving Device_2Impl::command_inout_history_2 method" << endl;
	return back;
}


//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::attr2cmd
//
// description : 	Method to transfer attribute value into the same
//			kind of data used to transprt command result on
//			the network (A CORBA Any)
//
// Args (in) :		att_val : A reference to the complete attribute
//				  value
//			state : Boolean set to true if this method is called
//				for the state command
//			status : Boolean set to true if this method is called
//				 for the status command
//
//--------------------------------------------------------------------------

CORBA::Any *Device_2Impl::attr2cmd(AttributeValue_3 &att_val,bool state,bool status)
{
	CORBA::Any *any = new CORBA::Any();
	if (state == true)
	{
		Tango::DevState sta;
		att_val.value >>= sta;
		(*any) <<= sta;
	}
	else if (status == true)
	{
		Tango::DevVarStringArray *dvsa;
		att_val.value >>= dvsa;
		(*any) <<= (*dvsa)[0];
	}

	return any;
}

CORBA::Any *Device_2Impl::attr2cmd(AttributeValue_4 &att_val,bool state,bool status)
{
	CORBA::Any *any = new CORBA::Any();
	if (state == true)
	{
		const Tango::DevState &sta = att_val.value.dev_state_att();
		(*any) <<= sta;
	}
	else if (status == true)
	{
		Tango::DevVarStringArray &dvsa = att_val.value.string_att_value();
		(*any) <<= dvsa[0];
	}

	return any;
}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::Hist_32Hist
//
// description : 	Move data from a AttrHistory_3 sequence to an
//			AttrHistory sequence. Usefull for client linked
//			with V4 requested history from attribute belonging
//			to device linked with V5 and inheriting from
//			Device_3Impl
//
// Args (in) :		back_3 : Pointer to the Attr History 3 sequence
//			back : Pointer to the Attr History sequence
//
//--------------------------------------------------------------------------

void Device_2Impl::Hist_32Hist(DevAttrHistoryList_3 *back_3,DevAttrHistoryList *back)
{
	unsigned long nb = back_3->length();

	for (unsigned long i = 0;i < nb;i++)
	{
		(*back)[i].attr_failed = (*back_3)[i].attr_failed;
		(*back)[i].value.value = (*back_3)[i].value.value;
		(*back)[i].value.quality = (*back_3)[i].value.quality;
		(*back)[i].value.time = (*back_3)[i].value.time;
		(*back)[i].value.name = (*back_3)[i].value.name;
		(*back)[i].value.dim_x = (*back_3)[i].value.r_dim.dim_x;
		(*back)[i].value.dim_y = (*back_3)[i].value.r_dim.dim_y;
		(*back)[i].errors = (*back_3)[i].value.err_list;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Device_2Impl::Polled_2_Live
//
// description : 	Move data from a polled V4 device attribute to the Any
//					used in a V2 or V1 read_attribute request
//
// Args (in) :		data_type : Atrtribute data type
//					hist_union : Reference to the Union object within the history buffer
//					live_any : Reference to the Any object returned to the caller
//
//--------------------------------------------------------------------------

void Device_2Impl::Polled_2_Live(TANGO_UNUSED(long data_type),Tango::AttrValUnion &hist_union,CORBA::Any &live_any)
{
	switch (hist_union._d())
	{
		case ATT_BOOL:
		{
			DevVarBooleanArray &union_seq = hist_union.bool_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_SHORT:
		{
			DevVarShortArray &union_seq = hist_union.short_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_LONG:
		{
			DevVarLongArray &union_seq = hist_union.long_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_LONG64:
		{
			DevVarLong64Array &union_seq = hist_union.long64_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_FLOAT:
		{
			DevVarFloatArray &union_seq = hist_union.float_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_DOUBLE:
		{
			DevVarDoubleArray &union_seq = hist_union.double_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_UCHAR:
		{
			DevVarCharArray &union_seq = hist_union.uchar_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_USHORT:
		{
			DevVarUShortArray &union_seq = hist_union.ushort_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_ULONG:
		{
			DevVarULongArray &union_seq = hist_union.ulong_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_ULONG64:
		{
			DevVarULong64Array &union_seq = hist_union.ulong64_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_STRING:
		{
			const DevVarStringArray &union_seq = hist_union.string_att_value();
			live_any <<= union_seq;
		}
		break;

		case ATT_STATE:
		{
			DevVarStateArray &union_seq = hist_union.state_att_value();
			live_any <<= union_seq;
		}
		break;

		case DEVICE_STATE:
		{
			DevState union_sta = hist_union.dev_state_att();
			live_any <<= union_sta;
		}
		break;

		case ATT_ENCODED:
		case NO_DATA:
		break;
	}
}

void Device_2Impl::Polled_2_Live(long data_type,CORBA::Any &hist_any,CORBA::Any &live_any)
{
	const Tango::DevVarDoubleArray *tmp_db;
	Tango::DevVarDoubleArray *new_tmp_db;
	const Tango::DevVarShortArray *tmp_sh;
	Tango::DevVarShortArray *new_tmp_sh;
	const Tango::DevVarLongArray *tmp_lg;
	Tango::DevVarLongArray *new_tmp_lg;
	const Tango::DevVarLong64Array *tmp_lg64;
	Tango::DevVarLong64Array *new_tmp_lg64;
	const Tango::DevVarStringArray *tmp_str;
	Tango::DevVarStringArray *new_tmp_str;
	const Tango::DevVarFloatArray *tmp_fl;
	Tango::DevVarFloatArray *new_tmp_fl;
	const Tango::DevVarBooleanArray *tmp_boo;
	Tango::DevVarBooleanArray *new_tmp_boo;
	const Tango::DevVarUShortArray *tmp_ush;
	Tango::DevVarUShortArray *new_tmp_ush;
	const Tango::DevVarCharArray *tmp_uch;
	Tango::DevVarCharArray *new_tmp_uch;
	const Tango::DevVarULongArray *tmp_ulg;
	Tango::DevVarULongArray *new_tmp_ulg;
	const Tango::DevVarULong64Array *tmp_ulg64;
	Tango::DevVarULong64Array *new_tmp_ulg64;
	const Tango::DevVarStateArray *tmp_state;
	Tango::DevVarStateArray *new_tmp_state;

	switch (data_type)
	{
	case Tango::DEV_SHORT :
		hist_any >>= tmp_sh;
		new_tmp_sh = new DevVarShortArray(
		tmp_sh->length(),
		tmp_sh->length(),
		const_cast<short *>(tmp_sh->get_buffer()),
		false);
		live_any <<= new_tmp_sh;
		break;

	case Tango::DEV_DOUBLE :
		hist_any >>= tmp_db;
		new_tmp_db = new DevVarDoubleArray(
		tmp_db->length(),
		tmp_db->length(),
		const_cast<double *>(tmp_db->get_buffer()),
		false);
		live_any <<= new_tmp_db;
		break;

	case Tango::DEV_LONG :
		hist_any >>= tmp_lg;
		new_tmp_lg = new DevVarLongArray(tmp_lg->length(),tmp_lg->length(),
								const_cast<DevLong *>(tmp_lg->get_buffer()),false);
		live_any <<= new_tmp_lg;
		break;

	case Tango::DEV_LONG64 :
		hist_any >>= tmp_lg64;
		new_tmp_lg64 = new DevVarLong64Array(tmp_lg64->length(),tmp_lg64->length(),
								const_cast<DevLong64 *>(tmp_lg64->get_buffer()),false);
		live_any <<= new_tmp_lg64;
		break;

	case Tango::DEV_STRING :
		hist_any >>= tmp_str;
		new_tmp_str = new DevVarStringArray(
			tmp_str->length(),
			tmp_str->length(),
			const_cast<char **>(tmp_str->get_buffer()),
			false);
		live_any <<= new_tmp_str;
		break;

	case Tango::DEV_FLOAT :
		hist_any >>= tmp_fl;
		new_tmp_fl = new DevVarFloatArray(
			tmp_fl->length(),
			tmp_fl->length(),
			const_cast<float *>(tmp_fl->get_buffer()),
			false);
		live_any <<= new_tmp_fl;
		break;

	case Tango::DEV_BOOLEAN :
		hist_any >>= tmp_boo;
		new_tmp_boo = new DevVarBooleanArray(
			tmp_boo->length(),
			tmp_boo->length(),
			const_cast<DevBoolean *>(tmp_boo->get_buffer()),
			false);
		live_any <<= new_tmp_boo;
		break;

	case Tango::DEV_USHORT :
		hist_any >>= tmp_ush;
		new_tmp_ush = new DevVarUShortArray(
			tmp_ush->length(),
			tmp_ush->length(),
			const_cast<DevUShort *>(tmp_ush->get_buffer()),
			false);
		live_any <<= new_tmp_ush;
		break;

	case Tango::DEV_UCHAR :
		hist_any >>= tmp_uch;
		new_tmp_uch = new DevVarCharArray(
			tmp_uch->length(),
		   tmp_uch->length(),
 		   const_cast<DevUChar *>(tmp_uch->get_buffer()),
 		   false);
 	   live_any <<= new_tmp_uch;
 	   break;

   case Tango::DEV_ULONG :
 	   hist_any >>= tmp_ulg;
 	   new_tmp_ulg = new DevVarULongArray(tmp_ulg->length(),tmp_ulg->length(),
 								   const_cast<DevULong *>(tmp_ulg->get_buffer()),false);
 	   live_any <<= new_tmp_ulg;
 	   break;

   case Tango::DEV_ULONG64 :
 	   hist_any >>= tmp_ulg64;
 	   new_tmp_ulg64 = new DevVarULong64Array(tmp_ulg64->length(),tmp_ulg64->length(),
 								   const_cast<DevULong64 *>(tmp_ulg64->get_buffer()),false);
 	   live_any <<= new_tmp_ulg64;
 	   break;

   case Tango::DEV_STATE :
 	   hist_any >>= tmp_state;
 	   new_tmp_state = new DevVarStateArray(tmp_state->length(),tmp_state->length(),
 								   const_cast<DevState *>(tmp_state->get_buffer()),false);
 	   live_any <<= new_tmp_state;
 	   break;
   }
}


} // End of Tango namespace
