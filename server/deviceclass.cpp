static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :		Deviceclass.cpp
//
// description :	C++ source code for the DeviceClass
//			class. This class
//			is one the root class for all derived Device classes.
//			It is an abstract class.
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.20  2007/05/17 07:59:07  taurel
// - The polling is not configured via a separate thread any more. The polling thread add_obj_polling method has been modified to support a parameter telling to the polling thread when it has to polled the object.
// Add device name in monitor print message
// Add device_destroyer method in DeviceClass class
//
// Revision 1.19  2007/05/11 07:29:29  taurel
// - Added device_destroyer() methods. These method are doing the opposite of
// the device_factory() one. They remove (and delete) a device from the list
// of available device in the class. They do not mmodify the DB
//
// Revision 1.18  2007/04/20 14:40:29  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 1.17  2007/04/16 14:56:36  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 1.16  2007/03/06 08:18:04  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 1.15  2006/11/20 16:26:37  taurel
// - Small changes when removing servant in case the device is not exported
//
// Revision 1.14  2006/06/21 14:52:19  taurel
// - Just fixing a logging print message
//
// Revision 1.13  2006/06/12 14:22:33  jensmeyer
// Corrected error handling for during initialisation of memorised attributes.
//
// Revision 1.12  2006/05/18 08:52:37  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 1.11  2006/03/09 08:18:06  taurel
// - Change the order of device server destruction. First devices then class.
// It was the contrary....
//
// Revision 1.10  2006/02/20 08:51:48  taurel
// - A small edit mistake in delete_dev method...
//
// Revision 1.9  2006/02/20 08:30:00  taurel
// - Add a new method in DeviceClass to delete a device at run-time
//
// Revision 1.8  2006/02/17 16:53:04  jensmeyer
// Corrections when porting to VC7 under windows
//
// Revision 1.7  2006/01/27 14:27:10  taurel
// - Fix a severe incompatibility problem introduced by all the modifs done for
// PY DS
// - Duplicate some EventSupplier class methods (instead of using template) in order to be able to generate Tango shared library on Suse 9.3
//
// Revision 1.6  2006/01/20 15:30:13  taurel
// - Fixes in the default command management
//
// Revision 1.5  2006/01/20 08:30:05  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 1.4  2005/11/07 12:15:00  jensmeyer
// Corrected initialisation of memorised attributes. No longer exits when receiving an
// exception during initialisation. Also added an option to only initialise the attribute setpoint and not writing to the attribute.
// set_memorized_init(false) -> only applies setpoint.
// set_memorized_init(true) -> also writes setpoint value to the attribute.
//
// Revision 1.3  2005/06/29 08:31:16  taurel
// - Last commit before release 5.2 ?
//
// Revision 1.2  2005/01/13 08:31:12  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 1.1.2.2  2004/10/27 05:59:46  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 1.1.2.1  2004/10/22 11:28:30  taurel
// Splitted device.cpp/device.h files in two sets device.cpp/device.h for the DeviceImpl
// class and deviceclass.cpp/deviceclass.h for the DeviceClass class
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <new>

#include <basiccommand.h>
#include <blackbox.h>
#include <dserversignal.h>
#include <classattribute.h>
#include <eventsupplier.h>

#include <apiexcept.h>

#ifdef TANGO_HAS_LOG4TANGO
#include <logging.h>
#endif

