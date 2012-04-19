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
// $Revision$
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <new>
#include <iterator>

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

static void lower_cmd_name(string &cmd)
{
	transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);
}

//+----------------------------------------------------------------------------
//
// method :		DeviceClass::DeviceClass(string &s)
//
// description :	DeviceClass constructor. Protected method which will
//			be called automatically by the compiler.
//
//-----------------------------------------------------------------------------

DeviceClass::DeviceClass(string &s):name(s),ext(new DeviceClassExt)
{

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

	type = NotSet;

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
		Database *db = tg->get_database();
		DbData db_data;

		db_data.push_back(DbDatum("doc_url"));
		db_data.push_back(DbDatum("cvs_tag"));
		db_data.push_back(DbDatum("cvs_location"));
		db_data.push_back(DbDatum("AllowedAccessCmd"));
		db_data.push_back(DbDatum("svn_tag"));
		db_data.push_back(DbDatum("svn_location"));

		try
		{
			db->get_class_property(name,db_data,tg->get_db_cache());
		}
		catch (Tango::DevFailed &)
		{
			TangoSys_OMemStream o;
			o << "Database error while trying to retrieve properties for class " << name.c_str() << ends;

			Except::throw_exception((const char *)"API_DatabaseAccess",
					o.str(),
					(const char *)"DeviceClass::get_class_system_resource");
		}

		if (db_data[1].is_empty() == false)
			db_data[1] >> ext->cvs_tag;
		if (db_data[2].is_empty() == false)
			db_data[2] >> ext->cvs_location;

//
// Init allowed commands vector (in lowercase letters)
//

		if (db_data[3].is_empty() == false)
		{
			db_data[3] >> allowed_cmds;
			for_each(allowed_cmds.begin(),allowed_cmds.end(),lower_cmd_name);
		}

		if (db_data[0].is_empty() == true)
		{
			cout4 << "doc_url property for class " << name << " is not defined in database" << endl;
			try
			{
				db->get_class_property("Default",db_data,tg->get_db_cache());
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

		if (db_data[4].is_empty() == false)
			db_data[4] >> ext->svn_tag;
		if (db_data[5].is_empty() == false)
			db_data[5] >> ext->svn_location;

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

void DeviceClass::set_memorized_values(bool all,long idx,bool from_init)
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

	Tango::DevString tmp_str;
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

	Tango::DevULong ulg;
	Tango::DevVarULongArray ulg_seq(1);
	ulg_seq.length(1);

	Tango::DevLong64 lg64;
	Tango::DevVarLong64Array lg64_seq(1);
	lg64_seq.length(1);

	Tango::DevULong64 ulg64;
	Tango::DevVarULong64Array ulg64_seq(1);
	ulg64_seq.length(1);

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
					if (from_init == false)
						str << mem_value << ends;

					try
					{
						switch (att.get_data_type())
						{
						case Tango::DEV_SHORT:
							if (from_init == false)
							{
								if (!(str >> sh))
									throw_mem_value(device_list[i],att);
								att.set_write_value(sh);
							}
							else
								att.get_write_value(sh);

							sh_seq[0] = sh;
							att_val[nb_wr - 1].value <<= sh_seq;
							break;

						case Tango::DEV_LONG:
							if (from_init == false)
							{
								if (!(str >> lg))
									throw_mem_value(device_list[i],att);
								att.set_write_value(lg);
							}
							else
								att.get_write_value(lg);

							lg_seq[0] = lg;
							att_val[nb_wr - 1].value <<= lg_seq;
							break;

						case Tango::DEV_DOUBLE:
							if (from_init == false)
							{
								if (!(str >> db))
									throw_mem_value(device_list[i],att);
								att.set_write_value(db);
							}
							else
								att.get_write_value(db);

							db_seq[0] = db;
							att_val[nb_wr - 1].value <<= db_seq;
							break;

						case Tango::DEV_STRING:
							if (from_init == false)
							{
								att.set_write_value(mem_value);

								str_seq[0] = CORBA::string_dup(mem_value.c_str());
							}
							else
							{
								att.get_write_value(tmp_str);
								str_seq[0] = CORBA::string_dup(tmp_str);
							}
							att_val[nb_wr - 1].value <<= str_seq;
							break;

						case Tango::DEV_FLOAT:
							if (from_init == false)
							{
								if (!(str >> fl))
									throw_mem_value(device_list[i],att);
								att.set_write_value(fl);
							}
							else
								att.get_write_value(fl);

							fl_seq[0] = fl;
							att_val[nb_wr - 1].value <<= fl_seq;
							break;

						case Tango::DEV_BOOLEAN:
							if (from_init == false)
							{
								if (!(str >> boolalpha >> boo))
									throw_mem_value(device_list[i],att);
								att.set_write_value(boo);
							}
							else
								att.get_write_value(boo);

							boo_seq[0] = boo;
							att_val[nb_wr - 1].value <<= boo_seq;
							break;

						case Tango::DEV_USHORT:
							if (from_init == false)
							{
								if (!(str >> ush))
									throw_mem_value(device_list[i],att);
								att.set_write_value(ush);
							}
							else
								att.get_write_value(ush);

							ush_seq[0] = ush;
							att_val[nb_wr - 1].value <<= ush_seq;
							break;

						case Tango::DEV_UCHAR:
							if (from_init == false)
							{
								if (!(str >> uch))
									throw_mem_value(device_list[i],att);
								att.set_write_value(uch);
							}
							else
								att.get_write_value(uch);

							uch_seq[0] = uch;
							att_val[nb_wr - 1].value <<= uch_seq;
							break;

						case Tango::DEV_ULONG:
							if (from_init == false)
							{
								if (!(str >> ulg))
									throw_mem_value(device_list[i],att);
								att.set_write_value(ulg);
							}
							else
								att.get_write_value(ulg);

							ulg_seq[0] = ulg;
							att_val[nb_wr - 1].value <<= ulg_seq;
							break;

						case Tango::DEV_LONG64:
							if (from_init == false)
							{
								if (!(str >> lg64))
									throw_mem_value(device_list[i],att);
								att.set_write_value(lg64);
							}
							else
								att.get_write_value(lg64);

							lg64_seq[0] = lg64;
							att_val[nb_wr - 1].value <<= lg64_seq;
							break;

						case Tango::DEV_ULONG64:
							if (from_init == false)
							{
								if (!(str >> ulg64))
									throw_mem_value(device_list[i],att);
								att.set_write_value(ulg64);
							}
							else
								att.get_write_value(ulg64);

							ulg64_seq[0] = ulg64;
							att_val[nb_wr - 1].value <<= ulg64_seq;
							break;
						}

//
// Check the initialisation flag for memorized attributes.
// The the flag is false, do not add the element to the att_val
// vector. This avoids a call to write the memorozied value to the attribute.
//

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

		unsigned long nb_dev = device_list.size();
		for (i = 0;i < nb_dev;i++)
		{

//
// Clear vectors used to memorize info used to clean db
// in case of devices with dyn attr removed during device
// destruction
//

			tg->get_polled_dyn_attr_names().clear();
			tg->get_full_polled_att_list().clear();
			tg->get_all_dyn_attr_names().clear();
			tg->get_dyn_att_dev_name().clear();

//
// Delete device
//

			delete_dev(0,tg,r_poa);

//
// Clean-up db (dyn attribute)
//

			if (tg->get_polled_dyn_attr_names().size() != 0)
				tg->clean_attr_polled_prop();
			if (tg->get_all_dyn_attr_names().size() != 0)
				tg->clean_dyn_attr_prop();

			vector<DeviceImpl *>::iterator it = device_list.begin();
			device_list.erase(it);
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

#ifndef HAS_UNIQUE_PTR
	delete ext;
#endif

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

	if ((tg->get_polling_threads_info().empty() == false) && (device_list[idx]->is_polled() == true))
	{
		device_list[idx]->stop_polling(false);
	}

//
// Deactivate the CORBA object
//

	bool py_dev = device_list[idx]->is_py_device();
	bool exported_device = device_list[idx]->get_exported_flag();

	if (exported_device == true)
		r_poa->deactivate_object(device_list[idx]->get_obj_id().in());

//
// Remove the servant.
// For C++ device, this will be automatically done by the POA when the last executing call
// will be over even if the device is not exported
//

	if (py_dev == true)
	{
		Device_3Impl *dev_3 = static_cast<Device_3Impl *>(device_list[idx]);
		dev_3->delete_dev();
	}

//
// Wait for CORBA to call the device dtor
//

    if (device_list[idx] != NULL && exported_device == true)
    {
#ifdef _TG_WINDOWS_
        while (device_list[idx] != NULL)
        {
            Sleep(10);
        }
#else
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10000000;

        while (device_list[idx] != NULL)
        {
            nanosleep(&ts,NULL);
        }
#endif
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
// Also call _remove_ref to give POA the full ownership of servant
//

		d = dev->_this();
		dev->set_d_var(Tango::Device::_duplicate(d));
		if (is_py_class() == false)
			dev->_remove_ref();

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
		dev->_remove_ref();
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
// We are still in the server starting phase. Therefore, the db timeout is still high (13 sec the 07/01/2011)
// with 3 retries in case of timeout
//

		try
		{
			tg->get_database()->export_device(exp);
		}
		catch (Tango::CommunicationFailed &)
		{
			cerr << "CommunicationFailed while exporting device " << dev->get_name() << endl;
			CORBA::release(orb_ptr);
			CORBA::string_free(s);
			throw;
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

//+----------------------------------------------------------------------------
//
// method :		DeviceClass::is_command_allowed
//
// description :	Method to check if a command is allowed even if the device
//					is locked by another client. It follows the definition
//					of the Tango control access system to define what is an
//					allowed command
//
// in : 	cmd : The command name
//
//-----------------------------------------------------------------------------

bool DeviceClass::is_command_allowed(const char *cmd)
{
	bool ret = true;

	string tmp_cmd(cmd);
	transform(tmp_cmd.begin(),tmp_cmd.end(),tmp_cmd.begin(),::tolower);

	vector<string>::iterator pos = find(allowed_cmds.begin(),allowed_cmds.end(),tmp_cmd);
	if (pos == allowed_cmds.end())
		ret = false;

	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::get_mcast_event()
//
// description : 	Get for all class devices and for all attributes multicast
//					event parameters (if any)
//
// in :	dserv : Pointer to the DServer device
//
//-----------------------------------------------------------------------------

void DeviceClass::get_mcast_event(DServer *dserv)
{
	cout4 << "Entering DeviceClass::get_mcast_event() method" << endl;
	vector<string> m_cast;

	for (unsigned int i = 0;i < device_list.size();++i)
	{
		vector<Attribute *> &att_list = device_list[i]->get_device_attr()->get_attribute_list();
		for (unsigned int j = 0;j < att_list.size();++j)
		{
			dserv->mcast_event_for_att(device_list[i]->get_name_lower(),att_list[j]->get_name_lower(),m_cast);
			if (m_cast.size() != 0)
				att_list[j]->set_mcast_event(m_cast);
		}
	}
}

//+----------------------------------------------------------------------------
//
// method :		DeviceClass::get_cmd_by_name
//
// description :	Get a reference to the Command object
//
// in : 	cmd_name : The command name
//
//-----------------------------------------------------------------------------

Command &DeviceClass::get_cmd_by_name(const string &cmd_name)
{
	vector<Command *>::iterator pos;

#ifdef HAS_LAMBDA_FUNC
    pos = find_if(command_list.begin(),command_list.end(),
                    [&] (Command *cmd) -> bool
                    {
                        if (cmd_name.size() != cmd->get_lower_name().size())
                            return false;
                        string tmp_name(cmd_name);
                        transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
                        return cmd->get_lower_name() == tmp_name;
                    });
#else
	pos = find_if(command_list.begin(),command_list.end(),
		      bind2nd(WantedCmd<Command *,const char *,bool>(),cmd_name.c_str()));
#endif

	if (pos == command_list.end())
	{
		cout3 << "DeviceClass::get_cmd_by_name throwing exception" << endl;
		TangoSys_OMemStream o;

		o << cmd_name << " command not found" << ends;
		Except::throw_exception((const char *)"API_CommandNotFound",
				      o.str(),
				      (const char *)"DeviceClass::get_cmd_by_name");
	}

	return *(*pos);
}

} // End of Tango namespace
