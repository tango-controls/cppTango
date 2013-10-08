//+===================================================================================================================
//
// file :               w_attribute_spec.tpp
//
// description :        C++ source code for the WAttribute class template methods specialization
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
// $Revision: 17240 $
//
//-==================================================================================================================

#ifndef _WATTRIBUTE_SPEC_TPP
#define _WATTRIBUTE_SPEC_TPP

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		WAttribute::set_min_value()
//
// description :
//		Sets minimum value attribute property. Throws exception in case the data type of provided
//		property does not match the attribute data type
//
// args :
//		in :
// 			- new_min_value_str : The minimum value property to be set
//
//-------------------------------------------------------------------------------------------------------------------

template <>
inline void WAttribute::set_min_value(const Tango::DevEncoded &)
{
	string err_msg = "Attribute properties cannot be set with Tango::DevEncoded data type";
	Except::throw_exception((const char *)API_MethodArgument,
				  (const char *)err_msg.c_str(),
				  (const char *)"WAttribute::set_min_value()");
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


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		WAttribute::set_max_value()
//
// description :
//		Sets maximum value attribute property. Throws exception in case the data type of provided
//		property does not match the attribute data type
//
// args :
//		in :
//			- new_max_value_str : The maximum value property to be set
//
//------------------------------------------------------------------------------------------------------------------

template <>
inline void WAttribute::set_max_value(const Tango::DevEncoded &)
{
	string err_msg = "Attribute properties cannot be set with Tango::DevEncoded data type";
	Except::throw_exception((const char *)API_MethodArgument,
				  (const char *)err_msg.c_str(),
				  (const char *)"WAttribute::set_max_value()");
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

} // End of Tango namespace

#endif // _WATTRIBUTE_SPEC_TPP
