static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               BasicCommand.cpp
//
// description :        C++ source code for commands which are automatically
//			installed for every devices
//			Three commands are :
//				DevState, DevStatus, DevRestart
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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
#include <basiccommand.h>

extern omni_thread::key_t key_py_data;

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		DevStatusCmd::DevStatusCmd
//
// description : 	constructor for Command class Status
//
//--------------------------------------------------------------------------

DevStatusCmd::DevStatusCmd(const char *name,Tango::CmdArgType in,Tango::CmdArgType out)
:Command(name,in,out)
{
}


//+-------------------------------------------------------------------------
//
// method : 		DevStatusCmd::execute
//
// description : 	return status as string
//
//--------------------------------------------------------------------------

CORBA::Any *DevStatusCmd::execute(DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevStatus::execute(): arrived " << endl;

//
// return status string as Any
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevStatus::execute");
	}

	try
	{
	    if (device->is_alarm_state_forced() == true)
            (*out_any) <<= device->DeviceImpl::dev_status();
        else
            (*out_any) <<= device->dev_status();
	}
	catch(...)
	{
		delete out_any;
		throw;
	}

	cout4 << "Leaving DevStatus::execute()" << endl;
	return out_any;

}

//+-------------------------------------------------------------------------
//
// method : 		DevStateCmd::DevStateCmd
//
// description : 	constructor for Command class State
//
//--------------------------------------------------------------------------

DevStateCmd::DevStateCmd(const char *name,Tango::CmdArgType in, Tango::CmdArgType out)
:Command(name,in,out)
{
}

//+-------------------------------------------------------------------------
//
// method : 		StateCmd::execute
//
// description : 	return state as enumerated type
//
//--------------------------------------------------------------------------

CORBA::Any *DevStateCmd::execute(DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevState::execute(): arrived" << endl;

//
// return state as Any
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevStatus::execute");
	}

	try
	{
	    if (device->is_alarm_state_forced() == true)
            (*out_any) <<= device->DeviceImpl::dev_state();
        else
            (*out_any) <<= device->dev_state();

	}
	catch(...)
	{
		delete out_any;
		throw;
	}

	cout4 << "Leaving DevState::execute()" << endl;
	return out_any;
}

//+-------------------------------------------------------------------------
//
// method : 		DevStateCmd::DevInitCmd
//
// description : 	constructor for Command class Init
//
//--------------------------------------------------------------------------

DevInitCmd::DevInitCmd(const char *name,Tango::CmdArgType in, Tango::CmdArgType out)
:Command(name,in,out)
{
}

//+-------------------------------------------------------------------------
//
// method : 		InitCmd::execute
//
// description : 	Initialize a device
//
//--------------------------------------------------------------------------

CORBA::Any *DevInitCmd::execute(DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "Init::execute(): arrived" << endl;

//
// Init device
//

	Tango::Util *tg = Tango::Util::instance();
	omni_thread *th;
	PyLock *lock_ptr = NULL;

	try
	{
		NoSyncModelTangoMonitor mon(device);

		if (tg->is_py_ds())
		{
			th = omni_thread::self();

			omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
			lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
			lock_ptr->Get();
		}

		// clean the sub-device list for this device
		tg->get_sub_dev_diag().remove_sub_devices (device->get_name());
		tg->get_sub_dev_diag().set_associated_device(device->get_name());

		device->delete_device();
		device->init_device();

//
// Re-configure polling in device on which the Init cmd been done is the admin
// device but only if the Init is not called during the DS startup sequence
//

		DeviceImpl *admin_dev = NULL;
		try
		{
			admin_dev = tg->get_dserver_device();
		}
		catch (Tango::DevFailed &) {}

		if (admin_dev == device)
			tg->polling_configure();

		if (tg->is_py_ds())
			lock_ptr->Release();

//
// Apply memorized values for memorized attributes (if any)
// For Py DS, if some attributes are memorized, the write_attributes
// call will take the Python lock
//

		Tango::DeviceClass *dc = device->get_device_class();
		vector<Tango::DeviceImpl *> dev_v = dc->get_device_list();
		unsigned int loop;
		for (loop = 0;loop < dev_v.size();loop++)
		{
			if (dev_v[loop] == device)
				break;
		}
		if (loop != dev_v.size())
			dc->set_memorized_values(false,loop,true);
		else
		{
			Tango::Except::throw_exception((const char *)"API_DeviceNotFound",
										   (const char *)"Can't find new device in device list",
										   (const char *)"DevInitCmd::execute()");
		}

	}
	catch (Tango::DevFailed &e)
	{
		if ((tg->is_py_ds() == true) && (lock_ptr != NULL))
		{
			lock_ptr->Release();
		}

		TangoSys_OMemStream o;

		o << "Init command failed!!";
		o << "\nHINT: RESTART device with the Restart command of the device server adm. device";
		o << "\nDevice server adm. device name = dserver/";
		o << tg->get_ds_name().c_str() << ends;

		Except::re_throw_exception(e,(const char *)"API_InitThrowsException",o.str(),
				           (const char *)"DevInitCmd::execute()");
	}

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("InitCmd");
	return ret;
}


} // End of Tango namespace
