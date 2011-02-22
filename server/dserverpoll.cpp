static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               DServer.cpp
//
// description :        C++ source for the DServer class and its commands. 
//			The class is derived from Device. It represents the
//			CORBA servant object which will be accessed from the
//			network. All commands which can be executed on a
//			DServer object are implemented in this file.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
// Revision 3.30  2009/04/01 06:31:25  taurel
// - Only some changes in printed messages when using -v5
//
// Revision 3.29  2009/03/30 15:03:44  taurel
// - Fix last bugs before Tango 7 ??
//
// Revision 3.28  2009/03/13 09:33:29  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.27  2009/02/27 13:26:46  taurel
// - Small changes for Solaris
//
// Revision 3.26  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.25  2009/01/15 13:57:20  taurel
// - Fix bugs found by Jens
//
// Revision 3.24  2008/12/17 09:50:59  taurel
// - First implementation of attributes sent on the wire using IDL Union
// instead of IDL Any
//
// Revision 3.23  2008/10/06 15:01:09  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.22  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.21  2008/10/02 09:09:47  taurel
// - First implementation of multiple polling thread(s)
//
// Revision 3.20  2008/01/08 14:38:18  taurel
// - strcasecmp() is not named like this on Windows !!
//
// Revision 3.19  2007/11/08 12:03:44  taurel
// - Start implementing user interceptors
// - Fix bug in poll thread pproperty management when removing polling object
// - Set a database timeout to 6 sec
//
// Revision 3.18  2007/10/26 11:35:44  taurel
// - Fix bug in synchronization with polling thread for start_polling, stop_polling and rem_obj_polling
// - String comparaison now case insensitive when storing polling properties in db
//
// Revision 3.17  2007/10/16 08:23:37  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
// Revision 3.16  2007/05/17 07:59:07  taurel
// - The polling is not configured via a separate thread any more. The polling thread add_obj_polling method has been modified to support a parameter telling to the polling thread when it has to polled the object.
// Add device name in monitor print message
// Add device_destroyer method in DeviceClass class
//
// Revision 3.15  2007/05/15 07:46:59  taurel
// - The polling thread is not configured by a separate thread any more.
// The Add_obj_polling command now support a delta_t to start the first polling
//
// Revision 3.14  2007/05/04 13:02:18  jensmeyer
// Added the removal of the attribute polling and event configuration when
// executing the remove_attribute() method.
//
// Revision 3.13  2007/04/20 14:40:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.12  2007/04/16 14:56:36  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.11  2007/03/29 07:00:28  taurel
// - Fix some data type for 64 bits compatibility
//
// Revision 3.10  2006/04/13 13:31:44  taurel
// Several changes:
// - A new DeviceImpl::push_event() method to push USER event with State as data
// - Fix bug in polling thread POLL_IUPD_PERIOD. The new polling period was applied only after the following poll still done with the old polling period
// - It is now possible to update polling period of polled objects from a polled attribute or command
//
// Revision 3.9  2006/01/20 08:12:17  taurel
// - No real change in this file. Simply changes in indentation
//
// Revision 3.8  2005/06/29 08:31:17  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.7  2005/01/13 08:29:05  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.6.2.8  2004/12/06 14:39:29  taurel
// - Polling starts in a separate thread
// - For Windows : Polling thread cmd/attr measurement used PerformanceCounter
// - Fix bug in pollext.h in methods to externally fill polling buffer
//
// Revision 3.6.2.7  2004/11/04 09:46:38  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.6.2.6  2004/10/22 11:25:50  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.6.2.5  2004/09/15 06:46:28  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.6.2.4  2004/08/26 07:34:45  taurel
// - Implement a way to directly fills command or attribute polling buffer
//
// Revision 3.6.2.3  2004/08/19 07:44:59  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.6.2.2  2004/07/15 15:04:06  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.6.2.1  2004/07/08 11:02:13  taurel
// - Add possibility to have command/attribute polling buffer depth specified
//   individually
//
// Revision 3.6  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.5  2003/08/22 12:52:54  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.4  2003/08/21 07:23:46  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.3.2.3  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.3.2.2  2003/10/03 13:34:26  taurel
// - Fix bug for device server started without database, with device name specified on command line using mix of upper and lower cases
// - It's now possible to send a command to the polling thread from itself
// - Add a release lock if the insert into the polling buffer failed
//
// Revision 3.3.2.1  2003/09/30 11:50:43  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.3  2003/05/28 14:55:09  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.2  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.1  2003/04/03 15:24:09  taurel
// Added methods to print DeviceData, DeviceAttribute, DeviceDataHistory
// and DeviceAttributeHistory instance
//
// Revision 3.0  2003/03/25 16:43:02  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:19  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
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
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <math.h>

#ifdef _TG_WINDOWS_
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
	#include <iomanip>
#else
	#include <iomanip.h>
#endif

namespace Tango
{

//+----------------------------------------------------------------------------
//
// method : 		DServer::polled_device()
// 
// description : 	command to read all the devices actually polled by the
//			device server
//
// out :		The device name list in a strings sequence
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::polled_device()
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In polled_device command" << endl;	
	
	long nb_class = class_list.size();
	vector<string> dev_name;
	
	try
	{
		for (int i = 0;i < nb_class;i++)
		{
			long nb_dev = class_list[i]->get_device_list().size();
			for (long j = 0;j < nb_dev;j++)
			{
				if ((class_list[i]->get_device_list())[j]->is_polled() == true)
				{
					dev_name.push_back((class_list[i]->get_device_list())[j]->get_name().c_str());
				}
			}			
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"DServer::polled_device");
	}

//
// Return an empty sequence if no devices are polled
//
	
	if (dev_name.size() == 0)
	{
		Tango::DevVarStringArray *ret = new Tango::DevVarStringArray();
		ret->length(0);
		return ret;
	}

//
// Returned device name list to caller (sorted)
//

