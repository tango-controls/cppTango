//+==================================================================================================================
//
// file :               Pipe.cpp
//
// description :        C++ source code for the Pipe class. The Pipe class is the root class for all derived
//						Pipe classes.
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
#include <eventsupplier.h>

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::Pipe
//
// description :
//		Constructor for abstract class Pipe
//
//-----------------------------------------------------------------------------------------------------------------

Pipe::Pipe(const std::string &_name,Tango::DispLevel _level,PipeWriteType _pwt)
:name(_name),disp_level(_level),writable(_pwt),ext(new PipeExt)
{
	lower_name = name;
	std::transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);

	pipe_serial_model = PIPE_BY_KERNEL;

	event_subscription = 0;

//
// Set lib default value for pipe label and desc
//

	label = name;
	desc = DescNotSpec;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_default_properties
//
// description :
//		This method set the default user properties in the Pipe object.
//
// arguments :
//		in :
//			- prop_list : The user property list
//
//-------------------------------------------------------------------------------------------------------------------

void Pipe::set_default_properties(UserDefaultPipeProp &prop_list)
{

//
// Init value in Pipe instance
//

	if ((prop_list.label.empty() == false) &&
		(TG_strcasecmp(prop_list.label.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.label.c_str(),NotANumber) != 0))
		label = prop_list.label;

	if (prop_list.description.empty() == false &&
		(TG_strcasecmp(prop_list.description.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.description.c_str(),NotANumber) != 0))
		desc = prop_list.description;

//
// Memorize user default (if any) for case of user requesting a "return to user default"
//

	user_def_prop.clear();
	if (prop_list.label.empty() == false)
	{
		PipeProperty pp("label",prop_list.label);
		user_def_prop.push_back(pp);
	}

	if (prop_list.description.empty() == false)
	{
		PipeProperty pp("description",prop_list.description);
		user_def_prop.push_back(pp);
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_upd_properties
//
// description :
//		Update pipe properties
//
// arguments :
//		in :
//			- new_conf : The pipe new configuration
//			- dev : The device
//
//--------------------------------------------------------------------------------------------------------------------

void Pipe::set_upd_properties(const PipeConfig &new_conf,DeviceImpl *dev)
{
//
// Backup current configuration (only label and description)
//

	PipeConfig old_conf = new_conf;
	old_conf.label = Tango::string_dup(label.c_str());
	old_conf.description = Tango::string_dup(desc.c_str());

	try
	{

//
// Set properties locally. In case of exception bring the backed-up values
//

		std::vector<Attribute::AttPropDb> v_db;
		set_properties(new_conf,dev,v_db);

//
// Update database
//

		try
		{
			upd_database(v_db,dev->get_name());
		}
		catch(DevFailed &)
		{

//
// In case of exception, try to store old properties in the database and inform the user about the error
//

			try
			{
				v_db.clear();
				set_properties(old_conf,dev,v_db);
				upd_database(v_db,dev->get_name());
			}
			catch(DevFailed &)
			{

//
// If the old values could not be restored, notify the user about possible database corruption
//

				TangoSys_OMemStream o;

				o << "Device " << dev->get_name() << "-> Pipe : " << name;
				o << "\nDatabase error occurred whilst setting pipe properties. The database may be corrupted." << std::ends;
				Except::throw_exception(API_CorruptedDatabase,o.str(),"Pipe::set_upd_properties()");
			}

			throw;
		}
	}
	catch(DevFailed &)
	{
		std::vector<Attribute::AttPropDb> v_db;
		set_properties(old_conf,dev,v_db);

		throw;
	}
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::upd_database
//
// description :
//		Update database according to the info received in the v_db parameter. We update or delete info in db for
//		pipe configuration.
//
// Arguments:
//		in :
//			- v_db : The vector of info with what has to be done in DB
//			- dev_name : The device name
//
//---------------------------------------------------------------------------------------------------------------------

void Pipe::upd_database(std::vector<Attribute::AttPropDb> &v_db,std::string &dev_name)
{

//
// Build info needed for the method upddating DB
//

	long prop_to_update = 0;
	long prop_to_delete = 0;

	Tango::DbData db_d;
	Tango::DbData db_del;

	db_d.push_back(DbDatum(name));
	db_del.push_back(DbDatum(name));

	std::vector<Attribute::AttPropDb>::iterator ite;

//
// A loop for each db action
//

	for (ite = v_db.begin();ite != v_db.end();++ite)
	{
		switch (ite->dba)
		{
			case Attribute::UPD:
			{
				DbDatum desc(ite->name);
				desc << ite->db_value;
				db_d.push_back(desc);
				prop_to_update++;
			}
			break;

			case Attribute::UPD_FROM_DB:
			{
				DbDatum desc(ite->name);
				desc << ite->db_value_db;
				db_d.push_back(desc);
				prop_to_update++;
			}
			break;

			case Attribute::UPD_FROM_VECT_STR:
			{
				DbDatum desc(ite->name);
				desc << ite->db_value_v_str;
				db_d.push_back(desc);
				prop_to_update++;
			}
			break;

			case Attribute::DEL:
			{
				DbDatum desc(ite->name);
				db_del.push_back(desc);
				prop_to_delete++;
			}
			break;
		}
	}

//
// Update database
//

	struct Attribute::CheckOneStrProp cosp;
	cosp.prop_to_delete = &prop_to_delete;
	cosp.prop_to_update = &prop_to_update;
	cosp.db_d = &db_d;
	cosp.db_del = &db_del;

//
// Update db only if needed
//

	if (*cosp.prop_to_update != 0)
	{
		cout4 << *cosp.prop_to_update << " properties to update in db" << std::endl;
		(*cosp.db_d)[0] << *cosp.prop_to_update;
//for (const auto &elem: *cosp.db_d)
//	cout << "prop_to_update name = " << elem.name << std::endl;

		Tango::Util *tg = Tango::Util::instance();

//
// Implement a reconnection schema. The first exception received if the db server is down is a COMM_FAILURE exception.
// Following exception received from following calls are TRANSIENT exception
//

		bool retry = true;
		while (retry == true)
		{
			try
			{
				tg->get_database()->put_device_pipe_property(dev_name,*cosp.db_d);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE &)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}

	if (*cosp.prop_to_delete != 0)
	{
		cout4 << *cosp.prop_to_delete << " properties to delete in db" << std::endl;
		(*cosp.db_del)[0] << *cosp.prop_to_delete;
//for (const auto &elem: *cosp.db_del)
//	cout << "prop_to_delete name = " << elem.name << std::endl;

		Tango::Util *tg = Tango::Util::instance();

//
// Implement a reconnection schema. The first exception received if the db server is down is a COMM_FAILURE exception.
// Following exception received from following calls are TRANSIENT exception
//

		bool retry = true;
		while (retry == true)
		{
			try
			{
				tg->get_database()->delete_device_pipe_property(dev_name,*cosp.db_del);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE &)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_properties
//
// description :
//		Set the pipe properties value for PipeConfig
//
// argument :
// 		in :
//			- conf : The new properties sent by client
//			- dev : The device
//		out :
//			- v_db : Vector of data used for database update/delete
//
//--------------------------------------------------------------------------------------------------------------------

void Pipe::set_properties(const Tango::PipeConfig &conf,DeviceImpl *dev,std::vector<Attribute::AttPropDb> &v_db)
{

//
// Check if the caller try to change "hard coded"properties. Throw exception in case of
//

	std::string user_pipe_name(conf.name.in());
	std::transform(user_pipe_name.begin(),user_pipe_name.end(),user_pipe_name.begin(),::tolower);
	if (user_pipe_name != lower_name)
	{
		Except::throw_exception(API_AttrNotAllowed,"Pipe name is not changeable at run time","Pipe::set_properties()");
	}

	if (conf.writable != writable)
	{
		Except::throw_exception(API_AttrNotAllowed,"Pipe writable property is not changeable at run time","Pipe::set_properties()");
	}

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string means returns to its default value which could be the library default
// value or the user defined default value
//

	Tango::DeviceClass *dev_class = dev->get_device_class();

	std::vector<PipeProperty> &def_user_prop = get_user_default_properties();
	std::vector<PipeProperty> def_class_prop;
	try
	{
		def_class_prop = dev_class->get_class_pipe()->get_prop_list(name);
	}
	catch (DevFailed &e) {}

//
// First the string properties
//

	set_one_str_prop("description",conf.description,desc,v_db,def_user_prop,def_class_prop,DescNotSpec);
	set_one_str_prop("label",conf.label,label,v_db,def_user_prop,def_class_prop,name.c_str());

}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_one_str_prop
//
// description :
//		Analyse one of the string properties. String properties are description and  label
//		A similar method exist in Attribute class. It's difficult to make it static and use it in this Pipe class
//
// argument :
// 		in :
//			- prop_name : The property name
//			- conf_val : The new property value
//			- def_user_prop : The set of user defined default values
//			- def_class_prop : The set of class defined default values
//			- lib_def : The property library default value
//		out :
//			- pipe_conf : The new property in Pipe object
//			- v_db : Vector of data used for database update/delete
//
//--------------------------------------------------------------------------------------------------------------------

void Pipe::set_one_str_prop(const char *prop_name,const CORBA::String_member &conf_val,
								 std::string &pipe_conf,std::vector<Attribute::AttPropDb> &v_db,std::vector<PipeProperty> &def_user_prop,
								std::vector<PipeProperty> &def_class_prop,const char *lib_def)
{
	Attribute::AttPropDb apd;
	apd.name = prop_name;

	bool user_defaults, class_defaults;
	std::string usr_def_val, class_def_val;
	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

    user_defaults = prop_in_list(prop_name,usr_def_val,nb_user,def_user_prop);
    class_defaults = prop_in_list(prop_name,class_def_val,nb_class,def_class_prop);

	if (TG_strcasecmp(conf_val,AlrmValueNotSpec) == 0)
	{

//
// Return to lib default. If something defined as user default or class default, put entry in DB to overwrite
// these defaults
//

		std::string old_val = pipe_conf;
		pipe_conf = lib_def;

		if (old_val != pipe_conf)
		{
			if (user_defaults == true || class_defaults == true)
			{
				apd.dba = Attribute::UPD;
				apd.db_value = pipe_conf;
				v_db.push_back(apd);
			}
			else
			{
				apd.dba = Attribute::DEL;
				v_db.push_back(apd);
			}
		}
	}
	else if (strlen(conf_val) == 0)
	{

//
// Return to user default or lib default. If something defined as class default, put entry in DB in order to
// overwrite this default value.
//

		std::string old_val = pipe_conf;

        if (user_defaults == true)
			pipe_conf = usr_def_val;
		else
		{
			pipe_conf = lib_def;
		}

		if (old_val != pipe_conf)
		{
			if (class_defaults == true)
			{
				apd.dba = Attribute::UPD;
				apd.db_value = pipe_conf;
				v_db.push_back(apd);
			}
			else
			{
				apd.dba = Attribute::DEL;
				v_db.push_back(apd);
			}
		}
	}
	else if (TG_strcasecmp(conf_val,NotANumber) == 0)
	{

//
// Return to class default or user default or lib default
//

		std::string old_val = pipe_conf;

		if (class_defaults == true)
			pipe_conf = class_def_val;
		else if (user_defaults == true)
			pipe_conf = usr_def_val;
		else
		{
			pipe_conf = lib_def;
		}

		if (old_val != pipe_conf)
		{
			apd.dba = Attribute::DEL;
			v_db.push_back(apd);
		}
	}
	else
	{

//
// Set property
//

		std::string old_val = pipe_conf;
		pipe_conf = conf_val;

		if (user_defaults == true && pipe_conf == usr_def_val)
		{

//
// Property value is the same than the user default value
//

			if (old_val != pipe_conf)
			{
				if (class_defaults == true)
				{
					apd.dba = Attribute::UPD;
					apd.db_value = pipe_conf;
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = Attribute::DEL;
					v_db.push_back(apd);
				}
			}
		}
		else if (class_defaults == true && pipe_conf == class_def_val)
		{

//
// Property value is the same than the class default value
//

			if (old_val != pipe_conf)
			{
				apd.dba = Attribute::DEL;
				v_db.push_back(apd);
			}
		}
   	 	else if (class_defaults == false && TG_strcasecmp(pipe_conf.c_str(),lib_def) == 0)
		{

//
// Property value is the same than the lib default value
//

			if (old_val != pipe_conf)
			{
				apd.dba = Attribute::DEL;
				v_db.push_back(apd);
			}
		}
		else if (class_defaults == false && strcmp(prop_name,"label") == 0)
		{

//
// Property Label: Property value is the same than the lib default value
//

			if (old_val != pipe_conf)
			{
				if (TG_strcasecmp(pipe_conf.c_str(),LabelNotSpec) == 0)
				{
					apd.dba = Attribute::DEL;
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = Attribute::UPD;
					apd.db_value = pipe_conf;
					v_db.push_back(apd);
				}
			}
		}
		else
		{
			if (old_val != pipe_conf)
			{
				apd.dba = Attribute::UPD;
				apd.db_value = pipe_conf;
				v_db.push_back(apd);
			}
		}
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_time
//
// description :
//		Set the date in the pipe object
//
//--------------------------------------------------------------------------------------------------------------------

void Pipe::set_time()
{
#ifdef _TG_WINDOWS_
	struct _timeb t;
	_ftime(&t);

	when.tv_sec = (CORBA::Long)t.time;
	when.tv_usec = (CORBA::Long)(t.millitm * 1000);
	when.tv_nsec = 0;
#else
	struct timezone tz;
	struct timeval tv;
	gettimeofday(&tv,&tz);

	when.tv_sec = (CORBA::Long)tv.tv_sec;
	when.tv_usec = (CORBA::Long)tv.tv_usec;
	when.tv_nsec = 0;
#endif
}

//+-------------------------------------------------------------------------
//
// method : 		Pipe::set_pipe_serial_method
//
// description : 	Set pipe serialization method
//
//--------------------------------------------------------------------------

void Pipe::set_pipe_serial_model(PipeSerialModel ser_model)
{
	if (ser_model == Tango::PIPE_BY_USER)
	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->get_serial_model() != Tango::BY_DEVICE)
		{
			Except::throw_exception(API_PipeNotAllowed,
				      	  "Pipe serial model by user is not allowed when the process is not in BY_DEVICE serialization model",
				      	  "Pipe::set_pipe_serial_model");
		}
	}

	pipe_serial_model=ser_model;
}

Pipe &Pipe::operator[](const std::string &_na)
{
	the_blob.operator[](_na);
	return *this;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::fire_event
//
// description :
//		Fire a pipe event
//
// arguments:
// 		in :
//			- dev : Device pointer
//			- except : Pointer to the DevFailed exception
//
//---------------------------------------------------------------------------------------------------------------------


void Pipe::fire_event(DeviceImpl *dev,DevFailed *except)
{
	cout4 << "Pipe::fire_event() entering ..." << std::endl;

//
// Check if it is needed to send an event
//

	if (! is_pipe_event_subscribed())
	{
		return;
	}

//
// Get the event supplier, and simply return if not created
//

	ZmqEventSupplier *event_supplier_zmq = NULL;

	Tango::Util *tg = Util::instance();
	event_supplier_zmq = tg->get_zmq_event_supplier();

	if (event_supplier_zmq == NULL)
	{
		return;
	}

//
// Make sure the severity field is initialized (this field is never used!!)
//

	size_t err_nb = except->errors.length();
	for (size_t loop = 0;loop < err_nb;loop++)
		(except->errors)[loop].severity = Tango::ERR;

//
// Create the structure used to send data to event system
//

	EventSupplier::SuppliedEventData ad;
	::memset(&ad,0,sizeof(ad));

//
// Fire event
//

	std::vector<std::string> f_names;
	std::vector<double> f_data;
	std::vector<std::string> f_names_lg;
	std::vector<long> f_data_lg;

	std::string event_type("pipe");
	event_supplier_zmq->push_event(dev, event_type, f_names, f_data, f_names_lg, f_data_lg, ad, name, except, true);
}

#ifdef _TG_WINDOWS_
void Pipe::fire_event(DeviceImpl *_dev,DevicePipeBlob *_dat,bool reuse_it)
{
	struct _timeb now_win;
	struct timeval now;

	_ftime(&now_win);
	now.tv_sec = (unsigned long)now_win.time;
	now.tv_usec = (long)now_win.millitm * 1000;

	fire_event(_dev,_dat,now,reuse_it);
}
#endif // _TG_WINDOWS_


void Pipe::fire_event(DeviceImpl *dev,DevicePipeBlob *p_data,struct timeval &t,bool reuse_it)
{
	cout4 << "Pipe::fire_event() entering ..." << std::endl;

//
// Check if it is needed to send an event
//

	if (! is_pipe_event_subscribed())
	{
		p_data->reset_insert_ctr();
		DevVarPipeDataEltArray *tmp_ptr = p_data->get_insert_data();
		delete tmp_ptr;
		return;
	}

//
// Get the event supplier, and simply return if not created
//

	ZmqEventSupplier *event_supplier_zmq = NULL;

	Tango::Util *tg = Util::instance();
	event_supplier_zmq = tg->get_zmq_event_supplier();

	if (event_supplier_zmq == NULL)
	{
		p_data->reset_insert_ctr();
		DevVarPipeDataEltArray *tmp_ptr = p_data->get_insert_data();
		delete tmp_ptr;
		return;
	}

//
// Create the structure used to send data to event system
//

	EventSupplier::SuppliedEventData ad;
	::memset(&ad,0,sizeof(ad));

//
// Init the structure sent with the event
//

	ad.pipe_val = new DevPipeData();
	ad.pipe_val->name = Tango::string_dup(name.c_str());

	::memset(&(ad.pipe_val->time),0,sizeof(ad.pipe_val->time));
	ad.pipe_val->time.tv_sec = t.tv_sec;
	ad.pipe_val->time.tv_usec = t.tv_usec;


	const std::string &bl_name = p_data->get_name();
	if (bl_name.size() != 0)
		ad.pipe_val->data_blob.name = bl_name.c_str();

	DevVarPipeDataEltArray *tmp_ptr = p_data->get_insert_data();
	if (tmp_ptr == nullptr)
	{
		Except::throw_exception(API_PipeNoDataElement,"No data in DevicePipeBlob!","Pipe::fire_event()");
	}

	CORBA::ULong max,len;
	max = tmp_ptr->maximum();
	len = tmp_ptr->length();
	ad.pipe_val->data_blob.blob_data.replace(max,len,tmp_ptr->get_buffer((CORBA::Boolean)true),true);

//
// Fire event
//

	std::vector<std::string> f_names;
	std::vector<double> f_data;
	std::vector<std::string> f_names_lg;
	std::vector<long> f_data_lg;

	std::string event_type("pipe");
	event_supplier_zmq->push_event(dev, event_type, f_names, f_data, f_names_lg, f_data_lg, ad, name, NULL, true);

	if (reuse_it == false)
	{
		p_data->reset_insert_ctr();
		delete tmp_ptr;
	}

	delete ad.pipe_val;
}

bool Pipe::is_pipe_event_subscribed() const
{
    const auto now = time(NULL);
    const auto delta_subscription = now - event_subscription;

    return delta_subscription < EVENT_RESUBSCRIBE_PERIOD;
}

} // End of Tango namespace
