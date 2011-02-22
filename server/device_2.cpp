static const char *RcsId = "$Header$";

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
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:16:21  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:06  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:19  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:54  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:41  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <device_2.h>
#include <new>

#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */


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
DeviceImpl(device_class,dev_name),ext(NULL)
{
}
		
Device_2Impl::Device_2Impl(DeviceClass *device_class,
			   string &dev_name,
			   string &desc):
DeviceImpl(device_class,dev_name,desc),ext(NULL)
{
}
		
Device_2Impl::Device_2Impl(DeviceClass *device_class,
	           	   string &dev_name,string &desc,
	           	   Tango::DevState dev_state,string &dev_status):
DeviceImpl(device_class,dev_name,desc,dev_state,dev_status),ext(NULL)
{
} 
		   
Device_2Impl::Device_2Impl(DeviceClass *device_class,
	           	   const char *dev_name,
			   const char *desc,
	           	   Tango::DevState dev_state,
	           	   const char *dev_status):
DeviceImpl(device_class,dev_name,desc,dev_state,dev_status),ext(NULL)
{
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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

	cout4 << "Device_2Impl::command_inout_2 arrived, source = " << source << ", command = " << in_cmd << endl;
	
	PollObj *polled_cmd;
	bool polling_failed = false;

//
// If the source parameter specifies device, call the command_inout method
// implemented by the DeviceImpl class
//

	if (source == Tango::DEV)
	{
		return command_inout(in_cmd,in_data);
	}
	else
	{
		try
		{
		
//
// Record operation request in black box
//

			blackbox_ptr->insert_cmd(in_cmd,2);

			string cmd_str(in_cmd);
		
//
// Check that device supports this command
//

			check_command_exists(cmd_str);
		
//
// Check that the command is polled
//

			bool found = false;
			vector<PollObj *> &poll_list = get_poll_obj_list();
			long i;
			for (i = 0;i < poll_list.size();i++)
			{
				if ((poll_list[i]->get_type() == Tango::POLL_CMD) &&
			    	    (poll_list[i]->get_name() == in_cmd))
				{
					polled_cmd = poll_list[i];
					found = true;
				}
			}
		
//
// If the command is not polled but its polling update period is defined,
// and the command is not in the device list of command which should not be
// polled, start to poll it
//
		
			if (found == false)
			{
				Command *cmd = get_command(cmd_str);
				long poll_period = cmd->get_polling_period();
			
				if (poll_period == 0)
				{
					TangoSys_OMemStream o;
					o << "Command " << in_cmd << " not polled" << ends;
					Except::throw_exception((const char *)"API_CmdNotPolled",
							o.str(),
							(const char *)"Device_2Impl::command_inout");
				}
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
					
						adm_dev->add_obj_polling(send,false);
					
						delete send;

//
// Wait for first polling. Release monitor during this sleep in order
// to give it to the polling thread
//

#ifdef WIN32
						DeviceImpl::ext->only_one.rel_monitor();
						Sleep((DWORD)500);
						DeviceImpl::ext->only_one.get_monitor();
#else
						struct timespec to_wait,inter;
						to_wait.tv_sec = 0;
						to_wait.tv_nsec = 500000000;
						DeviceImpl::ext->only_one.rel_monitor();				
						nanosleep(&to_wait,&inter);
						DeviceImpl::ext->only_one.get_monitor();				
#endif

//
// Update the polled-cmd pointer to the new polled object
//

						for (i = 0;i < poll_list.size();i++)
						{
							if ((poll_list[i]->get_type() == Tango::POLL_CMD) &&
			    			    	(poll_list[i]->get_name() == in_cmd))
							{
								polled_cmd = poll_list[i];
								break;
							}
						}
					}

				}
			}
		
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
//

			double last = polled_cmd->get_last_insert_date();
			struct timeval now;
#ifdef WIN32
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
		catch (Tango::DevFailed &)
		{
			if (source == Tango::CACHE)
				throw;
			polling_failed = true;
		}

//
// Get cmd result (or exception)
//

		if (source == Tango::CACHE)
		{
			cout4 << "Device_2Impl: Returning data from polling buffer" << endl;
			return polled_cmd->get_last_cmd_result();
		}
		else
		{
			if (polling_failed == false)
			{
				cout4 << "Device_2Impl: Returning data from polling buffer" << endl;
				return polled_cmd->get_last_cmd_result();
			}
			else
				return command_inout(in_cmd,in_data);
		}
	
	}
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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

	cout4 << "Device_2Impl::read_attributes_2 arrived" << endl;
	
	bool att_in_fault = false;
	bool polling_failed = false;
	Tango::AttributeValueList *back = NULL;