	sort(dev_name.begin(),dev_name.end());
	long nb_dev = dev_name.size();	
	Tango::DevVarStringArray *ret = new Tango::DevVarStringArray(nb_dev);
	ret->length(nb_dev);
	for (long k = 0;k < nb_dev;k++)
		(*ret)[k] = dev_name[k].c_str();
			
	return(ret);
	
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::dev_polled_status()
// 
// description : 	command to read device polling status
//
// out :		The device polling status as a string (multiple lines)
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::dev_poll_status(string &dev_name)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In dev_poll_status method" << endl;

//
// Find the device
//	

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev;

	dev = tg->get_device_by_name(dev_name);

	vector<PollObj *> &poll_list = dev->get_poll_obj_list();
	long nb_poll_obj = poll_list.size();

//
// Return an empty sequence if nothing is polled for this device
//

	if (nb_poll_obj == 0)
	{
		Tango::DevVarStringArray *ret;
		ret = new DevVarStringArray();
		ret->length(0);
		return ret;
	}
	
	
	long i,nb_cmd,nb_attr;
	
//
// Compute how many cmds and/or attributes are polled
// Since IDL V3, state and status are polled as attributes
// For compatibility, if one of these "attributes" is polled,
// also returned the info as the command is polled
//

	nb_cmd = nb_attr = 0;
	long nb_to_add = 0;
	for (i = 0;i < nb_poll_obj;i++)
	{
		if (poll_list[i]->get_type() == Tango::POLL_CMD)
			nb_cmd++;
		else
		{ 	 
			nb_attr++;
			if ((poll_list[i]->get_name() == "state") || 	 
     			    (poll_list[i]->get_name() == "status")) 	 
			{ 	 
				nb_cmd++; 	 
				nb_to_add++; 	 
			} 	 
		}
	}

//
// Allocate memory for returned strings
//

	Tango::DevVarStringArray *ret;
	ret = new DevVarStringArray(nb_poll_obj + nb_to_add);
	ret->length(nb_poll_obj + nb_to_add);

//
// Populate returned strings
//

	long cmd_ind = 0;
	long attr_ind = nb_cmd;
	string returned_info;
	
	for(i = 0;i < nb_poll_obj;i++)
	{
		bool duplicate = false;
			
//
// First, the name
//
		
		Tango::PollObjType type = poll_list[i]->get_type();
		if (type == Tango::POLL_CMD)
		{
			returned_info = "Polled command name = ";
			long k;	
			long nb_cmd = dev->get_device_class()->get_command_list().size();
			for (k = 0;k < nb_cmd;k++)
			{
				if (dev->get_device_class()->get_command_list()[k]->get_lower_name() == poll_list[i]->get_name())
				{
					returned_info = returned_info + dev->get_device_class()->get_command_list()[k]->get_name();
					break;
				}
			}
		}		
		else
		{
			returned_info = "Polled attribute name = ";
			
			if (poll_list[i]->get_name() == "state")
			{
				duplicate = true;
				returned_info = returned_info + "State";
			}
			else if (poll_list[i]->get_name() == "status")
			{
				duplicate = true;
				returned_info = returned_info + "Status";
			}
			else
			{
				Attribute &att = dev->get_device_attr()->get_attr_by_name(poll_list[i]->get_name().c_str());
				returned_info = returned_info + att.get_name();
			}		
		}
				
//
// Add update period
//

//		TangoSys_MemStream s;
		stringstream s;
		string tmp_str;

		long po = poll_list[i]->get_upd();
		if (po == 0)
		{
			returned_info = returned_info + "\nPolling externally triggered";
		}
		else
		{
			returned_info = returned_info + "\nPolling period (mS) = ";
			s << po;
			s >> tmp_str;
			returned_info = returned_info + tmp_str;
			s.clear();	// clear the stream eof flag
		}
							
		s.str("");	// clear the underlying string

//
// Add ring buffer depth
//

		returned_info = returned_info + "\nPolling ring buffer depth = ";
		long depth;
		if (type == Tango::POLL_CMD)
			depth = dev->get_cmd_poll_ring_depth(poll_list[i]->get_name());
		else
			depth = dev->get_attr_poll_ring_depth(poll_list[i]->get_name());
		
		s << depth;
		returned_info = returned_info + s.str();
		
		s.str("");	// Clear the underlying string
								
//
// Add a message if the data ring is empty
//

		if (poll_list[i]->is_ring_empty() == true)
		{
			returned_info = returned_info + "\nNo data recorded yet";
		}
		else
		{

//
// Take polled object ownership in order to have coherent info returned to caller
// We don't want the polling thread to insert a new elt in polling ring while
// we are getting these data. Therefore, use the xxx_i methods
//

			{
			omni_mutex_lock sync(*(poll_list[i]));
							
//
// Add needed time to execute last command
//

			double tmp_db = poll_list[i]->get_needed_time_i();
			if (tmp_db == 0.0)
			{
				returned_info = returned_info + "\nThe polling buffer is externally filled in";
			}
			else
			{
				if (po != 0)
				{
					returned_info = returned_info + "\nTime needed for the last ";
					if (type == Tango::POLL_CMD)
						returned_info = returned_info + "command execution (mS) = ";
					else
						returned_info = returned_info + "attribute reading (mS) = "; 

					s.setf(ios::fixed);							
					s << setprecision(3) << tmp_db;
					returned_info = returned_info + s.str();

					s.str("");
						
//
// Add not updated since... info
//

					returned_info = returned_info + "\nData not updated since ";
					double since = poll_list[i]->get_last_insert_date_i();
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
					double diff_t = now_d - since;
					diff_t = diff_t - (tmp_db / 1000);
					if (diff_t < 1.0)
					{
						long nb_msec = (long)(diff_t * 1000);
						s << nb_msec;

						returned_info = returned_info + s.str() + " mS";
						s.str("");
					}
					else if (diff_t < 60.0)
					{
						long nb_sec = (long)diff_t;
						long nb_msec = (long)((diff_t - nb_sec) * 1000);

						s << nb_sec;

						returned_info = returned_info + s.str() + " S and ";
						s.str("");

						s << nb_msec;
						returned_info = returned_info + s.str() + " mS";
						s.str("");
					}
					else
					{
						long nb_min = (long)(diff_t / 60);
						long nb_sec = (long)(diff_t - (60 * nb_min));
						long nb_msec = (long)((diff_t - (long)diff_t) * 1000);

						s << nb_min;
						returned_info = returned_info + s.str() + " MN";
						s.str("");

						if (nb_sec != 0)
						{
							s << nb_sec ;
							returned_info = returned_info + " ," + s.str() + " S";
							s.str("");
						}

						if (nb_msec != 0)
						{
							s << nb_msec;
							returned_info = returned_info + " and " + s.str() + " mS";
							s.str("");
						}
					}
				}
			} 
			
//
// Add delta_t between last record(s)
//

			try
			{
				vector<double> delta;
				poll_list[i]->get_delta_t_i(delta,4);
				
				returned_info = returned_info + "\nDelta between last records (in mS) = ";
				for (unsigned long j = 0;j < delta.size();j++)
				{
					long nb_msec = (long)(delta[j] * 1000);
					s << nb_msec;
					returned_info = returned_info + s.str();
					s.str("");
					if (j != (delta.size() - 1))
						returned_info = returned_info + ", ";
				}				
			}
			catch (Tango::DevFailed)
			{
			}
			
			
//
// Add last polling exception fields (if any)
//

			long dev_vers;
			bool last_err;
			
			dev_vers = dev->get_dev_idl_version();
			if (dev_vers < 3)
				last_err = poll_list[i]->is_last_an_error_i();
			else
				last_err = poll_list[i]->is_last_an_error_i_3();				
			if (last_err == true)
			{
				if (type == Tango::POLL_CMD)
					returned_info = returned_info + "\nLast command execution FAILED :";
				else
					returned_info = returned_info + "\nLast attribute read FAILED :";

				Tango::DevFailed *exe_ptr = poll_list[i]->get_last_except_i();
				returned_info = returned_info + "\n\tReason = " + exe_ptr->errors[0].reason.in();
				returned_info = returned_info + "\n\tDesc = " + exe_ptr->errors[0].desc.in();
				returned_info = returned_info + "\n\tOrigin = " + exe_ptr->errors[0].origin.in();
			}
			
//
// Release polled object monitor (only a compiler block end)
//
			}
		}

//
// Init. string in sequence
//
			
		if (type == Tango::POLL_CMD)
		{
			(*ret)[cmd_ind] = CORBA::string_dup(returned_info.c_str());
			cmd_ind++;
		}
		else
		{
			(*ret)[attr_ind] = CORBA::string_dup(returned_info.c_str());
			attr_ind++;

// 	 
// If the attribute is state or status, also add the string in command list 	 
// after replacing all occurences of "attributes" by "command" in the returned 	 
// string 	 
// 	 
  	 
			if (duplicate == true)  	
			{	
				string::size_type pos = returned_info.find("attribute");	
				while (pos != string::npos)	
				{	
					returned_info.replace(pos,9,"command"); 	
					string::size_type npos; 	
					npos = returned_info.find("attribute",pos);	
					pos = npos;	
				}	
				(*ret)[cmd_ind] = CORBA::string_dup(returned_info.c_str());	
				cmd_ind++;	
			}
		}
	}
				
