//+==================================================================================================================
//
// file :               attribute_spec.tpp
//
// description :        C++ source code for the Attribute class template methods when they are specialized
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2013
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
// $Revision$
//
//-=================================================================================================================

#ifndef _ATTRIBUTE_SPEC_TPP
#define _ATTRIBUTE_SPEC_TPP

//
// These methods are in a sepearte files because I did not find a way to explicitely instanciate specialized
// templates. Therefore, for template management we have three files:
// 1 - A file with explicit template instanciation (templ_inst.cpp)
// 2 - A file with template method definition (attribute.tpp for Attribute class)
// 3 - A file with template specialization defnition (attribute_spec.tpp (this file))
//
// We did this in order to have explicit instanciation of templates method except for specialized templates
// for which we have instanciation following the inclusion model
// See C++ template book chapter 6
//

namespace Tango
{

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_min_alarm()
//		Template specialization for data type DevEncoded and string
//
// description :
//		Sets minimum alarm attribute property. Throws exception in case the data type of provided property does not
//		match the attribute data type
//
// args :
// 		in :
//			- new_min_alarm : The minimum alarm property to be set
//
//-------------------------------------------------------------------------------------------------------------------

template <>
inline void Attribute::set_min_alarm(const Tango::DevEncoded &)
{
	string err_msg = "Attribute properties cannot be set with Tango::DevEncoded data type";
	Except::throw_exception((const char *)API_MethodArgument,
				  (const char *)err_msg.c_str(),
				  (const char *)"Attribute::set_min_alarm()");
}

template <>
inline void Attribute::set_min_alarm(const string &new_min_alarm_str)
{
	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("min_alarm",d_name,"Attribute::set_min_alarm()");

	string min_alarm_str_tmp = new_min_alarm_str;
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

    user_defaults = prop_in_list("min_alarm",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("min_alarm",class_def_val,nb_class,def_class_prop);

	bool set_value = true;

    if (class_defaults)
    {
		if(TG_strcasecmp(new_min_alarm_str.c_str(),AlrmValueNotSpec) == 0)
		{
		    set_value = false;

		    avns_in_db("min_alarm",dev_name);
            avns_in_att(MIN_ALARM);
		}
		else if ((TG_strcasecmp(new_min_alarm_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_min_alarm_str.c_str(),class_def_val.c_str()) == 0))
        {
            min_alarm_str_tmp = class_def_val;
        }
		else if (strlen(new_min_alarm_str.c_str()) == 0)
		{
		    if (user_defaults)
		    {
                min_alarm_str_tmp = usr_def_val;
		    }
		    else
		    {
		        set_value = false;

                avns_in_db("min_alarm",dev_name);
                avns_in_att(MIN_ALARM);
		    }
		}
    }
	else if(user_defaults)
	{
		if(TG_strcasecmp(new_min_alarm_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

            avns_in_db("min_alarm",dev_name);
            avns_in_att(MIN_ALARM);
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

            avns_in_db("min_alarm",dev_name);
            avns_in_att(MIN_ALARM);
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
			str << min_alarm_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				set_min_alarm((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				set_min_alarm((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				set_min_alarm((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				set_min_alarm(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				set_min_alarm(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				(db < 0.0) ? set_min_alarm((DevUShort)(-db)) : set_min_alarm((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				(db < 0.0) ? set_min_alarm((DevUChar)(-db)) : set_min_alarm((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				(db < 0.0) ? set_min_alarm((DevULong)(-db)) : set_min_alarm((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				(db < 0.0) ? set_min_alarm((DevULong64)(-db)) : set_min_alarm((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_min_alarm()");
				(db < 0.0) ? set_min_alarm((DevUChar)(-db)) : set_min_alarm((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("min_alarm",dev_name,"Attribute::set_min_alarm()");
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_max_alarm()
//		Template specialization for data type DevEncoded and string
//
// description :
//		Sets maximum alarm attribute property. Throws exception in case the data type of provided property does not
//		match the attribute data type
//
// args :
// 		in :
//			- new_min_alarm : The minimum alarm property to be set
//
//-------------------------------------------------------------------------------------------------------------------

template <>
inline void Attribute::set_max_alarm(const Tango::DevEncoded &)
{
	string err_msg = "Attribute properties cannot be set with Tango::DevEncoded data type";
	Except::throw_exception((const char *)API_MethodArgument,
				  (const char *)err_msg.c_str(),
				  (const char *)"Attribute::set_max_alarm()");
}

template <>
inline void Attribute::set_max_alarm(const string &new_max_alarm_str)
{
	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("max_alarm",d_name,"Attribute::set_max_alarm()");

	string max_alarm_str_tmp = new_max_alarm_str;
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

    user_defaults = prop_in_list("max_alarm",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("max_alarm",class_def_val,nb_class,def_class_prop);

	bool set_value = true;

    if (class_defaults)
    {
		if(TG_strcasecmp(new_max_alarm_str.c_str(),AlrmValueNotSpec) == 0)
		{
		    set_value = false;

		    avns_in_db("max_alarm",dev_name);
            avns_in_att(MAX_ALARM);
		}
		else if ((TG_strcasecmp(new_max_alarm_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_max_alarm_str.c_str(),class_def_val.c_str()) == 0))
        {
            max_alarm_str_tmp = class_def_val;
        }
		else if (strlen(new_max_alarm_str.c_str()) == 0)
		{
		    if (user_defaults)
		    {
                max_alarm_str_tmp = usr_def_val;
		    }
		    else
		    {
		        set_value = false;

                avns_in_db("max_alarm",dev_name);
                avns_in_att(MAX_ALARM);
		    }
		}
    }
	else if(user_defaults)
	{
		if(TG_strcasecmp(new_max_alarm_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

            avns_in_db("max_alarm",dev_name);
            avns_in_att(MAX_ALARM);
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

            avns_in_db("max_alarm",dev_name);
            avns_in_att(MAX_ALARM);
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
			str << max_alarm_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				set_max_alarm((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				set_max_alarm((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				set_max_alarm((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				set_max_alarm(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				set_max_alarm(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				(db < 0.0) ? set_max_alarm((DevUShort)(-db)) : set_max_alarm((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				(db < 0.0) ? set_max_alarm((DevUChar)(-db)) : set_max_alarm((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				(db < 0.0) ? set_max_alarm((DevULong)(-db)) : set_max_alarm((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				(db < 0.0) ? set_max_alarm((DevULong64)(-db)) : set_max_alarm((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_max_alarm()");
				(db < 0.0) ? set_max_alarm((DevUChar)(-db)) : set_max_alarm((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("max_alarm",dev_name,"Attribute::set_max_alarm()");
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_min_warning()
//		Template specialization for data type DevEncoded and string
//
// description :
//		Sets minimum warning attribute property. Throws exception in case the data type of provided property does not
//		match the attribute data type
//
// args :
// 		in :
//			- new_min_alarm : The minimum alarm property to be set
//
//-------------------------------------------------------------------------------------------------------------------

template <>
inline void Attribute::set_min_warning(const Tango::DevEncoded &)
{
	string err_msg = "Attribute properties cannot be set with Tango::DevEncoded data type";
	Except::throw_exception((const char *)API_MethodArgument,
				  (const char *)err_msg.c_str(),
				  (const char *)"Attribute::set_min_warning()");
}

template <>
inline void Attribute::set_min_warning(const string &new_min_warning_str)
{
	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("min_warning",d_name,"Attribute::set_min_warning()");

	string min_warning_str_tmp = new_min_warning_str;
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

    user_defaults = prop_in_list("min_warning",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("min_warning",class_def_val,nb_class,def_class_prop);

	bool set_value = true;

    if (class_defaults)
    {
		if(TG_strcasecmp(new_min_warning_str.c_str(),AlrmValueNotSpec) == 0)
		{
		    set_value = false;

		    avns_in_db("min_warning",dev_name);
            avns_in_att(MIN_WARNING);
		}
		else if ((TG_strcasecmp(new_min_warning_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_min_warning_str.c_str(),class_def_val.c_str()) == 0))
        {
            min_warning_str_tmp = class_def_val;
        }
		else if (strlen(new_min_warning_str.c_str()) == 0)
		{
		    if (user_defaults)
		    {
                min_warning_str_tmp = usr_def_val;
		    }
		    else
		    {
		        set_value = false;

                avns_in_db("min_warning",dev_name);
                avns_in_att(MIN_WARNING);
		    }
		}
    }
	else if(user_defaults)
	{
		if(TG_strcasecmp(new_min_warning_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

            avns_in_db("min_warning",dev_name);
            avns_in_att(MIN_WARNING);
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

            avns_in_db("min_warning",dev_name);
            avns_in_att(MIN_WARNING);
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
			str << min_warning_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				set_min_warning((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				set_min_warning((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				set_min_warning((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				set_min_warning(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				set_min_warning(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				(db < 0.0) ? set_min_warning((DevUShort)(-db)) : set_min_warning((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				(db < 0.0) ? set_min_warning((DevUChar)(-db)) : set_min_warning((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				(db < 0.0) ? set_min_warning((DevULong)(-db)) : set_min_warning((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				(db < 0.0) ? set_min_warning((DevULong64)(-db)) : set_min_warning((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_min_warning()");
				(db < 0.0) ? set_min_warning((DevUChar)(-db)) : set_min_warning((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("min_warning",dev_name,"Attribute::set_min_warning()");
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_max_warning()
//		Template specialization for data type DevEncoded and string
//
// description :
//		Sets maximum warning attribute property. Throws exception in case the data type of provided property does not
//		match the attribute data type
//
// args :
// 		in :
//			- new_min_alarm : The minimum alarm property to be set
//
//-------------------------------------------------------------------------------------------------------------------

template <>
inline void Attribute::set_max_warning(const Tango::DevEncoded &)
{
	string err_msg = "Attribute properties cannot be set with Tango::DevEncoded data type";
	Except::throw_exception((const char *)API_MethodArgument,
				  (const char *)err_msg.c_str(),
				  (const char *)"Attribute::set_max_warning()");
}

template <>
inline void Attribute::set_max_warning(const string &new_max_warning_str)
{
	if((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE))
		throw_err_data_type("max_warning",d_name,"Attribute::set_max_warning()");

	string max_warning_str_tmp = new_max_warning_str;
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

    user_defaults = prop_in_list("max_warning",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("max_warning",class_def_val,nb_class,def_class_prop);

	bool set_value = true;

    if (class_defaults)
    {
		if(TG_strcasecmp(new_max_warning_str.c_str(),AlrmValueNotSpec) == 0)
		{
		    set_value = false;

		    avns_in_db("max_warning",dev_name);
            avns_in_att(MAX_WARNING);
		}
		else if ((TG_strcasecmp(new_max_warning_str.c_str(),NotANumber) == 0) ||
				(TG_strcasecmp(new_max_warning_str.c_str(),class_def_val.c_str()) == 0))
        {
            max_warning_str_tmp = class_def_val;
        }
		else if (strlen(new_max_warning_str.c_str()) == 0)
		{
		    if (user_defaults)
		    {
                max_warning_str_tmp = usr_def_val;
		    }
		    else
		    {
		        set_value = false;

                avns_in_db("max_warning",dev_name);
                avns_in_att(MAX_WARNING);
		    }
		}
    }
	else if(user_defaults)
	{
		if(TG_strcasecmp(new_max_warning_str.c_str(),AlrmValueNotSpec) == 0)
		{
			set_value = false;

            avns_in_db("max_warning",dev_name);
            avns_in_att(MAX_WARNING);
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

            avns_in_db("max_warning",dev_name);
            avns_in_att(MAX_WARNING);
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
			str << max_warning_str_tmp;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				set_max_warning((DevShort)db);
				break;

			case Tango::DEV_LONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				set_max_warning((DevLong)db);
				break;

			case Tango::DEV_LONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				set_max_warning((DevLong64)db);
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				set_max_warning(db);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> fl && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				set_max_warning(fl);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				(db < 0.0) ? set_max_warning((DevUShort)(-db)) : set_max_warning((DevUShort)db);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				(db < 0.0) ? set_max_warning((DevUChar)(-db)) : set_max_warning((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				(db < 0.0) ? set_max_warning((DevULong)(-db)) : set_max_warning((DevULong)db);
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				(db < 0.0) ? set_max_warning((DevULong64)(-db)) : set_max_warning((DevULong64)db);
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_max_warning()");
				(db < 0.0) ? set_max_warning((DevUChar)(-db)) : set_max_warning((DevUChar)db);
				break;
			}
		}
		else
			throw_err_data_type("max_warning",dev_name,"Attribute::set_max_warning()");
	}
}

} // End of Tango namespace
#endif // _ATTRIBUTE_SPEC_TPP
