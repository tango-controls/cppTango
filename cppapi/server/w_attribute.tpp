//+============================================================================
//
// file :               WAttribute.tpp
//
// description :        C++ source code for the WAttribute class template
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

#ifndef _WATTRIBUTE_TPP
#define _WATTRIBUTE_TPP

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_min_value()
//
// description : 	Sets minimum value attribute property
//					Throws exception in case the data type of provided
//					property does not match the attribute data type
//
// in :	new_min_value : The minimum value property to be set
//
//--------------------------------------------------------------------------

template <typename T>
void WAttribute::set_min_value(const T &new_min_value)
{

//
// Check type validity
//

	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("min_value",d_name,"WAttribute::set_min_value()");

	if (!(data_type == DEV_ENCODED && ranges_type2const<T>::enu == DEV_UCHAR) &&
		(data_type != ranges_type2const<T>::enu))
	{
		string err_msg = "Attribute (" + name + ") data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"WAttribute::set_min_value()");
	}

//
//	Check coherence with max_value
//

	if(check_max_value)
	{
		T max_value_tmp;
		memcpy((void *) &max_value_tmp, (const void *) &max_value, sizeof(T));
		if(new_min_value >= max_value_tmp)
			throw_incoherent_val_err("min_value","max_value",d_name,"WAttribute::set_min_value()");
	}

//
// Store new min value as a string
//

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_min_value; // to represent the numeric value
	else
		str << new_min_value;
	string min_value_tmp_str = str.str();

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::TangoMonitor *mon_ptr = NULL;
	if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
		mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Store the new value locally
//

	Attr_CheckVal old_min_value;
	memcpy((void *)&old_min_value, (void *)&min_value, sizeof(T));
	memcpy((void *)&min_value, (void *)&new_min_value, sizeof(T));

//
// Then, update database
//

	Tango::DeviceClass *dev_class = get_att_device_class(d_name);
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
			if (def_user_prop[i].get_name() == "min_value")
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
		if(user_defaults && min_value_tmp_str == usr_def_val)
		{
			DbDatum attr_dd(name), prop_dd("min_value");
			DbData db_data;
			db_data.push_back(attr_dd);
			db_data.push_back(prop_dd);

			bool retry = true;
			while (retry == true)
			{
				try
				{
					tg->get_database()->delete_device_attribute_property(d_name,db_data);
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
				upd_att_prop_db(min_value,"min_value");
			}
			catch (Tango::DevFailed &)
			{
				memcpy((void *)&min_value, (void *)&old_min_value, sizeof(T));
				throw;
			}
		}
	}

//
// Set the min_value flag
//

	check_min_value = true;

//
// Store new value as a string
//

	min_value_str = min_value_tmp_str;

//
// Push a att conf event
//

	if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
		get_att_device()->push_att_conf_event(this);

//
// Delete device startup exception related to min_value if there is any
//

	delete_startup_exception("min_value");
}