	return(ret);
	
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::add_obj_polling()
// 
// description : 	command to add one object to be polled
//
// in :			The polling parameters :
//				device name
//				object type (command or attribute)
//				object name
//				update period in mS (in the long array)
//
//-----------------------------------------------------------------------------

void DServer::add_obj_polling(const Tango::DevVarLongStringArray *argin,
			      bool with_db_upd,int delta_ms)
{
	NoSyncModelTangoMonitor nosyn_mon(this);

	cout4 << "In add_obj_polling method" << endl;
	unsigned long i;
	for (i = 0;i < argin->svalue.length();i++)
		cout4 << "Input string = " << (argin->svalue)[i].in() << endl;
	for (i = 0;i < argin->lvalue.length();i++)
		cout4 << "Input long = " << (argin->lvalue)[i] << endl;
		
//
// Check that parameters number is correct
//

	if ((argin->svalue.length() != 3) || (argin->lvalue.length() != 1))
	{
		Except::throw_exception((const char *)"API_WrongNumberOfArgs",
					(const char *)"Incorrect number of inout arguments",
					(const char *)"DServer::add_obj_polling");
	}
	
//
// Find the device
//	

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev;
	try
	{
		dev = tg->get_device_by_name((argin->svalue)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (argin->svalue)[0] << " not found" << ends;
		
		Except::re_throw_exception(e,(const char *)"API_DeviceNotFound",o.str(),
				   	   (const char *)"DServer::add_obj_polling");
	}

//
// Check that the command (or the attribute) exists. For command, also checks
// that it does not need input value.
//

	string obj_type((argin->svalue)[1]);
	transform(obj_type.begin(),obj_type.end(),obj_type.begin(),::tolower);
	string obj_name((argin->svalue)[2]);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
	PollObjType type;
	
	if (obj_type == PollCommand)
	{
		dev->check_command_exists(obj_name);
		type = Tango::POLL_CMD;		
	}
	else if (obj_type == PollAttribute)
	{
		dev->get_device_attr()->get_attr_by_name((argin->svalue)[2]);
		type = Tango::POLL_ATTR;	
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Object type " << obj_type << " not supported" << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::add_obj_polling");
	}
	
//
// If it's for the Init command, refuse to poll it
//

	if (obj_type == PollCommand)
	{
		if (obj_name == "init")
		{
			TangoSys_OMemStream o;
			o << "It's not possible to poll the Init command!" << ends;
			Except::throw_exception((const char *)"API_NotSupported",o.str(),
						(const char *)"DServer::add_obj_polling");
		}

//
// Since IDl release 3, state and status command must be polled
// as attributes to be able to generate event on state or
// status.
//

		else if ((dev->get_dev_idl_version() >= 3) && ((obj_name == "state") || (obj_name == "status")))
			type = Tango::POLL_ATTR;
	}
	
//
// Check that the object is not already polled
//

	vector<PollObj *> &poll_list = dev->get_poll_obj_list();
	for (i = 0;i < poll_list.size();i++)
	{
		if (poll_list[i]->get_type() == type)
		{
			string name_lower = poll_list[i]->get_name();
			transform(name_lower.begin(),name_lower.end(),name_lower.begin(),::tolower);
			if (name_lower == obj_name)
			{
				TangoSys_OMemStream o;
				if (type == Tango::POLL_CMD)
					o << "Command ";
				else
					o << "Attribute ";
				o << obj_name << " already polled" << ends;
				Except::throw_exception((const char *)"API_AlreadyPolled",
							o.str(),
							(const char *)"DServer::add_obj_polling");
			}
		}
	}
		
//
// Check that the update period is not to small
//

	int upd = (argin->lvalue)[0];
	if ((upd != 0) && (upd < MIN_POLL_PERIOD))
	{
		TangoSys_OMemStream o;
		o << (argin->lvalue)[0] << " is below the min authorized period (" << MIN_POLL_PERIOD << " mS)" << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::add_obj_polling");
	}
	
//
// Round the update period (which is in mS) to the next tenth of a second
//

/*	double upd_db = (double)upd;
	double remain = fmod(upd_db,(double)MIN_POLL_PERIOD);
	double base = floor(upd_db) - remain;
	if (remain >= 50)
		upd = (long)base + MIN_POLL_PERIOD;
	else
		upd = (long)base;
	cout4 << "Rounded polling period = " << upd << " mS" << endl;*/
	
//
// Create a new PollObj instance for this object
// Protect this code by a monitor in case of the polling thread using one of
// the vector element.
//

	long depth;
	if (obj_type == PollCommand)
		depth = dev->get_cmd_poll_ring_depth(obj_name);
	else
		depth = dev->get_attr_poll_ring_depth(obj_name);

	dev->get_poll_monitor().get_monitor();	
	poll_list.push_back(new PollObj(dev,type,obj_name,upd,depth));
	dev->get_poll_monitor().rel_monitor();

//
// Find out which thread is in charge of the device.
// If none exists already, create one
//

	PollingThreadInfo *th_info;
	int thread_created = -2;
	
	int poll_th_id = tg->get_polling_thread_id_by_name((argin->svalue)[0]);
	if (poll_th_id == 0)
	{
		cout4 << "POLLING: Creating a thread to poll device " << (argin->svalue)[0] << endl;
		thread_created = tg->create_poll_thread((argin->svalue)[0],false);
		poll_th_id = tg->get_polling_thread_id_by_name((argin->svalue)[0]);
	}

	cout4 << "POLLING: Thread in charge of device " << (argin->svalue)[0] << " is thread " << poll_th_id << endl;
	th_info = tg->get_polling_thread_info_by_id(poll_th_id);
	
//
// Send command to the polling thread but wait in case of previous cmd 
// still not executed
//

	cout4 << "Sending cmd to polling thread" << endl;
	int interupted;
	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	{	
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_ADD_OBJ;
		shared_cmd.dev = dev;
		shared_cmd.index = poll_list.size() - 1;
		shared_cmd.new_upd = delta_ms;

		mon.signal();
		
		cout4 << "Cmd sent to polling thread" << endl;

//
// Wait for thread to execute command except if the command is
// requested by the polling thread itself
//

		omni_thread *th = omni_thread::self();
		int th_id = th->id();
		if (th_id != poll_th_id)
		{
			while (shared_cmd.cmd_pending == true)
			{
				interupted = mon.wait(DEFAULT_TIMEOUT);
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << endl;
					delete poll_list.back();
					poll_list.pop_back();

//
// If the thread has been created by this request, try to kill it
//
					
					if (thread_created == -1)
					{
						shared_cmd.cmd_pending = true;
						shared_cmd.cmd_code = POLL_EXIT;
						
						mon.signal();
					}
					Except::throw_exception((const char *)"API_CommandTimedOut",
					        		(const char *)"Polling thread blocked !!!",
					        		(const char *)"DServer::add_obj_polling");
				}
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
	th_info->nb_polled_objects++;
		
//
// Update polling parameters in database (if wanted and possible)
// If the property is already there (it should not but...), only update its
// polling period
//

	if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
	{
		TangoSys_MemStream s;
		string upd_str;
		s << upd;
		s >> upd_str;
		bool found = false;

		DbDatum db_info("polled_cmd");	
		if (type == Tango::POLL_CMD)
		{
			vector<string> &non_auto_list = dev->get_non_auto_polled_cmd();
			vector<string>::iterator ite;
			for (ite = non_auto_list.begin();ite < non_auto_list.end();++ite)
			{
				if (*ite == obj_name)
				{
					non_auto_list.erase(ite);
					db_info.name = "non_auto_polled_cmd";
					db_info << non_auto_list;
					found = true;
					break;
				}
			}
			if (found == false)
			{	
				vector<string> &cmd_list = dev->get_polled_cmd();
				for (i = 0;i < cmd_list.size();i = i+2)
				{
					if (TG_strcasecmp(cmd_list[i].c_str(),obj_name.c_str()) == 0)
					{
						cmd_list[i + 1] = upd_str;
						break;
					}
				}
				if (i == cmd_list.size())
				{
					cmd_list.push_back(obj_name);
					cmd_list.push_back(upd_str);
				}
				db_info << cmd_list;
			}
		}
		else
		{
			vector<string> &non_auto_list = dev->get_non_auto_polled_attr();
			vector<string>::iterator ite;
			for (ite = non_auto_list.begin();ite < non_auto_list.end();++ite)
			{
				if (*ite == obj_name)
				{
					non_auto_list.erase(ite);
					db_info.name = "non_auto_polled_attr";
					db_info << non_auto_list;
					found = true;
					break;
				}
			}
			if (found == false)
			{	
				db_info.name = "polled_attr";		
				vector<string> &attr_list = dev->get_polled_attr();
				for (i = 0;i < attr_list.size();i = i+2)
				{
					if (TG_strcasecmp(attr_list[i].c_str(),obj_name.c_str()) == 0)
					{
						attr_list[i + 1] = upd_str;
						break;
					}
				}
				if (i == attr_list.size())
				{
					attr_list.push_back(obj_name);
					attr_list.push_back(upd_str);
				}
				db_info << attr_list;
			}
		}	

		DbData send_data;
		send_data.push_back(db_info);	
		dev->get_db_device()->put_property(send_data);
	}

//
// If a polling thread has just been created, ask it to poll
//

	if (thread_created == -1)
	{
		start_polling(th_info);
	}
			
//
// Also update the polling threads pool conf if one thread has been created by this call
//

	if (thread_created != -2)
	{
		string dev_name((argin->svalue)[0]);
		transform(dev_name.begin(),dev_name.end(),dev_name.begin(),::tolower);
		cout4 << "thread_created = " << thread_created << endl;
		if (thread_created == -1)
		{
			tg->get_poll_pool_conf().push_back(dev_name);
		}
		else if (thread_created >= 0)
		{
			string &conf_entry = (tg->get_poll_pool_conf())[thread_created];
			conf_entry = conf_entry + ',' + dev_name;
		}

		if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
		{
			DbData send_data;
			send_data.push_back(DbDatum("polling_threads_pool_conf"));			
			send_data[0] << tg->get_poll_pool_conf();

			tg->get_dserver_device()->get_db_device()->put_property(send_data);
		}
	}

	cout4 << "Polling properties updated" << endl;
	
//
// Mark the device as polled
//

	dev->is_polled(true);
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::upd_obj_polling_period()
// 
// description : 	command to upadte an already polled object update period
//
// in :			The polling parameters :
//				device name
//				object type (command or attribute)
//				object name
//				update period in mS (in the long array)
//
//-----------------------------------------------------------------------------

void DServer::upd_obj_polling_period(const Tango::DevVarLongStringArray *argin,
				     bool with_db_upd)
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In upd_obj_polling_period method" << endl;
	unsigned long i;
	for (i = 0;i < argin->svalue.length();i++)
		cout4 << "Input string = " << (argin->svalue)[i].in() << endl;
	for (i = 0;i < argin->lvalue.length();i++)
		cout4 << "Input long = " << (argin->lvalue)[i] << endl;

//
// Check that parameters number is correct
//

	if ((argin->svalue.length() != 3) || (argin->lvalue.length() != 1))
	{
		Except::throw_exception((const char *)"API_WrongNumberOfArgs",
					(const char *)"Incorrect number of inout arguments",
					(const char *)"DServer::upd_obj_polling_period");
	}
	
//
// Find the device
//	

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev;
	try
	{
		dev = tg->get_device_by_name((argin->svalue)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (argin->svalue)[0] << " not found" << ends;
		
		Except::re_throw_exception(e,(const char *)"API_DeviceNotFound",o.str(),
				   	   (const char *)"DServer::upd_obj_polling_period");
	}
	
//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << (argin->svalue)[0] << " is not polled" << ends;
		
		Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   	(const char *)"DServer::upd_obj_polling_period");
	}
	
//
// Find the wanted object in the list of device polled object
//

	string obj_type((argin->svalue)[1]);
	transform(obj_type.begin(),obj_type.end(),obj_type.begin(),::tolower);
	string obj_name((argin->svalue)[2]);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
	PollObjType type;
	
	if (obj_type == PollCommand)
	{
		type = Tango::POLL_CMD;
//
// Since IDl release 3, state and status command must be polled
// as attributes to be able to generate event on state or
// status.
//

		if ((obj_name == "state") || (obj_name == "status"))
			type = Tango::POLL_ATTR;
	}
	else if (obj_type == PollAttribute)
	{
		type = Tango::POLL_ATTR;	
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Object type " << obj_type << " not supported" << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::upd_obj_polling_period");
	}
		
	vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(type,obj_name);

//
// Check that it is not an externally triggered polling object. In this
// case, throw exception
//

/*	long tmp_upd = (*ite)->get_upd();
	if (tmp_upd == 0)
	{
		TangoSys_OMemStream o;

		o << "Polling for ";		
		if (type == Tango::POLL_CMD)
			o << "command ";
		else
			o << "attribute ";
		o << (argin->svalue)[2];
		o << " (device " << (argin->svalue)[0] << ") ";
		o << " is externally triggered. Remove and add object to change its polling period";
		o << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::upd_obj_polling_period");
	}*/

//
// Check that the update period is not to small
//

	int upd = (argin->lvalue)[0];
	if ((upd != 0) && (upd < MIN_POLL_PERIOD))
	{
		TangoSys_OMemStream o;
		o << (argin->lvalue)[0] << " is below the min authorized period (" << MIN_POLL_PERIOD << " mS)" << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::upd_obj_polling");
	}

//
// Find out which thread is in charge of the device.
// If none exists already, create one
//

	PollingThreadInfo *th_info;
		
	int poll_th_id = tg->get_polling_thread_id_by_name((argin->svalue)[0]);
	if (poll_th_id == 0)
	{
		TangoSys_OMemStream o;
		o << "Can't find a polling thread for device " << (argin->svalue)[0] << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::upd_obj_polling");
	}

	th_info = tg->get_polling_thread_info_by_id(poll_th_id);

//
// Update polling period
//

	(*ite)->update_upd((argin->lvalue)[0]);
	
//
// Send command to the polling thread
//

	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	int th_id = omni_thread::self()->id();
	if (th_id != poll_th_id)		
	{	
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_UPD_PERIOD;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = type;
		shared_cmd.new_upd = (argin->lvalue)[0];

#ifdef __SUNPRO_CC
		distance(dev->get_poll_obj_list().begin(),ite,shared_cmd.index);
#else
		shared_cmd.index = distance(dev->get_poll_obj_list().begin(),ite);
#endif
		mon.signal();
	}
	else
	{
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_UPD_PERIOD;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = type;
		shared_cmd.new_upd = (argin->lvalue)[0];
#ifdef __SUNPRO_CC
		distance(dev->get_poll_obj_list().begin(),ite,shared_cmd.index);
#else
		shared_cmd.index = distance(dev->get_poll_obj_list().begin(),ite);
#endif
		
		PollThread *poll_th = th_info->poll_th;
		poll_th->set_local_cmd(shared_cmd);
		poll_th->execute_cmd();
	}
	
//
// Update database property --> Update polling period if this object is already
// defined in the polling property. Add object name and update period if the 
// object is not known in the property
//

	if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
	{
		TangoSys_MemStream s;
		string upd_str;	
		s << (argin->lvalue)[0] << ends;
		s >> upd_str;
	
		DbDatum db_info("polled_attr");		
		if (type == Tango::POLL_CMD)
		{
			db_info.name = "polled_cmd";	
			vector<string> &cmd_list = dev->get_polled_cmd();
			for (i = 0;i < cmd_list.size();i = i+2)
			{
				if (cmd_list[i] == obj_name)
				{
					cmd_list[i + 1] = upd_str;
					break;
				}
			}
			if (i == cmd_list.size())
			{
				cmd_list.push_back(obj_name);
				cmd_list.push_back(upd_str);
			}
			db_info << cmd_list;
		}
		else
		{
			vector<string> &attr_list = dev->get_polled_attr();
			for (i = 0;i < attr_list.size();i = i+2)
			{
				if (attr_list[i] == obj_name)
				{
					attr_list[i + 1] = upd_str;
					break;
				}
			}
			if (i == attr_list.size())
			{
				attr_list.push_back(obj_name);
				attr_list.push_back(upd_str);
			}
			db_info << attr_list;
		}
		
		DbData send_data;
		send_data.push_back(db_info);
		dev->get_db_device()->put_property(send_data);
	}
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::rem_obj_polling()
// 
// description : 	command to remove an already polled object from the device
//			polled object list
//
// in :			The polling parameters :
//				device name
//				object type (command or attribute)
//				object name
//
//-----------------------------------------------------------------------------

void DServer::rem_obj_polling(const Tango::DevVarStringArray *argin,
			      bool with_db_upd)
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In rem_obj_polling method" << endl;
	unsigned long i;
	for (i = 0;i < argin->length();i++)
		cout4 << "Input string = " << (*argin)[i].in() << endl;

//
// Check that parameters number is correct
//

	if (argin->length() != 3)
	{
		Except::throw_exception((const char *)"API_WrongNumberOfArgs",
					(const char *)"Incorrect number of inout arguments",
					(const char *)"DServer::rem_obj_polling");
	}
	
//
// Find the device
//	

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev;
	try
	{
		dev = tg->get_device_by_name((*argin)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (*argin)[0] << " not found" << ends;
		
		Except::re_throw_exception(e,(const char *)"API_DeviceNotFound",o.str(),
				   	   (const char *)"DServer::rem_obj_polling");
	}
	
//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << (*argin)[0] << " is not polled" << ends;
		
		Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   	(const char *)"DServer::rem_obj_polling");
	}
	
//
// Find the wanted object in the list of device polled object
//