extern omni_thread::key_t key_py_data;
namespace Tango
{


//+----------------------------------------------------------------------------
//
// method :		DeviceClass::DeviceClass(string &s)
// 
// description :	DeviceClass constructor. Protected method which will 
//			be called automatically by the compiler.
//
//-----------------------------------------------------------------------------

DeviceClass::DeviceClass(string &s):name(s)
{

//
// Create the DeviceClassExt instance
//

	ext = new DeviceClassExt;
	
//
// Create the associated DbClass object
//

	db_class = new DbClass(name,Tango::Util::instance()->get_database());
		
//
// initialise command_list with State, Status and Init 
//

	try
	{
		command_list.push_back(new DevStatusCmd("Status",Tango::DEV_VOID,Tango::DEV_STRING));
		command_list[0]->set_out_type_desc("Device status");
		command_list.push_back(new DevStateCmd("State",Tango::DEV_VOID,Tango::DEV_STATE));
		command_list[1]->set_out_type_desc("Device state");
		command_list.push_back(new DevInitCmd("Init",Tango::DEV_VOID,Tango::DEV_VOID));
	}
	catch (bad_alloc)
	{
		if (command_list.empty() == false)
		{
			for(unsigned long i = 0;i < command_list.size();i++)
				delete command_list[i];
			command_list.clear();
		}
		throw;
	}

//
// Retrieve basic class resource
//
	
	get_class_system_resource();
	
//
// Create the multi class attribute object
//

	class_attr = new MultiClassAttribute();

//
// Give a default value for device type
//

	type = DescNotSet;
			
}

//+----------------------------------------------------------------------------
//
// method :		DeviceClass::get_class_system_resource(string &s)
// 
// description :	Method to retrieve some basic class resource(s)
//			The resource to be retrived are :
//				- The class doc URL
//
//-----------------------------------------------------------------------------

void DeviceClass::get_class_system_resource()
{

//
// Try to retrieve the class resource doc_url
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->_UseDb == true)
	{
		DbData db_data;
		
		db_data.push_back(DbDatum("doc_url"));
		db_data.push_back(DbDatum("cvs_tag"));
		db_data.push_back(DbDatum("cvs_location"));

		bool retry = true;
		while (retry == true)
		{		
			try
			{
				db_class->get_property(db_data);
				retry = false;
			}
			catch (Tango::CommunicationFailed &) {}
			catch (Tango::DevFailed &)
			{
				TangoSys_OMemStream o;
				o << "Database error while trying to retrieve properties for class " << name.c_str() << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
						o.str(),
						(const char *)"DeviceClass::get_class_system_resource");
			}
		}
		
		if (db_data[1].is_empty() == false)
			db_data[1] >> ext->cvs_tag;
		if (db_data[2].is_empty() == false)
			db_data[2] >> ext->cvs_location;
					
		if (db_data[0].is_empty() == true)
		{
			cout4 << "doc_url property for class " << name << " is not defined in database" << endl;
			Database *db = tg->get_database();
			try
			{
				db->get_class_property("Default",db_data);
			}
			catch (Tango::DevFailed &)
			{			
				TangoSys_OMemStream o;
				o << "Database error while trying to retrieve properties for class " << name.c_str() << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
							o.str(),
							(const char *)"DeviceClass::get_class_system_resource");
			}			
			