//
// If the source parameter specifies device, call the command_inout method
// implemented by the DeviceImpl class
//

	if (source == Tango::DEV)
	{
		return read_attributes(names);
	}
	else
	{

		try
		{
		
//
// Record operation request in black box
//

			blackbox_ptr->insert_op(Op_Read_Attr_2);
			
//
// Build a sequence with the names of the attribute to be read.
// This is necessary in case of the "AllAttr" shortcut is used
// If all attributes are wanted, build this list
//

			long i,j;
			bool all_attr = false;
			vector<PollObj *> &poll_list = get_poll_obj_list();
			long nb_poll = poll_list.size();
			long nb_names = names.length();
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
							real_names.length(real_names.length() + 1);
							real_names[i] = poll_list[i]->get_name().c_str();
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
						if (strcmp(poll_list[j]->get_name().c_str(),real_names[i]) == 0)
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
#ifdef WIN32
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

#ifdef WIN32
				DeviceImpl::ext->only_one.rel_monitor();
				Sleep((DWORD)500);
				DeviceImpl::ext->only_one.get_monitor();
#else
				struct timespec to_wait,inter;
				to_wait.tv_sec = 0;
				to_wait.tv_nsec = 500000000;
				DeviceImpl::ext->only_one.rel_monitor();				
				nanosleep(&to_wait,&inter);
				DeviceImpl::ext->only_one.get_monitor();				
#endif

			}

//
// Allocate memory for the AttributeValue structures
//

			long nb_attr = real_names.length();
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
				bool found = false;
				vector<PollObj *> &poll_list = get_poll_obj_list();
				PollObj *polled_attr;
				long j;
				for (j = 0;j < poll_list.size();j++)
				{
					if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
			    	    	    (strcmp(poll_list[j]->get_name().c_str(),real_names[i]) == 0))
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
//

				double last = polled_attr->get_last_insert_date();
				struct timeval now;
#ifdef WIN32
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

//
// Get attribute data type
//

				Attribute &att = dev_attr->get_attr_by_name(real_names[i]);
				long type = att.get_data_type();
								
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
				
						AttributeValue &att_val = polled_attr->get_last_attr_value(false);
					
						const Tango::DevVarDoubleArray *tmp_db;
						Tango::DevVarDoubleArray *new_tmp_db;
						const Tango::DevVarShortArray *tmp_sh;
						Tango::DevVarShortArray *new_tmp_sh;
						const Tango::DevVarLongArray *tmp_lg;
						Tango::DevVarLongArray *new_tmp_lg;
						const Tango::DevVarStringArray *tmp_str;
						Tango::DevVarStringArray *new_tmp_str;
																									
						switch (type)
						{
						case Tango::DEV_SHORT :
							att_val.value >>= tmp_sh;
							new_tmp_sh = new DevVarShortArray(
								tmp_sh->length(),
								tmp_sh->length(),
								const_cast<short *>(tmp_sh->get_buffer()),
								false);
							(*back)[i].value <<= new_tmp_sh;
							break;
						
						case Tango::DEV_DOUBLE :					
							att_val.value >>= tmp_db;		
							new_tmp_db = new DevVarDoubleArray(
								tmp_db->length(),
								tmp_db->length(),
								const_cast<double *>(tmp_db->get_buffer()),
								false);
							(*back)[i].value <<= new_tmp_db;
							break;
					
						case Tango::DEV_LONG :
							att_val.value >>= tmp_lg;
							new_tmp_lg = new DevVarLongArray(
								tmp_lg->length(),
								tmp_lg->length(),
								const_cast<long *>(tmp_lg->get_buffer()),
								false);
							(*back)[i].value <<= new_tmp_lg;
							break;
						
						case Tango::DEV_STRING :
							att_val.value >>= tmp_str;
							new_tmp_str = new DevVarStringArray(
								tmp_str->length(),
								tmp_str->length(),
								const_cast<char **>(tmp_str->get_buffer()),
								false);
							(*back)[i].value <<= new_tmp_str;
							break;
						}
						(*back)[i].quality= att_val.quality;
						(*back)[i].time = att_val.time;
						(*back)[i].dim_x = att_val.dim_x;
						(*back)[i].dim_y = att_val.dim_y;
						(*back)[i].name = CORBA::string_dup(att_val.name);
			
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

//
// If the source parameter is CACHE, returned data, else called method reading
// attribute from device
//

		if ((source == Tango::CACHE_DEV) && (polling_failed == true))
		{
			if (back != NULL)
				delete back;		
			return read_attributes(names);
		}
		else
		{
			cout4 << "Device_2Impl: Returning attribute(s) value from polling buffer" << endl;
			return back;
		}			
	}

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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

	cout4 << "DeviceImpl::command_query_2 arrived" << endl;

	Tango::DevCmdInfo_2 *back;
	string cmd(command);

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
		if (device_class->get_command_list()[i]->get_name() == command)
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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

	cout4 << "Device_2Impl::get_attribute_config_2 arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList_2 *back;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config_2);
	