	string obj_type((*argin)[1]);
	transform(obj_type.begin(),obj_type.end(),obj_type.begin(),::tolower);
	string obj_name((*argin)[2]);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
	PollObjType type;
	
	if (obj_type == PollCommand)
	{
		type = Tango::POLL_CMD;	
		if ((obj_name == "state") || (obj_name == "status"))
			type = Tango::POLL_ATTR;	
	}
	else if (obj_type == PollAttribute)
	{
		type = Tango::POLL_ATTR;	
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Object type " << obj_type << " not supported" << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"DServer::rem_obj_polling");
	}
		
	vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(type,obj_name);
	long tmp_upd = (*ite)->get_upd();

//
// Find out which thread is in charge of the device.
//

	PollingThreadInfo *th_info;
			
	int poll_th_id = tg->get_polling_thread_id_by_name((*argin)[0]);
	if (poll_th_id == 0)
	{
		TangoSys_OMemStream o;
		o << "Can't find a polling thread for device " << (*argin)[0] << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
						(const char *)"DServer::rem_obj_polling");
	}

	cout4 << "Thread in charge of device " << (*argin)[0] << " is thread " << poll_th_id << endl;
	th_info = tg->get_polling_thread_info_by_id(poll_th_id);
	
//
// Test whether the polling thread is still running!
//

	if ( th_info->poll_th != NULL )
	{
//
// Send command to the polling thread
//

		cout4 << "Sending cmd to polling thread" << endl;
		int interupted;
		TangoMonitor &mon = th_info->poll_mon;
		PollThCmd &shared_cmd = th_info->shared_data;
		{	
			omni_mutex_lock sync(mon);
			if (shared_cmd.cmd_pending == true)
			{
				mon.wait();
			}
			shared_cmd.cmd_pending = true;
			if (tmp_upd == 0)
				shared_cmd.cmd_code = POLL_REM_EXT_TRIG_OBJ;
			else
				shared_cmd.cmd_code = POLL_REM_OBJ;
			shared_cmd.dev = dev;
			shared_cmd.name = obj_name;
			shared_cmd.type = type;

			mon.signal();

			cout4 << "Cmd sent to polling thread" << endl;

//
// Wait for thread to execute command except if the command is
// requested by the polling thread itself
//

			int th_id = omni_thread::self()->id();
			if (th_id != poll_th_id)
			{
				while (shared_cmd.cmd_pending == true)
				{
					interupted = mon.wait(DEFAULT_TIMEOUT);
					if ((shared_cmd.cmd_pending == true) && (interupted == false))
					{
						cout4 << "TIME OUT" << endl;
						Except::throw_exception((const char *)"API_CommandTimedOut",
					        		(const char *)"Polling thread blocked !!!",
					        		(const char *)"DServer::rem_obj_polling");
					}
				}
			}
		}
		cout4 << "Thread cmd normally executed" << endl;
	}
	else
	{
		cout4 << "Polling thread is no longer running!!!!" << endl;
	}

