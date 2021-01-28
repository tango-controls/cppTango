//+=============================================================================
//
// file :               dserverlock.cpp
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


namespace Tango
{

//+----------------------------------------------------------------------------
//
// method : 		DServer::lock_device()
//
// description : 	command to lock device
//
// args: - in_data : Pointer to a structure with:
//						1 - Name of the device(s) to be locked
//		 				2 - Lock validity
//
//-----------------------------------------------------------------------------

void DServer::lock_device(const Tango::DevVarLongStringArray *in_data)
{
	NoSyncModelTangoMonitor mon(this);

	std::string dev_name(in_data->svalue[0]);
	int lock_validity = in_data->lvalue[0];

	cout4 << "In lock_device command for device " << dev_name << ", lock validity = " << lock_validity << std::endl;

//
// Get client identification
//

	Tango::client_addr *cl = get_client_ident();
	if (cl == NULL)
	{
		TANGO_THROW_EXCEPTION(API_CantGetClientIdent, "Cannot retrieve client identification");
	}

	cout4 << "Client identification = " << *cl << std::endl;

	if (cl->client_ident == false)
	{
		TANGO_THROW_EXCEPTION(API_ClientTooOld, "Your client cannot lock devices. You are using a too old Tango release. Please, update to tango V7 or more");
	}

//
// Transform device name to lower case
//

	Tango::Util *tg = Tango::Util::instance();

	std::string local_dev_name(get_name());
	transform(local_dev_name.begin(),local_dev_name.end(),local_dev_name.begin(),::tolower);

	std::string d_name_lower(dev_name);
	transform(d_name_lower.begin(),d_name_lower.end(),d_name_lower.begin(),::tolower);

//
// Refuse to lock the admin device
//

	if (d_name_lower == local_dev_name)
	{
		TANGO_THROW_EXCEPTION(API_DeviceUnlockable, "Impossible to lock device server administration device");
	}

//
// Get device ptr
//

	DeviceImpl *the_dev;
	the_dev = tg->get_device_by_name(dev_name);

//
// Refuse to lock database device
//

	std::string &cl_name = the_dev->get_device_class()->get_name();
	if (::strcmp(cl_name.c_str(),DATABASE_CLASS) == 0)
	{
		TANGO_THROW_EXCEPTION(API_DeviceUnlockable, "Impossible to lock database device");
	}

//
// Mark the device as locked
//

	the_dev->lock(cl,lock_validity);

}


//+----------------------------------------------------------------------------
//
// method : 		DServer::un_lock_device()
//
// description : 	command to un lock a device
//
// args: - dev_name : The device name
//
//-----------------------------------------------------------------------------

Tango::DevLong DServer::un_lock_device(const Tango::DevVarLongStringArray *in_data)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In un_lock_device command for device " << in_data->svalue[0] << std::endl;

//
// Get client identification
//

	Tango::client_addr *cl = get_client_ident();
	if (cl == NULL)
	{
		TANGO_THROW_EXCEPTION(API_CantGetClientIdent, "Cannot retrieve client identification");
	}

	cout4 << "Client identification = " << *cl << std::endl;

	if ((cl->client_ident == false) && (in_data->lvalue[0] == 0))
	{
		TANGO_THROW_EXCEPTION(API_ClientTooOld, "Your client cannot un-lock devices. You are using a too old Tango release. Please, update to tango V7 or more");
	}

//
// Get the device and unlock it
//

	DevLong ctr = 0;
	Tango::Util *tg = Tango::Util::instance();

	for (unsigned int loop = 0;loop < in_data->svalue.length();++loop)
	{
		std::string d_name(in_data->svalue[loop]);

		if (d_name == get_name())
			ctr = lock_ctr;
		else
		{
			DeviceImpl *the_dev = tg->get_device_by_name(d_name);
			ctr = the_dev->unlock((bool)in_data->lvalue[0]);
		}

		if (loop > 0)
			ctr = 0;
	}

	return ctr;
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::re_lock_devices()
//
// description : 	command to re-lock devices
//
// args: - dev_name_list : Name of the device(s) to be re-locked
//
//-----------------------------------------------------------------------------

void DServer::re_lock_devices(const Tango::DevVarStringArray *dev_name_list)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In re_lock_devices command" << std::endl;
	CORBA::ULong loop;
	CORBA::ULong nb_dev = dev_name_list->length();

	for (loop = 0;loop < nb_dev;loop++)
		cout4 << "Device to re-lock: " << (*dev_name_list)[loop] << std::endl;

//
// Get client identification
//

	Tango::client_addr *cl = get_client_ident();
	if (cl == NULL)
	{
		TANGO_THROW_EXCEPTION(API_CantGetClientIdent, "Cannot retrieve client identification");
	}

	cout4 << "Client identification = " << *cl << std::endl;

	if (cl->client_ident == false)
	{
		TANGO_THROW_EXCEPTION(API_ClientTooOld, "Your client cannot re_lock devices. You are using a too old Tango release. Please, update to tango V7 or more");
	}

//
// ReLock the devices
// If we have an error in this loop, memorize it and throw the exception at the
// end of the loop
//

	Tango::Util *tg = Tango::Util::instance();

	DevErrorList errors;
	long nb_error = 0;

	for (loop = 0;loop < nb_dev;loop++)
	{
		std::string d_name((*dev_name_list)[loop]);

//
// Get device ptr
//

		DeviceImpl *the_dev = NULL;
		try
		{
			the_dev = tg->get_device_by_name(d_name);
		}
		catch (Tango::DevFailed &e)
		{
			errors.length(nb_error + 1);
			errors[nb_error].desc = Tango::string_dup(e.errors[0].desc.in());
			errors[nb_error].reason = Tango::string_dup(e.errors[0].reason.in());
			errors[nb_error].origin = Tango::string_dup(e.errors[0].origin.in());
			errors[nb_error].severity = e.errors[0].severity;
			nb_error++;
		}

//
// ReLock the device
//

		try
		{
			the_dev->relock(cl);
		}
		catch (Tango::DevFailed &e)
		{
			errors.length(nb_error + 1);
			errors[nb_error].desc = Tango::string_dup(e.errors[0].desc.in());
			errors[nb_error].reason = Tango::string_dup(e.errors[0].reason.in());
			errors[nb_error].origin = Tango::string_dup(e.errors[0].origin.in());
			errors[nb_error].severity = e.errors[0].severity;
			nb_error++;
		}
	}

//
// Throw the exception if we had one during the loop
//

	if (nb_error != 0)
	{
		throw Tango::DevFailed(errors);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::dev_lock_status()
//
// description : 	command to get device lock status
//
// args: - dev_name : The device name
//
//-----------------------------------------------------------------------------

Tango::DevVarLongStringArray *DServer::dev_lock_status(Tango::ConstDevString dev_name)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In dev_lock_status command for device " << dev_name << std::endl;

//
// Get the device and get its lock status
//

	std::string d_name(dev_name);

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *the_dev = tg->get_device_by_name(d_name);
	return the_dev->lock_status();
}


} // End of Tango namespace