			if (db_data[0].is_empty() == true)
			{
				doc_url = DefaultDocUrl;
			}
			else
				db_data[0] >> doc_url;
		}
		else
			db_data[0] >> doc_url;
			
		
	}
	else
	{
		doc_url = DefaultDocUrl;
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::set_memorized_values()
// 
// description : 	Write the memorized attribute with the value
//			stored in database
//
// in :			all : Flag set to true if memorized values must be 
//			      applied to all class devices
//			idx : Index of the device in the device_list vector
//			      of the device for which memorized values must
//			      be applied. Used if "all" is set to false
//
//-----------------------------------------------------------------------------

void DeviceClass::set_memorized_values(bool all,long idx)
{
	cout4 << "Entering DeviceClass::set_memorized_values() method" << endl;
	
	short sh;
	Tango::DevVarShortArray sh_seq(1);
	sh_seq.length(1);
						
	Tango::DevLong lg;
	Tango::DevVarLongArray lg_seq(1);
	lg_seq.length(1);
		
	double db;
	Tango::DevVarDoubleArray db_seq(1);
	db_seq.length(1);
						
	Tango::DevVarStringArray str_seq(1);
	str_seq.length(1);

	float fl;
	Tango::DevVarFloatArray fl_seq(1);
	fl_seq.length(1);
						
	Tango::DevBoolean boo;
	Tango::DevVarBooleanArray boo_seq(1);
	boo_seq.length(1);
		
	Tango::DevUShort ush;
	Tango::DevVarUShortArray ush_seq(1);
	ush_seq.length(1);

	Tango::DevUChar uch;
	Tango::DevVarCharArray uch_seq(1);
	uch_seq.length(1);
		
//
// Set loop start and stop limits
//

	unsigned long start,stop;
	if (all == true)
	{
		start = 0;
		stop = device_list.size();
	}
	else
	{
		start = idx;
		stop = idx + 1;
	}
								
	for (unsigned long i = start;i < stop;i++)
	{

//
// This feature is available only for devices implementing IDL release 3 and
// above
//

		if (device_list[i]->get_dev_idl_version() < 3)
			continue;
				
//
// Get list of device writable attributes
//

		AttributeValueList att_val(10);
		vector<long> &att_list = device_list[i]->get_device_attr()->get_w_attr_list();
		
		long nb_wr = 0;
		for (unsigned long j = 0;j < att_list.size();j++)
		{
						
			WAttribute &att = device_list[i]->get_device_attr()->get_w_attr_by_ind(att_list[j]);
			
			if (att.is_memorized() == true)
			{

				string &mem_value = att.get_mem_value();
				if (mem_value != MemNotUsed)
				{
					nb_wr++;
					att_val.length(nb_wr);
							
//
// In order to not send
// a new time the already memorized value into db, mark it as not memorized
// before writing the value
//

					att.set_memorized(false);
					
//
// The memorized value gotten from db is a string, we need to convert this string
// to its real type before inserting it into an Any
//

					TangoSys_MemStream str;
					str << mem_value << ends;
					
					try
					{
						switch (att.get_data_type())
						{
						case Tango::DEV_SHORT:
							if (!(str >> sh))
								throw_mem_value(device_list[i],att);
							att.set_write_value(sh);
							
							sh_seq[0] = sh;
							att_val[nb_wr - 1].value <<= sh_seq;
							break;

						case Tango::DEV_LONG:
							if (!(str >> lg))
								throw_mem_value(device_list[i],att);
							att.set_write_value(lg);	
								
							lg_seq[0] = lg;
							att_val[nb_wr - 1].value <<= lg_seq;
							break;

						case Tango::DEV_DOUBLE:
							if (!(str >> db))
								throw_mem_value(device_list[i],att);
							att.set_write_value(db);
							
							db_seq[0] = db;
							att_val[nb_wr - 1].value <<= db_seq;
							break;

						case Tango::DEV_STRING:
							att.set_write_value(mem_value);
							
							str_seq[0] = CORBA::string_dup(mem_value.c_str());
							att_val[nb_wr - 1].value <<= str_seq;
							break;

						case Tango::DEV_FLOAT:
							if (!(str >> fl))
								throw_mem_value(device_list[i],att);
							att.set_write_value(fl);	
								
							fl_seq[0] = fl;
							att_val[nb_wr - 1].value <<= fl_seq;
							break;

						case Tango::DEV_BOOLEAN:
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
							if (!(str >> boolalpha >> boo))
								throw_mem_value(device_list[i],att);
#else
							transform(mem_value.begin(),mem_value.end(),mem_value.begin(),::tolower);
							if (mem_value == "true")
								boo = true;
							else if (mem_value == "false")
								boo = false;
							else
								throw_mem_value(device_list[i],att);
#endif
							att.set_write_value(boo);
							
							boo_seq[0] = boo;
							att_val[nb_wr - 1].value <<= boo_seq;
							break;

						case Tango::DEV_USHORT:
							if (!(str >> ush))
								throw_mem_value(device_list[i],att);
							att.set_write_value(ush);	
								
							ush_seq[0] = ush;
							att_val[nb_wr - 1].value <<= ush_seq;
							break;

						case Tango::DEV_UCHAR:
							if (!(str >> uch))
								throw_mem_value(device_list[i],att);
							att.set_write_value(uch);
								
							uch_seq[0] = uch;
							att_val[nb_wr - 1].value <<= uch_seq;
							break;
						}
						
					// Check the initialisation flag for memorized attributes.
					// The the flag is false, do not add the element to the att_val
					// vector. This avoids a call to write the memorozied value to the attribute.
					
					if ( att.is_memorized_init() == false )
						{
						nb_wr--;
						att_val.length(nb_wr);
						// reset memorized flag
						att.set_memorized(true);
						}
					else
						{
						//
						// Init the AttributeValue structure
						//
					
						att_val[nb_wr - 1].name = CORBA::string_dup(att.get_name().c_str());
						att_val[nb_wr - 1].dim_x = 1;
						att_val[nb_wr - 1].dim_y = 0;						
						att_val[nb_wr - 1].quality = Tango::ATTR_VALID;
						}
					}
						
					catch (Tango::DevFailed &e)
					{
						cout3 << "Cannot configure setpoint value for memorized attribute " << att.get_name() << endl;
						Tango::Except::print_exception(e);
						
						nb_wr--;
						att_val.length(nb_wr);
						// reset memorized flag
						att.set_memorized(true);
					}						
				}
			}
		}
			
																	


		if (nb_wr != 0)
		{
		
		//
		// Write attribute values. Re-throw exception if any but reset memorized flags before the
		// re-throw.
		//
			
			try
			{
				cout4 << "Writing data for " << att_val.length() << " attribute(s) for device " << device_list[i]->get_name() << endl;
				(static_cast<Device_3Impl *>(device_list[i]))->write_attributes_3(att_val);
			}
			catch (Tango::DevFailed &e)
			{
				cout3 << "Cannot write setpoint(s) value for memorized attribute(s) " << device_list[i]->get_name() << endl;
				Tango::Except::print_exception(e);

				//for (unsigned long k = 0;k < att_val.length();k++)
				//{
				//	WAttribute &att = device_list[i]->get_device_attr()->get_w_attr_by_name(att_val[k].name.in());
				//	att.set_memorized(true);
				//}
				//throw;
			}
			catch (Tango::MultiDevFailed &e)
			{
				cout3 << "Cannot write setpoint(s) value for memorized attribute(s) " << device_list[i]->get_name() << endl;
				
				Tango::NamedDevFailedList e_list (e, device_list[i]->get_name(), (const char *)"DeviceClass::set_memorized_values()",
					       			(const char *)"API_AttributeFailed");
				Tango::Except::print_exception(e_list);

				//for (unsigned long k = 0;k < att_val.length();k++)
				//{
				//	WAttribute &att = device_list[i]->get_device_attr()->get_w_attr_by_name(att_val[k].name.in());
				//	att.set_memorized(true);
				//}
				//throw Tango::NamedDevFailedList(e,device_list[i]->get_name(),
				//	       			(const char *)"DeviceClass::set_memorized_values()",
				//	       			(const char *)"API_AttributeFailed");
			}
						
			//
			// Reset memorized flags
			//

			for (unsigned long k = 0;k < att_val.length();k++)
			{
				WAttribute &att = device_list[i]->get_device_attr()->get_w_attr_by_name(att_val[k].name.in());
				att.set_memorized(true);
			}
		}
	}

	cout4 << "Leaving DeviceClass::set_memorized_values() method" << endl;
	
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::throw_mem_value()
// 
// description : 	Write the memorized attribute with the value
//			stored in database
//
//-----------------------------------------------------------------------------

void DeviceClass::throw_mem_value(DeviceImpl *dev,Attribute &att)
{
	TangoSys_OMemStream o;
	
	o << "Memorized value for attribute ";
	o << att.get_name();
	o << " (device ";
	o << dev->get_name();
	o << ") is in an incorrect format !" << ends;
	
	Except::throw_exception((const char *)"API_AttrWrongMemValue",
				o.str(),
				(const char *)"DeviceClass::set_memorized_values");
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::~DeviceClass(string &s)
// 
// description :	DeviceClass destructor.
//
//-----------------------------------------------------------------------------

DeviceClass::~DeviceClass()
{
	cout4 << "Entering DeviceClass destructor for class " << name << endl;

//
// Destroy the DbClass object
//

	delete db_class;
		
//
// Destroy the device list
//

	unsigned long i;
	if (device_list.size() != 0)
	{
		Tango::Util *tg = Tango::Util::instance();
		PortableServer::POA_ptr r_poa = tg->get_poa();

		for (i = 0;i < device_list.size();i++)
		{	
			delete_dev(i,tg,r_poa);		
		}
		device_list.clear();
		CORBA::release(r_poa);
	}

//
// Destroy the command list
//

	for (i = 0;i < command_list.size();i++)
		delete command_list[i];
	command_list.clear();

//
// Destroy the MultiClassAttribute object
//

	delete class_attr;
		
//
// Unregister the class from signal handler
//

	DServerSignal::instance()->unregister_class_signal(this);
	
//
// Delete the class extension object
//

	delete ext;
	
	cout4 << "Leaving DeviceClass destructor for class " << name << endl;	
}


//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::delete_dev()
// 
// description :	delete a device from the class device list
//
//-----------------------------------------------------------------------------


void DeviceClass::delete_dev(long idx,Tango::Util *tg,PortableServer::POA_ptr r_poa)
{
	cout4 << "Entering DeviceClas::delete_dev method for device with index " << idx << endl;

//
// If the polling thread is alive and if device is polled,
// ask polling thread to stop polling
//

	if ((tg->get_polling_thread_object() != NULL) && (device_list[idx]->is_polled() == true))
		device_list[idx]->stop_polling();
	
//
// Deactivate the CORBA object
//

	if (device_list[idx]->get_exported_flag() == true)
		r_poa->deactivate_object(device_list[idx]->get_obj_id().in());
			
//
// Unfortunately, the deactivate_object on the POA immediately returns even if there
// is a running call (see Henning/Vinoski page 500). Therefore, try to get the
// object monitor which will be free only when the last running call will finish
// Two cases have to be taken into account :
// 1 - The running commands : The get/rel on the monitor will make the thread
//     waiting for them
// 2- BUT, there are also (for very demanding clients) cases where commands are
//    registered by the ORB but the monitor owner. For this case, sleep a while
//    to give them time to execute. But sleep how long ?
//    By default, I choose 200 mS
// May be there is a more CORBA way to do this.
// Need to investigate
//
	omni_thread::value_t *tmp_py_data = omni_thread::self()->get_value(key_py_data);
	PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;		

#ifdef _TG_WINDOWS_
	try
	{
		lock_ptr->Release();

		device_list[idx]->get_dev_monitor().get_monitor();	
		device_list[idx]->get_dev_monitor().rel_monitor();
		
		Sleep(200);

		lock_ptr->Get();
	}
#else
	try
	{
		lock_ptr->Release();

		device_list[idx]->get_dev_monitor().get_monitor();
		device_list[idx]->get_dev_monitor().rel_monitor();

		struct timespec wait_to_sleep,still_wait;
		wait_to_sleep.tv_sec = 0;
		wait_to_sleep.tv_nsec = 200000000;

		nanosleep(&wait_to_sleep,&still_wait);

		lock_ptr->Get();
	}
#endif /* _TG_WINDOWS_ */
	catch (Tango::DevFailed &)
	{
	}
		
//
// Remove the C++ servant
//

	if (device_list[idx]->is_py_device() == false)
	{
		if (device_list[idx]->get_exported_flag() == true)
			device_list[idx]->_remove_ref();
		else
		{
			Device_3Impl *dev_3 = static_cast<Device_3Impl *>(device_list[idx]);
			dev_3->delete_dev();
		}
	}		
	else
	{
		Device_3Impl *dev_3 = static_cast<Device_3Impl *>(device_list[idx]);
		dev_3->delete_dev();
	}
	
	cout4 << "Leaving DeviceClass delete_dev" << endl;	
}


//+-------------------------------------------------------------------------
//
// method :		DeviceClass::register_signal
// 
// description :	Method to register a class on a signal. When the
//			signal is sent to the process, the signal_handler
//			method of this class will be executed
//
// in : 		signo : The signal number
//			delayed : The boolean delayed flag
//
//--------------------------------------------------------------------------

#if !(defined __linux)
void DeviceClass::register_signal(long signo)
{
	cout4 << "DeviceClass::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_class_signal(signo,this);
		
	cout4 << "Leaving DeviceClass::register_signal method()" << endl;
}
#else
void DeviceClass::register_signal(long signo,bool handler)
{
	cout4 << "DeviceClass::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_class_signal(signo,handler,this);
		
	cout4 << "Leaving DeviceClass::register_signal method()" << endl;
}
#endif

//+-------------------------------------------------------------------------
//
// method :		DeviceClass::unregister_signal
// 
// description :	Method to unregister a class on a signal.
//
// in : 		signo : The signal number
//			delayed : The boolean delayed flag
//
//--------------------------------------------------------------------------

void DeviceClass::unregister_signal(long signo)
{
	cout4 << "DeviceClass::unregister_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->unregister_class_signal(signo,this);
		
	cout4 << "Leaving DeviceClass::unregister_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceClass::signal_handler
// 
// description :	This is the signal handler for the class. This method
//			is defined as virtual and therefore, can be redefined
//			by DS programmers in their own classes derived from
//			DeviceClass class
//
// in : 		signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceClass::signal_handler(long signo)
{
	cout4 << "DeviceClass::signal_handler() arrived for signal " << signo << endl;
	
	cout4 << "Leaving DeviceClass::signal_handler method()" << endl;
}


//+-------------------------------------------------------------------------
//
// method :		DeviceClass::export_device()
// 
// description :	This method exports a device to the outside world. 
//			This is done by sending its CORBA network parameter
//			(mainly the IOR) to the Tango database
//
// in : 		dev : The device to be exported
//			corba_obj_name : The CORBA object name associated
//					 with the device. A default value
//					 is provided. This field is mainly
//					 use for specific device server
//					 like the database device server.
//
//--------------------------------------------------------------------------

void DeviceClass::export_device(DeviceImpl *dev,const char *corba_obj_name)
{
	cout4 << "DeviceClass::export_device() arrived" << endl;

	Device_var d;
		
	if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
	{
			
// 
// Activate the CORBA object incarnated by the dev C++ object
//

		d = dev->_this();
		dev->set_d_var(Tango::Device::_duplicate(d));

//
// Store the ObjectId (The ObjectId_var type is a typedef of a string_var
// type)
//

		PortableServer::ObjectId_var oid;
		try
		{
			PortableServer::POA_ptr r_poa = Util::instance()->get_poa();
			oid = r_poa->reference_to_id(d);
			CORBA::release(r_poa);
		}
		catch (...)
		{
			TangoSys_OMemStream o;
			o << "Cant get CORBA reference Id for device " << dev->get_name() << ends;
			Except::throw_exception((const char *)"API_CantGetDevObjectId",
					      o.str(),
					      (const char *)"DeviceClass::export_device");
		}
		dev->set_obj_id(oid);
	}
	else
	{
	
//
// For server started without db usage (Mostly the database server). In this case,
// it is necessary to create our own CORBA object id and to bind it into the
// OOC Boot Manager for access through a stringified object reference 
// constructed using the corbaloc style
// The API will try to connect to device using lower case letters.
// Register device in POA with lower case letters
//

		string corba_obj_name_lower(corba_obj_name);
		transform(corba_obj_name_lower.begin(),
			  corba_obj_name_lower.end(),
			  corba_obj_name_lower.begin(),
			  ::tolower);
		PortableServer::ObjectId_var id = PortableServer::string_to_ObjectId(corba_obj_name_lower.c_str());
		try
		{
			PortableServer::POA_ptr r_poa = Util::instance()->get_poa();
			r_poa->activate_object_with_id(id.in(),dev);
			CORBA::release(r_poa);
		}
		catch (...)
		{
			TangoSys_OMemStream o;
			o << "Can't get CORBA reference Id for device " << dev->get_name() << ends;
			Except::throw_exception((const char *)"API_CantGetDevObjId",
						o.str(),
						(const char *)"DeviceClass::export_device");
		}
		
		d = dev->_this();
		dev->set_obj_id(id);		
		dev->set_d_var(Tango::Device::_duplicate(d));		
	}
			
//
// Prepare sent parameters and allocate mem for them
//

	if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
	{
		Tango::Util *tg = Tango::Util::instance();
		CORBA::ORB_ptr orb_ptr = tg->get_orb(); 
		
		char *s = orb_ptr->object_to_string(d);
		string ior_string(s);

		Tango::DbDevExportInfo exp;
			
		exp.name = dev->get_name();
		exp.ior = ior_string;
		exp.host = tg->get_host_name();
		exp.pid = tg->get_pid();
		exp.version = tg->get_version_str();

//
// Call db server
//

		bool retry = true;
		while (retry == true)
		{
			try
			{
				tg->get_database()->export_device(exp);
				retry = false;
			}
			catch (Tango::CommunicationFailed &) {}
		}
		
		CORBA::release(orb_ptr);
		CORBA::string_free(s);
	}
	
//
// Set the DeviceImpl exported flag to true
//

	dev->set_exported_flag(true);
			
	cout4 << "Leaving DeviceClass::export_device method()" << endl;
}


//+----------------------------------------------------------------------------
//
// method :		DeviceClass::command_handler(string &s)
// 
// description :	Command handler which is called by Device
//			when a command is received. It will check
//			to see if the command is implemented. If
//			so it will test to see if it is allowed
//			in this state. Finally it will execute the
//			command by calling its execute method.
//			If an error occurs it will throw a DevFailed
//			exception.
//
//-----------------------------------------------------------------------------

CORBA::Any *DeviceClass::command_handler(DeviceImpl *device,string &command,const CORBA::Any &in_any)
{	
	CORBA::Any *ret = NULL;
	vector<Command *>::iterator i_cmd;
	
	bool with_default_cmd = false;
	string command_lower(command);

	cout4 << "Entering DeviceClass::command_handler() method" << endl;

	transform(command_lower.begin(),command_lower.end(),command_lower.begin(),::tolower);
	
	for (i_cmd = command_list.begin();i_cmd < command_list.end();++i_cmd)
	{
		if ((*i_cmd)->get_lower_name() == command_lower)
		{

//
// Call the always executed method
//

			device->always_executed_hook();

//
// Check if command is allowed
//

			if ((*i_cmd)->is_allowed(device,in_any) == false)
			{
				TangoSys_OMemStream o;
				o << "Command " << command << " not allowed when the device is in " << Tango::DevStateName[device->get_state()] << " state"  << ends;
				Except::throw_exception((const char *)"API_CommandNotAllowed",
						      o.str(),
						      (const char *)"DeviceClass::command_handler");
			}

//
// Execute command
//

			ret = (*i_cmd)->execute(device,in_any);
			break;
		}
	}

	if (i_cmd == command_list.end())
	{
	
		cout3 << "DeviceClass::command_handler(): command " << command << " not found" << endl;


		Command *def_cmd = get_default_command();
		if (def_cmd != NULL)
		{
		
//
// Set name in default command object
//

			def_cmd->set_name(command);
					
//
// Call the always executed method
//

			device->always_executed_hook();

//
// Check if command is allowed
//

			if (def_cmd->is_allowed(device,in_any) == false)
			{
				TangoSys_OMemStream o;
				o << "Command " << command << " not allowed when the device is in " << Tango::DevStateName[device->get_state()] << " state"  << ends;
				Except::throw_exception((const char *)"API_CommandNotAllowed",
						o.str(),
						(const char *)"DeviceClass::command_handler");
			}
	
//
// Execute command
//

			ret = def_cmd->execute(device,in_any);

		}
		else
		{
		
					
//		
// Throw an exception to client
//

			TangoSys_OMemStream o;

			o << "Command " << command << " not found" << ends;
			Except::throw_exception((const char *)"API_CommandNotFound",
					      o.str(),
					      (const char *)"DeviceClass::command_handler");
		}
	}
	
	cout4 << "Leaving DeviceClass::command_handler() method" << endl;	
	return ret;
}


//+----------------------------------------------------------------------------
//
// method :		DeviceClass::add_wiz_dev_prop()
// 
// description :	Method to add a device property definition for the
//			device  wizard
//
// in : 		name : The device property name
//			desc : The device property description
//			def : The device property default value
//
//-----------------------------------------------------------------------------

void DeviceClass::add_wiz_dev_prop(string &p_name,string &desc,string &def)
{

//
// Name in lowercase letters
//

	string name_low = p_name;
	transform(name_low.begin(),name_low.end(),name_low.begin(),::tolower);
	
//
// Check that this property is not already in the vector
//

	vector<string>::iterator ite;
	for (ite = wiz_dev_prop.begin();ite < wiz_dev_prop.end();++ite)
	{
		string tmp_name(*ite);
		transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		if (tmp_name == name_low)
			break;
	}
	
	if (ite != wiz_dev_prop.end())
	{
		TangoSys_OMemStream o;
		o << "Device property " << p_name;
		o << " for class " << name << " is already defined in the wizard" << ends;
		Except::throw_exception((const char *)"API_WizrdConfError",
					o.str(),
					(const char *)"DeviceClass::add_wiz_dev_prop");
	}
	
//
// Insert data in vector
//

	wiz_dev_prop.push_back(p_name);
	wiz_dev_prop.push_back(desc);
	wiz_dev_prop.push_back(def);

}


void DeviceClass::add_wiz_dev_prop(string &p_name,string &desc)
{
	string def(AlrmValueNotSpec);
	add_wiz_dev_prop(p_name,desc,def);
}


//+----------------------------------------------------------------------------
//
// method :		DeviceClass::add_wiz_class_prop()
// 
// description :	Method to add a class property definition for the
//			device  wizard
//
// in : 		name : The class property name
//			desc : The class property description
//			def : The class property default value
//
//-----------------------------------------------------------------------------

void DeviceClass::add_wiz_class_prop(string &p_name,string &desc,string &def)
{
//
// Name in lowercase letters
//

	string name_low = p_name;
	transform(name_low.begin(),name_low.end(),name_low.begin(),::tolower);
	
//
// Check that this property is not already in the vector
//

	vector<string>::iterator ite;
	for (ite = wiz_class_prop.begin();ite < wiz_class_prop.end();++ite)
	{
		string tmp_name(*ite);
		transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		if (tmp_name == name_low)
			break;
	}

	if (ite != wiz_class_prop.end())
	{
		TangoSys_OMemStream o;
		o << "Class property " << p_name;
		o << " for class " << name << " is already defined in the wizard" << ends;
		Except::throw_exception((const char *)"API_WizrdConfError",
					o.str(),
					(const char *)"DeviceClass::add_wiz_dev_prop");
	}
	
//
// Insert data in vector
//

	wiz_class_prop.push_back(p_name);
	wiz_class_prop.push_back(desc);
	wiz_class_prop.push_back(def);
}


void DeviceClass::add_wiz_class_prop(string &p_name,string &desc)
{
	string def(AlrmValueNotSpec);
	add_wiz_class_prop(p_name,desc,def);
}

//+----------------------------------------------------------------------------
//
// method :		DeviceClass::device_destroyer
// 
// description :	Method to remove and delete a device from the running devices
//					belonging to the Tango class
//
// in : 	dev_name : The device name
//
//-----------------------------------------------------------------------------

void DeviceClass::device_destroyer(const string &dev_name)
{
	
//
// Check that the class know this device
//
  
  	unsigned long k;				
  	for (k = 0;k < device_list.size();k++)
  	{
  		if (device_list[k]->get_name() == dev_name)
  			break;
  	}
  
  	if (k == device_list.size())
  	{
		TangoSys_OMemStream o;
		o << "Device " << dev_name << " not in Tango class device list!" << ends;
		
  		Tango::Except::throw_exception((const char *)"API_CantDestroyDevice",o.str(),
  					       (const char *)"DeviceClass::device_destroyer");
  	}
	
//
// Check if the device is polled
// If yes, ask polling thread to stop polling it
//

	if (device_list[k]->is_polled() == true)
	{
		device_list[k]->stop_polling();
	}
	
			
//
// Delete the device
//
 
	Tango::Util *tg = Tango::Util::instance();
	 			
	PortableServer::POA_ptr r_poa = tg->get_poa();
	delete_dev(k,tg,r_poa);
	vector<DeviceImpl *>::iterator it = device_list.begin();
	it += k;
	device_list.erase(it);
	CORBA::release(r_poa);  	  				
}

void DeviceClass::device_destroyer(const char *dev_name)
{
	string name_str(dev_name);
	return device_destroyer(name_str);
}

} // End of Tango namespace