//
// Remove the object from the polled object list
//

	vector<PollObj *> &poll_list = dev->get_poll_obj_list();
	
	dev->get_poll_monitor().get_monitor();
	delete(*ite);
	poll_list.erase(ite);
	dev->get_poll_monitor().rel_monitor();
	
//
// Mark the device as non polled if this was the last polled object
//

	if (poll_list.empty() == true)
		dev->is_polled(false);
					
//
// Update database property. This means remove object entry in the polling 
// properties if they exist or add it to the list of device not polled
// for automatic polling defined at command/attribute level.
// Do this if possible and wanted.
//

	if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
	{
		DbData send_data;
		DbDatum db_info("polled_attr");
		bool update_needed = false;
	
		if (type == Tango::POLL_CMD)
		{
			db_info.name = "polled_cmd";
			vector<string> &cmd_list = dev->get_polled_cmd();
			vector<string>::iterator s_ite;	
			for (s_ite = cmd_list.begin();s_ite < cmd_list.end();++s_ite)
			{
				if (*s_ite == obj_name)
				{
					s_ite = cmd_list.erase(s_ite);
					cmd_list.erase(s_ite);
					db_info << cmd_list;
					update_needed = true;			
					break;
				}
				++s_ite;
			}
			if (update_needed == false)
			{
				vector<string> &non_auto_cmd = dev->get_non_auto_polled_cmd();
				for (s_ite = non_auto_cmd.begin();s_ite < non_auto_cmd.end();s_ite++)
				{
					if (*s_ite == obj_name)
						break;
				}
				if (s_ite == non_auto_cmd.end())
				{
					non_auto_cmd.push_back(obj_name);
					db_info.name = "non_auto_polled_cmd";
					db_info << non_auto_cmd;
					update_needed = true;
				}
			}
		}
		else
		{
			vector<string> &attr_list = dev->get_polled_attr();
			vector<string>::iterator s_ite;	
			for (s_ite = attr_list.begin();s_ite < attr_list.end();++s_ite)
			{
				if (*s_ite == obj_name)
				{
					s_ite = attr_list.erase(s_ite);
					attr_list.erase(s_ite);
					db_info << attr_list;
					update_needed = true;			
					break;
				}
				++s_ite;
			}
			if (update_needed == false)
			{
				vector<string> &non_auto_attr = dev->get_non_auto_polled_attr();
				for (s_ite = non_auto_attr.begin();s_ite < non_auto_attr.end();s_ite++)
				{
					if (*s_ite == obj_name)
						break;
				}
				if (s_ite == non_auto_attr.end())
				{
					non_auto_attr.push_back(obj_name);
					db_info.name = "non_auto_polled_attr";
					db_info << non_auto_attr;
					update_needed = true;
				}
			}
		}

		if (update_needed == true)
		{	
			DbData send_data;
			send_data.push_back(db_info);
			if (db_info.size() == 0)
				dev->get_db_device()->delete_property(send_data);
			else
				dev->get_db_device()->put_property(send_data);
		
			cout4 << "Database polling properties updated" << endl;
		}
	}

