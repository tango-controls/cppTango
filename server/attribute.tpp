//+============================================================================
//
// file :               Attribute.tpp
//
// description :        C++ source code for the Attribute class template
//                      methods
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012
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
// $Revision: 17240 $
//
//-============================================================================

#ifndef _ATTRIBUTE_TPP
#define _ATTRIBUTE_TPP

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Attribute::check_hard_coded_properties()
//
// description : 	Check if the user tries to change attribute
//                  properties considered as hard coded
//                  Throw exception in case of
//
// in :	user_conf : The attribute configuration sent by the user
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::check_hard_coded_properties(const T &user_conf)
{
//
// Check attribute name
//

    string user_att_name(user_conf.name.in());
    transform(user_att_name.begin(),user_att_name.end(),user_att_name.begin(),::tolower);
    if (user_att_name != get_name_lower())
    {
        throw_hard_coded_prop("name");
    }

//
// Check data type
//

    if (user_conf.data_type != data_type)
    {
        throw_hard_coded_prop("data_type");
    }

//
// Check data format
//

    if (user_conf.data_format != data_format)
    {
        throw_hard_coded_prop("data_format");
    }

//
// Check writable
//

    if (user_conf.writable != writable)
    {
        throw_hard_coded_prop("writable");
    }

//
// Check max_dim_x
//

    if (user_conf.max_dim_x != max_x)
    {
        throw_hard_coded_prop("max_dim_x");
    }

//
// Check max_dim_y
//

    if (user_conf.max_dim_y != max_y)
    {
        throw_hard_coded_prop("max_dim_y");
    }

//
// Check writable_attr_name
//

    string local_w_name(writable_attr_name);
    transform(local_w_name.begin(),local_w_name.end(),local_w_name.begin(),::tolower);
    string user_w_name(user_conf.writable_attr_name.in());
    transform(user_w_name.begin(),user_w_name.end(),user_w_name.begin(),::tolower);

    if (user_w_name != local_w_name)
    {
        throw_hard_coded_prop("writable_attr_name");
    }
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_min_alarm()
//
// description : 	Sets minimum alarm attribute property
//					Throws exception in case the data type of provided
//					property does not match the attribute data type
//
// in :	new_min_alarm : The minimum alarm property to be set
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::set_min_alarm(const T &new_min_alarm)
{
//
// Check type validity
//

	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::set_min_alarm()");
	}

//
//	Check coherence with max_alarm
//

	if(alarm_conf.test(max_level))
	{
		T max_alarm_tmp;
		memcpy((void *) &max_alarm_tmp, (const void *) &max_alarm, sizeof(T));
		if(new_min_alarm >= max_alarm_tmp)
			throw_incoherent_val_err("min_alarm","max_alarm",ext->d_name,"Attribute::set_min_alarm");
	}

//
// Store new min alarm as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_min_alarm; // to represent the numeric value
	else
		str << new_min_alarm;
	string min_alarm_tmp_str;
	min_alarm_tmp_str = str.str();

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

    Tango::Util *tg = Tango::Util::instance();
    Tango::TangoMonitor *mon_ptr = NULL;
    if (tg->is_svr_starting() == false)
        mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Store the new alarm locally
//

	Attr_CheckVal old_min_alarm;
	memcpy((void *)&old_min_alarm, (void *)&min_alarm, sizeof(T));
	memcpy((void *)&min_alarm, (void *)&new_min_alarm, sizeof(T));

//
// Then, update database
//

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "min_alarm")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}


	if (Tango::Util::_UseDb == true)
	{
		if(user_defaults && min_alarm_tmp_str == usr_def_val)
		{
			DbDatum attr_dd(name), prop_dd("min_alarm");
			DbData db_data;
			db_data.push_back(attr_dd);
			db_data.push_back(prop_dd);

			bool retry = true;
			while (retry == true)
			{
				try
				{
					tg->get_database()->delete_device_attribute_property(ext->d_name,db_data);
					retry = false;
				}
				catch (CORBA::COMM_FAILURE)
				{
					tg->get_database()->reconnect(true);
				}
			}
		}
		else
		{
			try
			{
				upd_att_prop_db(min_alarm,"min_alarm");
			}
			catch (Tango::DevFailed &)
			{
				memcpy((void *)&min_alarm, (void *)&old_min_alarm, sizeof(T));
				throw;
			}
		}
	}