//
// Get attribute number
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	
//
// Check if the caller want to get config for all attribute
//

	string in_name(names[0]);
	if ((nb_attr == 1) && (in_name == AllAttr))
	{
		nb_attr = nb_dev_attr;
		all_attr = true;
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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

	cout4 << "Device_2Impl::command_inout_history_2 arrived" << endl;
	Tango::DevCmdHistoryList *back;

	string cmd_str(command);

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command_inout_history_2);
			
//
// Check that device supports this command
//

	check_command_exists(cmd_str);
		
//
// Check that the command is polled
//

	PollObj *polled_cmd;
	bool found = false;
	vector<PollObj *> &poll_list = get_poll_obj_list();
	long i;
	for (i = 0;i < poll_list.size();i++)
	{
		if ((poll_list[i]->get_type() == Tango::POLL_CMD) &&
		    (poll_list[i]->get_name() == cmd_str))
		{
			polled_cmd = poll_list[i];
			found = true;
		}
	}
	if (found == false)
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
//

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
	AutoTangoMonitor sync(DeviceImpl::ext->only_one);

	cout4 << "Device_2Impl::read_attribute_history_2 arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Read_Attr_history_2);
	
	Tango::DevAttrHistoryList *back;
	vector<PollObj *> &poll_list = get_poll_obj_list();
	long nb_poll = poll_list.size();
	
//
// Check that the device supports this attribute. This method returns an 
// exception in case of unsupported attribute
//

	Attribute &att = dev_attr->get_attr_by_name(name);

	string attr_str(name);
	
//
// Check that the wanted attribute is polled.
//

	long j;
	PollObj *polled_attr;
	for (j = 0;j < nb_poll;j++)
	{
		if ((poll_list[j]->get_type() == Tango::POLL_ATTR) &&
		    (poll_list[j]->get_name() == attr_str))
		{
			polled_attr = poll_list[j];
			break;
		}
	}
	if (j == nb_poll)
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

	
	polled_attr->get_attr_history(n,back,att.get_data_type());

	cout4 << "Leaving Device_2Impl::command_inout_history_2 method" << endl;				
	return back;
}
			   
} // End of Tango namespace