//
// If the device is not polled any more, update the pool conf first locally.
// Also update the map<device name,thread id>
// If this device was the only one for a polling thread, kill the thread
// Then in Db if possible
//

	bool kill_thread = false;
	if (poll_list.empty() == true)
	{
		int ind;
		string dev_name((*argin)[0]);
		transform(dev_name.begin(),dev_name.end(),dev_name.begin(),::tolower);
		
		if ((ind = tg->get_dev_entry_in_pool_conf(dev_name)) ==  -1)
		{
			TangoSys_OMemStream o;
			o << "Can't find entry for device " << (*argin)[0] << " in polling threads pool configuration !"<< ends;
			Except::throw_exception((const char *)"API_NotSupported",o.str(),
							(const char *)"DServer::rem_obj_polling");				
		}

		vector<string> &pool_conf = tg->get_poll_pool_conf();
		string &conf_entry = pool_conf[ind];
		string::size_type pos;
		if ((pos = conf_entry.find(',')) != string::npos)
		{
			pos = conf_entry.find(dev_name);
			if ((pos + dev_name.size()) != conf_entry.size())
				conf_entry.erase(pos,dev_name.size() + 1);
			else
				conf_entry.erase(pos - 1);
		}
		else
		{
			vector<string>::iterator iter = pool_conf.begin() + ind;
			pool_conf.erase(iter);
			kill_thread = true;
		}

		tg->remove_dev_from_polling_map(dev_name);
		
//
// Kill the thread if needed and join
//

		if (kill_thread == true)
		{
			TangoMonitor &mon = th_info->poll_mon;
			PollThCmd &shared_cmd = th_info->shared_data;
	
			{
				omni_mutex_lock sync(mon);

				shared_cmd.cmd_pending = true;
				shared_cmd.cmd_code = POLL_EXIT;

				mon.signal();
			}

			void *dummy_ptr;
			cout4 << "POLLING: Joining with one polling thread" << endl;
			th_info->poll_th->join(&dummy_ptr);
		
			tg->remove_polling_thread_info_by_id(poll_th_id);
		}

//
// Update db
//

		if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
		{
			DbData send_data;
			send_data.push_back(DbDatum("polling_threads_pool_conf"));
			send_data[0] << tg->get_poll_pool_conf();
			
			tg->get_dserver_device()->get_db_device()->put_property(send_data);
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::stop_polling()
// 
// description : 	command to stop the polling thread
//
//-----------------------------------------------------------------------------

void DServer::stop_polling()
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In stop_polling method" << endl;
	
//
// Send command to the polling thread and wait for its execution
//

	int interupted;
	Tango::Util *tg = Tango::Util::instance();
	
	vector<PollingThreadInfo *> &th_info = tg->get_polling_threads_info();
	vector<PollingThreadInfo *>:: iterator iter;

	for (iter = th_info.begin();iter != th_info.end();++iter)
	{
		TangoMonitor &mon = (*iter)->poll_mon;
		PollThCmd &shared_cmd = (*iter)->shared_data;

		{	
			omni_mutex_lock sync(mon);
			if (shared_cmd.cmd_pending == true)
			{
				mon.wait();
			}
			shared_cmd.cmd_pending = true;
			shared_cmd.cmd_code = POLL_STOP;
	
			mon.signal();
			
			while (shared_cmd.cmd_pending == true)
			{
				interupted = mon.wait(DEFAULT_TIMEOUT);
	
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << endl;
					Except::throw_exception((const char *)"API_CommandTimedOut",
						        	(const char *)"Polling thread blocked !!!",
						        	(const char *)"DServer::stop_polling");
				}
			}
		}
	}

//
// Update polling status
//

	tg->poll_status(false);
	string &str = get_status();
	str = "The device is ON\nThe polling is OFF";
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::start_polling()
// 
// description : 	command to start the polling thread
//
//-----------------------------------------------------------------------------

void DServer::start_polling()
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In start_polling method" << endl;
	
//
// Send command to the polling thread(s) and wait for its execution
//

	int interupted;
	Tango::Util *tg = Tango::Util::instance();
	
	vector<PollingThreadInfo *> &th_info = tg->get_polling_threads_info();
	vector<PollingThreadInfo *>:: iterator iter;

	for (iter = th_info.begin();iter != th_info.end();++iter)
	{
		TangoMonitor &mon = (*iter)->poll_mon;
		PollThCmd &shared_cmd = (*iter)->shared_data;

		{	
			omni_mutex_lock sync(mon);
			if (shared_cmd.cmd_pending == true)
			{
				mon.wait();
			}
			shared_cmd.cmd_pending = true;
			shared_cmd.cmd_code = POLL_START;
		
			mon.signal();
			
			while (shared_cmd.cmd_pending == true)
			{
				interupted = mon.wait(DEFAULT_TIMEOUT);
	
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << endl;
					Except::throw_exception((const char *)"API_CommandTimedOut",
						        	(const char *)"Polling thread blocked !!!",
						        	(const char *)"DServer::start_polling");
				}
			}
		}
	}
	