template <>
inline void WAttribute::set_min_value(const string &new_min_value_str)
{
	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("min_value",d_name,"WAttribute::set_min_value()");

	string min_value_str_tmp = new_min_value_str;
	string dev_name = d_name;

	Tango::DeviceClass *dev_class = get_att_device_class(d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);

	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	vector<AttrProperty> &def_class_prop = att.get_class_properties();

	size_t nb_class = def_class_prop.size();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	string class_def_val;
	bool user_defaults = false;
	bool class_defaults = false;

    user_defaults = prop_in_list("min_value",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("min_value",class_def_val,nb_class,def_class_prop);

	bool set_value = true;

    if (class_defaults)
    {
		if(TG_strcasecmp(new_min_value_str.c_str(),AlrmValueNotSpec) == 0)
		{
		    set_value = false;

		    avns_in_db("min_value",dev_name);
            avns_in_att(MIN_VALUE);
		}
		else if ((TG_strcasecmp(new_min_value_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_min_value_str.c_str(),class_def_val.c_str()) == 0))
        {
            min_value_str_tmp = class_def_val;
        }
		else if (strlen(new_min_value_str.c_str()) == 0)
		{
		    if (user_defaults)
		    {
                min_value_str_tmp = usr_def_val;
		    }
		    else
		    {
		        set_value = false;

                avns_in_db("min_value",dev_name);
                avns_in_att(MIN_VALUE);
		    }
		}
    }
	else if(user_defaults)
	{
		if(TG_strcasecmp(new_min_value_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

            avns_in_db("min_value",dev_name);
            avns_in_att(MIN_VALUE);
		}
		else if ((TG_strcasecmp(new_min_value_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_min_value_str.c_str(),usr_def_val.c_str()) == 0) ||
				(strlen(new_min_value_str.c_str()) == 0))
			min_value_str_tmp = usr_def_val;
	}
	else
	{
		if ((TG_strcasecmp(new_min_value_str.c_str(),AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(new_min_value_str.c_str(),NotANumber) == 0) ||
				(strlen(new_min_value_str.c_str()) == 0))
		{
			set_value = false;

            avns_in_db("min_value",dev_name);
            avns_in_att(MIN_VALUE);
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
			str.precision(TANGO_FLOAT_PRECISION);
			str << min_value_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				set_min_value((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				set_min_value((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				set_min_value((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				set_min_value(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				set_min_value(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				(db < 0.0) ? set_min_value((DevUShort)(-db)) : set_min_value((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				(db < 0.0) ? set_min_value((DevUChar)(-db)) : set_min_value((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				(db < 0.0) ? set_min_value((DevULong)(-db)) : set_min_value((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				(db < 0.0) ? set_min_value((DevULong64)(-db)) : set_min_value((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("min_value",dev_name,"WAttribute::set_min_value()");
				(db < 0.0) ? set_min_value((DevUChar)(-db)) : set_min_value((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("min_value",dev_name,"WAttribute::set_min_value()");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::get_min_value()
//
// description :	Gets attribute's minimum value and assigns it
//					to the variable provided as a parameter
//					Throws exception in case the data type of provided
//					parameter does not match the attribute data type
//					or if minimum value is not defined
//
// in :	min_val : The variable to be assigned the attribute's
//					minimum value
//
//--------------------------------------------------------------------------

template <typename T>
void WAttribute::get_min_value(T &min_val)
{
	if (!(data_type == DEV_ENCODED && ranges_type2const<T>::enu == DEV_UCHAR) &&
		(data_type != ranges_type2const<T>::enu))
	{
		string err_msg = "Attribute (" + name + ") data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"WAttribute::get_min_value()");
	}

	if (check_min_value == false)
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Minimum value not defined for this attribute",
					(const char *)"WAttribute::get_min_value()");
	}

	memcpy((void *)&min_val,(void *)&min_value,sizeof(T));
}


//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_max_value()
//
// description : 	Sets maximum value attribute property
//					Throws exception in case the data type of provided
//					property does not match the attribute data type
//
// in :	new_max_value : The maximum value property to be set
//
//--------------------------------------------------------------------------

template <typename T>
void WAttribute::set_max_value(const T &new_max_value)
{

//
// Check type validity
//

	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("max_value",d_name,"WAttribute::set_max_value()");

	if (!(data_type == DEV_ENCODED && ranges_type2const<T>::enu == DEV_UCHAR) &&
		(data_type != ranges_type2const<T>::enu))
	{
		string err_msg = "Attribute (" + name + ") data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"WAttribute::set_max_value()");
	}

//
//	Check coherence with max_value
//

	if(check_min_value)
	{
		T min_value_tmp;
		memcpy((void *) &min_value_tmp, (const void *) &min_value, sizeof(T));
		if(new_max_value <= min_value_tmp)
			throw_incoherent_val_err("min_value","max_value",d_name,"WAttribute::set_max_value()");
	}

//
// Store new max value as a string
//

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);
	if(ranges_type2const<T>::enu == Tango::DEV_UCHAR)
		str << (short)new_max_value; // to represent the numeric value
	else
		str << new_max_value;
	string max_value_tmp_str = str.str();

//
// Get the monitor protecting device att config
// If the server is in its starting phase, give a NULL ptr
// to the AutoLock object
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::TangoMonitor *mon_ptr = NULL;
	if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
		mon_ptr = &(get_att_device()->get_att_conf_monitor());
	AutoTangoMonitor sync1(mon_ptr);

//
// Store the new value locally
//

	Attr_CheckVal old_max_value;
	memcpy((void *)&old_max_value, (void *)&max_value, sizeof(T));
	memcpy((void *)&max_value, (void *)&new_max_value, sizeof(T));

//
// Then, update database
//

	Tango::DeviceClass *dev_class = get_att_device_class(d_name);
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
			if (def_user_prop[i].get_name() == "max_value")
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
		if(user_defaults && max_value_tmp_str == usr_def_val)
		{
			DbDatum attr_dd(name), prop_dd("max_value");
			DbData db_data;
			db_data.push_back(attr_dd);
			db_data.push_back(prop_dd);

			bool retry = true;
			while (retry == true)
			{
				try
				{
					tg->get_database()->delete_device_attribute_property(d_name,db_data);
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
				upd_att_prop_db(max_value,"max_value");
			}
			catch (Tango::DevFailed &)
			{
				memcpy((void *)&max_value, (void *)&old_max_value, sizeof(T));
				throw;
			}
		}
	}

//
// Set the max_value flag
//

	check_max_value = true;

//
// Store new value as a string
//

	max_value_str = max_value_tmp_str;

//
// Push a att conf event
//

	if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
		get_att_device()->push_att_conf_event(this);

//
// Delete device startup exception related to max_value if there is any
//

	delete_startup_exception("max_value");
}

template <>
inline void WAttribute::set_max_value(const string &new_max_value_str)
{
	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("max_value",d_name,"WAttribute::set_max_value()");

	string max_value_str_tmp = new_max_value_str;
	string dev_name = d_name;

	Tango::DeviceClass *dev_class = get_att_device_class(d_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);

	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	vector<AttrProperty> &def_class_prop = att.get_class_properties();

	size_t nb_class = def_class_prop.size();
	size_t nb_user = def_user_prop.size();

	string usr_def_val;
	string class_def_val;
	bool user_defaults = false;
	bool class_defaults = false;

    user_defaults = prop_in_list("max_value",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("max_value",class_def_val,nb_class,def_class_prop);

	bool set_value = true;

    if (class_defaults)
    {
		if(TG_strcasecmp(new_max_value_str.c_str(),AlrmValueNotSpec) == 0)
		{
		    set_value = false;

		    avns_in_db("max_value",dev_name);
            avns_in_att(MAX_VALUE);
		}
		else if ((TG_strcasecmp(new_max_value_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_max_value_str.c_str(),class_def_val.c_str()) == 0))
        {
            max_value_str_tmp = class_def_val;
        }
		else if (strlen(new_max_value_str.c_str()) == 0)
		{
		    if (user_defaults)
		    {
                max_value_str_tmp = usr_def_val;
		    }
		    else
		    {
		        set_value = false;

                avns_in_db("max_value",dev_name);
                avns_in_att(MAX_VALUE);
		    }
		}
    }
	else if(user_defaults)
	{
		if(TG_strcasecmp(new_max_value_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

            avns_in_db("max_value",dev_name);
            avns_in_att(MAX_VALUE);
		}
		else if ((TG_strcasecmp(new_max_value_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_max_value_str.c_str(),usr_def_val.c_str()) == 0) ||
				(strlen(new_max_value_str.c_str()) == 0))
			max_value_str_tmp = usr_def_val;
	}
	else
	{
		if ((TG_strcasecmp(new_max_value_str.c_str(),AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(new_max_value_str.c_str(),NotANumber) == 0) ||
				(strlen(new_max_value_str.c_str()) == 0))
		{
			set_value = false;

            avns_in_db("max_value",dev_name);
            avns_in_att(MAX_VALUE);
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
			str.precision(TANGO_FLOAT_PRECISION);
			str << max_value_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				set_max_value((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				set_max_value((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				set_max_value((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				set_max_value(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				set_max_value(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				(db < 0.0) ? set_max_value((DevUShort)(-db)) : set_max_value((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				(db < 0.0) ? set_max_value((DevUChar)(-db)) : set_max_value((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				(db < 0.0) ? set_max_value((DevULong)(-db)) : set_max_value((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				(db < 0.0) ? set_max_value((DevULong64)(-db)) : set_max_value((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("max_value",dev_name,"WAttribute::set_max_value()");
				(db < 0.0) ? set_max_value((DevUChar)(-db)) : set_max_value((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("max_value",dev_name,"WAttribute::set_max_value()");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::get_max_value()
//
// description :	Gets attribute's maximum value and assigns it
//					to the variable provided as a parameter
//					Throws exception in case the data type of provided
//					parameter does not match the attribute data type
//					or if maximum value is not defined
//
// in :	max_val : The variable to be assigned the attribute's
//					maximum value
//
//--------------------------------------------------------------------------

template <typename T>
void WAttribute::get_max_value(T &max_val)
{
	if (!(data_type == DEV_ENCODED && ranges_type2const<T>::enu == DEV_UCHAR) &&
		(data_type != ranges_type2const<T>::enu))
	{
		string err_msg = "Attribute (" + name + ") data type does not match the type provided : " + ranges_type2const<T>::str;
		Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					  (const char *)err_msg.c_str(),
					  (const char *)"WAttribute::get_max_value()");
	}

	if (check_max_value == false)
	{
		Except::throw_exception((const char *)"API_AttrNotAllowed",
					(const char *)"Minimum value not defined for this attribute",
					(const char *)"WAttribute::get_max_value()");
	}

	memcpy((void *)&max_val,(void *)&max_value,sizeof(T));
}

} // End of Tango namespace
#endif // _WATTRIBUTE_TPP