//
// Set the min_warn flag
//

	alarm_conf.set(min_level);

//
// Store new alarm as a string
//

	min_alarm_str = min_alarm_tmp_str;

//
// Push a att conf event
//

    if (tg->is_svr_starting() == false)
        get_att_device()->push_att_conf_event(this);
}

template <>
inline void Attribute::set_min_alarm(const string &new_min_alarm_str)
{
	string min_alarm_str_tmp = new_min_alarm_str;
	string dev_name = ext->d_name;

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "min_alarm")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	bool set_value = true;
	if(user_defaults)
	{
		if(TG_strcasecmp(new_min_alarm_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("min_alarm");
				attr_dd << 1L;
				prop_dd << AlrmValueNotSpec;
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->put_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(min_level);
			min_alarm_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
		else if ((TG_strcasecmp(new_min_alarm_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_min_alarm_str.c_str(),usr_def_val.c_str()) == 0) ||
				(strlen(new_min_alarm_str.c_str()) == 0))
			min_alarm_str_tmp = usr_def_val;
	}
	else
	{
		if ((TG_strcasecmp(new_min_alarm_str.c_str(),AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(new_min_alarm_str.c_str(),NotANumber) == 0) ||
				(strlen(new_min_alarm_str.c_str()) == 0))
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("min_alarm");
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->delete_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(min_level);
			min_alarm_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
	}

	if(set_value)
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			double db;
			float fl;

			TangoSys_MemStream str;
			str << min_alarm_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				set_min_alarm((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				set_min_alarm((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				set_min_alarm((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				set_min_alarm(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				set_min_alarm(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				(db < 0.0) ? set_min_alarm((DevUShort)(-db)) : set_min_alarm((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				(db < 0.0) ? set_min_alarm((DevUChar)(-db)) : set_min_alarm((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				(db < 0.0) ? set_min_alarm((DevULong)(-db)) : set_min_alarm((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				(db < 0.0) ? set_min_alarm((DevULong64)(-db)) : set_min_alarm((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm");
				(db < 0.0) ? set_min_alarm((DevUChar)(-db)) : set_min_alarm((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("min_alarm",dev_name,"Attribute::set_min_alarm");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_min_alarm()
//
// description :	Gets attribute's minimum alarm value and assigns it
//					to the variable provided as a parameter
//					Throws exception in case the data type of provided
//					parameter does not match the attribute data type
//					or if minimum alarm is not defined
//
// in :	min_al : The variable to be assigned the attribute's
//					minimum alarm value
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::get_min_alarm(T &min_al)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_min_alarm()");
	}

	if (!alarm_conf[min_level])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Minimum alarm not defined for this attribute",
					(const char *)"Attribute::get_min_alarm()");
	}

	memcpy((void *)&min_al,(void *)&min_alarm,sizeof(T));
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_max_alarm()
//
// description : 	Sets maximum alarm attribute property
//					Throws exception in case the data type of provided
//					property does not match the attribute data type
//
// in :	new_max_alarm : The maximum alarm property to be set
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::set_max_alarm(const T &new_max_alarm)
{

//
// Check type validity
//

	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::set_max_alarm()");
	}

//
//	Check coherence with min_alarm
//

	if(alarm_conf.test(min_level))
	{
		T min_alarm_tmp;
		memcpy((void *) &min_alarm_tmp, (const void *) &min_alarm, sizeof(T));
		if(new_max_alarm <= min_alarm_tmp)
			throw_incoherent_val_err("min_alarm","max_alarm",ext->d_name,"Attribute::set_max_alarm");
	}

//
// Store new max alarm as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_max_alarm; // to represent the numeric value
	else
		str << new_max_alarm;
	string max_alarm_tmp_str;
	max_alarm_tmp_str = str.str();

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::TangoMonitor *mon_ptr = NULL;
	if (tg->is_svr_starting() == false)
		mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Store the new alarm locally
//

	Attr_CheckVal old_max_alarm;
	memcpy((void *)&old_max_alarm, (void *)&max_alarm, sizeof(T));
	memcpy((void *)&max_alarm, (void *)&new_max_alarm, sizeof(T));

//
// Then, update database
//

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "max_alarm")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}


	if (Tango::Util::_UseDb == true)
	{
		if(user_defaults && max_alarm_tmp_str == usr_def_val)
		{
			DbDatum attr_dd(name), prop_dd("max_alarm");
			DbData db_data;
			db_data.push_back(attr_dd);
			db_data.push_back(prop_dd);

			bool retry = true;
			while (retry == true)
			{
				try
				{
					tg->get_database()->delete_device_attribute_property(ext->d_name,db_data);
					retry = false;
				}
				catch (CORBA::COMM_FAILURE)
				{
					tg->get_database()->reconnect(true);
				}
			}
		}
		else
		{
			try
			{
				upd_att_prop_db(max_alarm,"max_alarm");
			}
			catch (Tango::DevFailed &)
			{
				memcpy((void *)&max_alarm, (void *)&old_max_alarm, sizeof(T));
				throw;
			}
		}
	}

//
// Set the max_warn flag
//

	alarm_conf.set(max_level);

//
// Store new alarm as a string
//

	max_alarm_str = max_alarm_tmp_str;

//
// Push a att conf event
//

    if (tg->is_svr_starting() == false)
        get_att_device()->push_att_conf_event(this);
}

template <>
inline void Attribute::set_max_alarm(const string &new_max_alarm_str)
{
	string max_alarm_str_tmp = new_max_alarm_str;
	string dev_name = ext->d_name;

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "max_alarm")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	bool set_value = true;
	if(user_defaults)
	{
		if(TG_strcasecmp(new_max_alarm_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("max_alarm");
				attr_dd << 1L;
				prop_dd << AlrmValueNotSpec;
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				Tango::Util *tg = Tango::Util::instance();
				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->put_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(max_level);
			max_alarm_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
		else if ((TG_strcasecmp(new_max_alarm_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_max_alarm_str.c_str(),usr_def_val.c_str()) == 0) ||
				(strlen(new_max_alarm_str.c_str()) == 0))
			max_alarm_str_tmp = usr_def_val;
	}
	else
	{
		if ((TG_strcasecmp(new_max_alarm_str.c_str(),AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(new_max_alarm_str.c_str(),NotANumber) == 0) ||
				(strlen(new_max_alarm_str.c_str()) == 0))
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("max_alarm");
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->delete_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(max_level);
			max_alarm_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
	}

	if(set_value)
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			double db;
			float fl;

			TangoSys_MemStream str;
			str << max_alarm_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				set_max_alarm((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				set_max_alarm((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				set_max_alarm((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				set_max_alarm(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				set_max_alarm(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				(db < 0.0) ? set_max_alarm((DevUShort)(-db)) : set_max_alarm((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				(db < 0.0) ? set_max_alarm((DevUChar)(-db)) : set_max_alarm((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				(db < 0.0) ? set_max_alarm((DevULong)(-db)) : set_max_alarm((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				(db < 0.0) ? set_max_alarm((DevULong64)(-db)) : set_max_alarm((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm");
				(db < 0.0) ? set_max_alarm((DevUChar)(-db)) : set_max_alarm((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("max_alarm",dev_name,"Attribute::set_max_alarm");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_max_alarm()
//
// description :	Gets attribute's maximum alarm value and assigns it
//					to the variable provided as a parameter
//					Throws exception in case the data type of provided
//					parameter does not match the attribute data type
//					or if maximum alarm is not defined
//
// in :	max_al : The variable to be assigned the attribute's
//					maximum alarm value
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::get_max_alarm(T &max_al)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_max_alarm()");
	}

	if (!alarm_conf[max_level])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Maximum alarm not defined for this attribute",
					(const char *)"Attribute::get_max_alarm()");
	}

	memcpy((void *)&max_al,(void *)&max_alarm,sizeof(T));
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_min_warning()
//
// description : 	Sets minimum warning attribute property
//					Throws exception in case the data type of provided
//					property does not match the attribute data type
//
// in :	new_min_warning : The minimum warning property to be set
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::set_min_warning(const T &new_min_warning)
{

//
// Check type validity
//

	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
				      (const char *)err_msg.c_str(),
				      (const char *)"Attribute::set_min_warning()");
	}

//
//	Check coherence with max_warning
//

	if(alarm_conf.test(max_warn))
	{
		T max_warning_tmp;
		memcpy((void *) &max_warning_tmp, (const void *) &max_warning, sizeof(T));
		if(new_min_warning >= max_warning_tmp)
			throw_incoherent_val_err("min_warning","max_warning",ext->d_name,"Attribute::set_min_warning");
	}

//
// Store new min warning as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_min_warning; // to represent the numeric value
	else
		str << new_min_warning;
	string min_warning_tmp_str;
	min_warning_tmp_str = str.str();

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::TangoMonitor *mon_ptr = NULL;
	if (tg->is_svr_starting() == false)
		mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Store the new warning locally
//

	Attr_CheckVal old_min_warning;
	memcpy((void *)&old_min_warning, (void *)&min_warning, sizeof(T));
	memcpy((void *)&min_warning, (void *)&new_min_warning, sizeof(T));

//
// Then, update database
//

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "min_warning")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}


	if (Tango::Util::_UseDb == true)
	{
		if(user_defaults && min_warning_tmp_str == usr_def_val)
		{
			DbDatum attr_dd(name), prop_dd("min_warning");
			DbData db_data;
			db_data.push_back(attr_dd);
			db_data.push_back(prop_dd);

			bool retry = true;
			while (retry == true)
			{
				try
				{
					tg->get_database()->delete_device_attribute_property(ext->d_name,db_data);
					retry = false;
				}
				catch (CORBA::COMM_FAILURE)
				{
					tg->get_database()->reconnect(true);
				}
			}
		}
		else
		{
			try
			{
				upd_att_prop_db(min_warning,"min_warning");
			}
			catch (Tango::DevFailed &)
			{
				memcpy((void *)&min_warning, (void *)&old_min_warning, sizeof(T));
				throw;
			}
		}
	}

//
// Set the min_warn flag
//

	alarm_conf.set(min_warn);

//
// Store new warning as a string
//

	min_warning_str = min_warning_tmp_str;

//
// Push a att conf event
//

	if (tg->is_svr_starting() == false)
		get_att_device()->push_att_conf_event(this);
}

template <>
inline void Attribute::set_min_warning(const string &new_min_warning_str)
{
	string min_warning_str_tmp = new_min_warning_str;
	string dev_name = ext->d_name;

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "min_warning")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	bool set_value = true;
	if(user_defaults)
	{
		if(TG_strcasecmp(new_min_warning_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("min_warning");
				attr_dd << 1L;
				prop_dd << AlrmValueNotSpec;
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->put_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(min_warn);
			min_warning_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
		else if ((TG_strcasecmp(new_min_warning_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_min_warning_str.c_str(),usr_def_val.c_str()) == 0) ||
				(strlen(new_min_warning_str.c_str()) == 0))
			min_warning_str_tmp = usr_def_val;
	}
	else
	{
		if ((TG_strcasecmp(new_min_warning_str.c_str(),AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(new_min_warning_str.c_str(),NotANumber) == 0) ||
				(strlen(new_min_warning_str.c_str()) == 0))
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("min_warning");
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->delete_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(min_warn);
			min_warning_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
	}

	if(set_value)
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			double db;
			float fl;

			TangoSys_MemStream str;
			str << min_warning_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				set_min_warning((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				set_min_warning((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				set_min_warning((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				set_min_warning(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				set_min_warning(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				(db < 0.0) ? set_min_warning((DevUShort)(-db)) : set_min_warning((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				(db < 0.0) ? set_min_warning((DevUChar)(-db)) : set_min_warning((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				(db < 0.0) ? set_min_warning((DevULong)(-db)) : set_min_warning((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				(db < 0.0) ? set_min_warning((DevULong64)(-db)) : set_min_warning((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning");
				(db < 0.0) ? set_min_warning((DevUChar)(-db)) : set_min_warning((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("min_warning",dev_name,"Attribute::set_min_warning");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_min_warning()
//
// description :	Gets attribute's minimum warning value and assigns it
//					to the variable provided as a parameter
//					Throws exception in case the data type of provided
//					parameter does not match the attribute data type
//					or if minimum warning is not defined
//
// in :	min_war : The variable to be assigned the attribute's
//					minimum warning value
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::get_min_warning(T &min_war)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_min_warning()");
	}

	if (!alarm_conf[min_warn])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Minimum warning not defined for this attribute",
					(const char *)"Attribute::get_min_warning()");
	}

	memcpy((void *)&min_war,(void *)&min_warning,sizeof(T));
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_max_warning()
//
// description : 	Sets maximum warning attribute property
//					Throws exception in case the data type of provided
//					property does not match the attribute data type
//
// in :	new_maximum_warning : The maximum warning property to be set
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::set_max_warning(const T &new_max_warning)
{

//
// Check type validity
//

	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::set_max_warning()");
	}

//
//	Check coherence with min_warning
//

	if(alarm_conf.test(min_warn))
	{
		T min_warning_tmp;
		memcpy((void *) &min_warning_tmp, (const void *) &min_warning, sizeof(T));
		if(new_max_warning <= min_warning_tmp)
			throw_incoherent_val_err("min_warning","max_warning",ext->d_name,"Attribute::set_max_warning");
	}

//
// Store new max warning as a string
//

	TangoSys_MemStream str;
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_max_warning; // to represent the numeric value
	else
		str << new_max_warning;
	string max_warning_tmp_str;
	max_warning_tmp_str = str.str();

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::TangoMonitor *mon_ptr = NULL;
	if (tg->is_svr_starting() == false)
		mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Store the new warning locally
//

	Attr_CheckVal old_max_warning;
	memcpy((void *)&old_max_warning, (void *)&max_warning, sizeof(T));
	memcpy((void *)&max_warning, (void *)&new_max_warning, sizeof(T));

//
// Then, update database
//

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "max_warning")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}


	if (Tango::Util::_UseDb == true)
	{
		if(user_defaults && max_warning_tmp_str == usr_def_val)
		{
			DbDatum attr_dd(name), prop_dd("max_warning");
			DbData db_data;
			db_data.push_back(attr_dd);
			db_data.push_back(prop_dd);

			bool retry = true;
			while (retry == true)
			{
				try
				{
					tg->get_database()->delete_device_attribute_property(ext->d_name,db_data);
					retry = false;
				}
				catch (CORBA::COMM_FAILURE)
				{
					tg->get_database()->reconnect(true);
				}
			}
		}
		else
		{
			try
			{
				upd_att_prop_db(max_warning,"max_warning");
			}
			catch (Tango::DevFailed &)
			{
				memcpy((void *)&max_warning, (void *)&old_max_warning, sizeof(T));
				throw;
			}
		}
	}

//
// Set the max_warn flag
//

	alarm_conf.set(max_warn);

//
// Store new warning as a string
//

	max_warning_str = max_warning_tmp_str;

//
// Push a att conf event
//

	if (tg->is_svr_starting() == false)
		get_att_device()->push_att_conf_event(this);
}

template <>
inline void Attribute::set_max_warning(const string &new_max_warning_str)
{
	string max_warning_str_tmp = new_max_warning_str;
	string dev_name = ext->d_name;

	Tango::DeviceClass *dev_class = get_att_device_class(ext->d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	bool user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "max_warning")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	bool set_value = true;
	if(user_defaults)
	{
		if(TG_strcasecmp(new_max_warning_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("max_warning");
				attr_dd << 1L;
				prop_dd << AlrmValueNotSpec;
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->put_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(max_warn);
			max_warning_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
		else if ((TG_strcasecmp(new_max_warning_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_max_warning_str.c_str(),usr_def_val.c_str()) == 0) ||
				(strlen(new_max_warning_str.c_str()) == 0))
			max_warning_str_tmp = usr_def_val;
	}
	else
	{
		if ((TG_strcasecmp(new_max_warning_str.c_str(),AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(new_max_warning_str.c_str(),NotANumber) == 0) ||
				(strlen(new_max_warning_str.c_str()) == 0))
		{
			set_value = false;

			Tango::Util *tg = Tango::Util::instance();
			Tango::TangoMonitor *mon_ptr = NULL;
			if (tg->is_svr_starting() == false)
				mon_ptr = &(get_att_device()->get_att_conf_monitor());
			AutoTangoMonitor sync1(mon_ptr);

			if (Tango::Util::_UseDb == true)
			{
				DbDatum attr_dd(name), prop_dd("max_warning");
				DbData db_data;
				db_data.push_back(attr_dd);
				db_data.push_back(prop_dd);

				bool retry = true;
				while (retry == true)
				{
					try
					{
						tg->get_database()->delete_device_attribute_property(dev_name,db_data);
						retry = false;
					}
					catch (CORBA::COMM_FAILURE)
					{
						tg->get_database()->reconnect(true);
					}
				}
			}

			alarm_conf.reset(max_warn);
			max_warning_str = AlrmValueNotSpec;

			if (tg->is_svr_starting() == false)
				get_att_device()->push_att_conf_event(this);
		}
	}

	if(set_value)
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			double db;
			float fl;

			TangoSys_MemStream str;
			str << max_warning_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				set_max_warning((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				set_max_warning((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				set_max_warning((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				set_max_warning(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				set_max_warning(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				(db < 0.0) ? set_max_warning((DevUShort)(-db)) : set_max_warning((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				(db < 0.0) ? set_max_warning((DevUChar)(-db)) : set_max_warning((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				(db < 0.0) ? set_max_warning((DevULong)(-db)) : set_max_warning((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				(db < 0.0) ? set_max_warning((DevULong64)(-db)) : set_max_warning((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning");
				(db < 0.0) ? set_max_warning((DevUChar)(-db)) : set_max_warning((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("max_warning",dev_name,"Attribute::set_max_warning");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_max_warning()
//
// description :	Gets attribute's maximum warning value and assigns it
//					to the variable provided as a parameter
//					Throws exception in case the data type of provided
//					parameter does not match the attribute data type
//					or if maximum warning is not defined
//
// in :	max_war : The variable to be assigned the attribute's
//					maximum warning value
//
//--------------------------------------------------------------------------

template <typename T>
void Attribute::get_max_warning(T &max_war)
{
	if (data_type != ranges_type2const<T>::enu)
	{
		string err_msg = "Attribute data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"Attribute::get_max_warning()");
	}

	if (!alarm_conf[max_warn])
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Maximum warning not defined for this attribute",
					(const char *)"Attribute::get_max_warning()");
	}

	memcpy((void *)&max_war,(void *)&max_warning,sizeof(T));
}


template <typename T>
void Attribute::get_properties(Tango::MultiAttrProp<T> &props)
{

//
// Get the monitor protecting device att config
// If the server is in its starting phase, gives a NULL ptr
// to the AutoLock object
//

    Tango::Util *tg = Tango::Util::instance();
    Tango::TangoMonitor *mon_ptr = NULL;
    if (tg->is_svr_starting() == false)
        mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

	AttributeConfig_3 conf;
	get_properties_3(conf);

	props.label = conf.label;
	props.description = conf.description;
	props.unit = conf.unit;
	props.standard_unit = conf.standard_unit;
	props.display_unit = conf.display_unit;
	props.format = conf.format;
	props.min_alarm = conf.att_alarm.min_alarm;
	props.max_alarm = conf.att_alarm.max_alarm;
	props.min_value = conf.min_value;
	props.max_value = conf.max_value;
	props.min_warning = conf.att_alarm.min_warning;
	props.max_warning = conf.att_alarm.max_warning;
	props.delta_t = conf.att_alarm.delta_t;
	props.delta_val = conf.att_alarm.delta_val;
	props.event_period = conf.event_prop.per_event.period;
	props.archive_period = conf.event_prop.arch_event.period;
	props.rel_change = conf.event_prop.ch_event.rel_change;
	props.abs_change = conf.event_prop.ch_event.abs_change;
	props.archive_rel_change = conf.event_prop.arch_event.rel_change;
	props.archive_abs_change = conf.event_prop.arch_event.abs_change;
}

template <typename T>
void Attribute::set_properties(Tango::MultiAttrProp<T> &props)
{

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

    Tango::Util *tg = Tango::Util::instance();
    Tango::TangoMonitor *mon_ptr = NULL;
    if (tg->is_svr_starting() == false)
        mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Get current attribute configuration and update properties with provided values
//

	AttributeConfig_3 conf;
	get_properties_3(conf);

	conf.label = CORBA::string_dup(props.label.c_str());
	conf.description = CORBA::string_dup(props.description.c_str());
	conf.unit = CORBA::string_dup(props.unit.c_str());
	conf.standard_unit = CORBA::string_dup(props.standard_unit.c_str());
	conf.display_unit = CORBA::string_dup(props.display_unit.c_str());
	conf.format = CORBA::string_dup(props.format.c_str());
	conf.att_alarm.min_alarm = CORBA::string_dup(props.min_alarm);
	conf.att_alarm.max_alarm = CORBA::string_dup(props.max_alarm);
	conf.min_value = CORBA::string_dup(props.min_value);
	conf.max_value = CORBA::string_dup(props.max_value);
	conf.att_alarm.min_warning = CORBA::string_dup(props.min_warning);
	conf.att_alarm.max_warning = CORBA::string_dup(props.max_warning);
	conf.att_alarm.delta_t = CORBA::string_dup(props.delta_t);
	conf.att_alarm.delta_val = CORBA::string_dup(props.delta_val);
	conf.event_prop.per_event.period = CORBA::string_dup(props.event_period);
	conf.event_prop.arch_event.period = CORBA::string_dup(props.archive_period);
	conf.event_prop.ch_event.rel_change = CORBA::string_dup(props.rel_change);
	conf.event_prop.ch_event.abs_change = CORBA::string_dup(props.abs_change);
	conf.event_prop.arch_event.rel_change = CORBA::string_dup(props.archive_rel_change);
	conf.event_prop.arch_event.abs_change = CORBA::string_dup(props.archive_abs_change);

//
// Set properties and update database
//

	set_upd_properties(conf,ext->d_name);

//
// Push a att conf event
//

    if (tg->is_svr_starting() == false)
        get_att_device()->push_att_conf_event(this);
}

} // End of Tango namespace
#endif // _ATTRIBUTE_TPP