//
// Update polling status
//

	tg->poll_status(true);
	string &str = get_status();
	str = "The device is ON\nThe polling is ON";
}

void DServer::start_polling(PollingThreadInfo *th_info)
{
	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;
	int interupted;
	
	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_START;

		mon.signal();

		while (shared_cmd.cmd_pending == true)
		{
			interupted = mon.wait(DEFAULT_TIMEOUT);

			if ((shared_cmd.cmd_pending == true) && (interupted == false))
			{
				cout4 << "TIME OUT" << endl;
				Except::throw_exception((const char *)"API_CommandTimedOut",
						    (const char *)"Polling thread blocked while trying to start thread polling!!!",
						    (const char *)"DServer::start_polling");
			}
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::add_event_heartbeat()
// 
// description : 	command to ask the heartbeat thread to send
//			the event heartbeat every 10 seconds
//
//-----------------------------------------------------------------------------

void DServer::add_event_heartbeat()
{
	NoSyncModelTangoMonitor nosyn_mon(this);

	cout4 << "In add_event_heartbeat method" << endl;

//
// Send command to the heartbeat thread but wait in case of previous cmd 
// still not executed
//

	cout4 << "Sending cmd to polling thread" << endl;
	int interupted;
	Tango::Util *tg = Tango::Util::instance();
	
	TangoMonitor &mon = tg->get_heartbeat_monitor();
	PollThCmd &shared_cmd = tg->get_heartbeat_shared_cmd();

	{	
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_ADD_HEARTBEAT;

		mon.signal();
		
		cout4 << "Cmd sent to polling thread" << endl;

//
// Wait for thread to execute command except if the command is
// requested by the polling thread itself
//

		int th_id = omni_thread::self()->id();
		if (th_id != tg->get_heartbeat_thread_id())
		{
			while (shared_cmd.cmd_pending == true)
			{
				interupted = mon.wait(DEFAULT_TIMEOUT);
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << endl;
					Except::throw_exception((const char *)"API_CommandTimedOut",
					        		(const char *)"Polling thread blocked !!!",
					        		(const char *)"DServer::add_event_heartbeat");
				}
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::rem_event_heartbeat()
// 
// description : 	command to ask the heartbeat thread to stop sending
//			the event heartbeat 
//
//-----------------------------------------------------------------------------

void DServer::rem_event_heartbeat()
{
	NoSyncModelTangoMonitor nosyn_mon(this);

	cout4 << "In rem_event_heartbeat method" << endl;

//
// Send command to the heartbeat thread but wait in case of previous cmd 
// still not executed
//

	cout4 << "Sending cmd to polling thread" << endl;
	int interupted;
	Tango::Util *tg = Tango::Util::instance();	
	TangoMonitor &mon = tg->get_heartbeat_monitor();
	PollThCmd &shared_cmd = tg->get_heartbeat_shared_cmd();

	{	
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_REM_HEARTBEAT;

		mon.signal();
		
		cout4 << "Cmd sent to polling thread" << endl;

//
// Wait for thread to execute command except if the command is
// requested by the polling thread itself
//

		int th_id = omni_thread::self()->id();
		if (th_id != tg->get_heartbeat_thread_id())
		{
			while (shared_cmd.cmd_pending == true)
			{
				interupted = mon.wait(DEFAULT_TIMEOUT);
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << endl;
					Except::throw_exception((const char *)"API_CommandTimedOut",
					        		(const char *)"Polling thread blocked !!!",
					        		(const char *)"DServer::rem_event_heartbeat");
				}
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
}

} // End of Tango namespace

