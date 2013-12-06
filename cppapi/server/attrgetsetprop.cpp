static const char *RcsId = "$Id$";

//====================================================================================================================
//
// file :               attrgetsetprop.cpp
//
// description :        C++ source code for the Attribute class methods erlated to set/get attribute properties
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
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
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
//====================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attribute.h>
#include <classattribute.h>
#include <eventsupplier.h>

#include <functional>
#include <algorithm>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */
#include <math.h>

namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::get_properties
//
// description :
//		Init the Tango::AttributeConfig with all the attribute properties value
//
// argument :
//		out :
//			- conf : Structure in which attribute configuration must be stored
//
//------------------------------------------------------------------------------------------------------------------

void Attribute::get_properties(Tango::AttributeConfig &conf)
{

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = CORBA::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = CORBA::string_dup(label.c_str());
	conf.description = CORBA::string_dup(description.c_str());
	conf.unit = CORBA::string_dup(unit.c_str());
	conf.standard_unit = CORBA::string_dup(standard_unit.c_str());
	conf.display_unit = CORBA::string_dup(display_unit.c_str());
	conf.format = CORBA::string_dup(format.c_str());
	conf.writable_attr_name = CORBA::string_dup(writable_attr_name.c_str());
	conf.min_alarm = CORBA::string_dup(min_alarm_str.c_str());
	conf.max_alarm = CORBA::string_dup(max_alarm_str.c_str());
	conf.min_value = CORBA::string_dup(min_value_str.c_str());
	conf.max_value = CORBA::string_dup(max_value_str.c_str());
}

void Attribute::get_properties_2(Tango::AttributeConfig_2 &conf)
{

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = CORBA::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = CORBA::string_dup(label.c_str());
	conf.description = CORBA::string_dup(description.c_str());
	conf.unit = CORBA::string_dup(unit.c_str());
	conf.standard_unit = CORBA::string_dup(standard_unit.c_str());
	conf.display_unit = CORBA::string_dup(display_unit.c_str());
	conf.format = CORBA::string_dup(format.c_str());
	conf.writable_attr_name = CORBA::string_dup(writable_attr_name.c_str());
	conf.min_alarm = CORBA::string_dup(min_alarm_str.c_str());
	conf.max_alarm = CORBA::string_dup(max_alarm_str.c_str());
	conf.min_value = CORBA::string_dup(min_value_str.c_str());
	conf.max_value = CORBA::string_dup(max_value_str.c_str());

	conf.level = disp_level;
}

void Attribute::get_properties_3(Tango::AttributeConfig_3 &conf)
{

//
// Throw exception in case of Fwd attribute and the att configuration is not yet received
//

	if (data_type == DATA_TYPE_UNKNOWN)
	{
		string desc("Attribute ");
		FwdAttribute *fwd = static_cast<FwdAttribute *>(this);
		desc = desc + get_name() + " is a forwarded attribute and its root device (";
		desc = desc + fwd->get_fwd_dev_name();
		desc = desc + ") is not yet available";
		Tango::Except::throw_exception(API_AttrConfig,desc,"Attribute::get_properties_3");
	}

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = CORBA::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = CORBA::string_dup(label.c_str());
	conf.description = CORBA::string_dup(description.c_str());
	conf.unit = CORBA::string_dup(unit.c_str());
	conf.standard_unit = CORBA::string_dup(standard_unit.c_str());
	conf.display_unit = CORBA::string_dup(display_unit.c_str());
	conf.format = CORBA::string_dup(format.c_str());
	conf.writable_attr_name = CORBA::string_dup(writable_attr_name.c_str());
	conf.min_value = CORBA::string_dup(min_value_str.c_str());
	conf.max_value = CORBA::string_dup(max_value_str.c_str());
	conf.level = disp_level;

//
// Copy alarm properties
//

	conf.att_alarm.min_alarm = CORBA::string_dup(min_alarm_str.c_str());
	conf.att_alarm.max_alarm = CORBA::string_dup(max_alarm_str.c_str());
	conf.att_alarm.min_warning = CORBA::string_dup(min_warning_str.c_str());
	conf.att_alarm.max_warning = CORBA::string_dup(max_warning_str.c_str());
	if (delta_t == 0)
		conf.att_alarm.delta_t = CORBA::string_dup(AlrmValueNotSpec);
	else
		conf.att_alarm.delta_t = CORBA::string_dup(delta_t_str.c_str());
	conf.att_alarm.delta_val = CORBA::string_dup(delta_val_str.c_str());

//
// Copy periodic event property
//

	TangoSys_OMemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	if (event_period == INT_MAX)
		conf.event_prop.per_event.period = CORBA::string_dup((const char *)(DEFAULT_EVENT_PERIOD));
	else
	{
		int per = (int)((double)event_period);
		str << per;
		MEM_STREAM_2_CORBA(conf.event_prop.per_event.period,str);
	}

//
// Copy change event properties
//

	if (fabs(rel_change[0]) == fabs(rel_change[1]))
	{
		if (rel_change[0] == INT_MAX)
			conf.event_prop.ch_event.rel_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(rel_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.rel_change,str);
		}
	}
	else
	{
		if (rel_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(rel_change[0]) << ",";

		if (rel_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(rel_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.ch_event.rel_change,str);
	}

	if (fabs(abs_change[0]) == fabs(abs_change[1]))
	{
		if (abs_change[0] == INT_MAX)
			conf.event_prop.ch_event.abs_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(abs_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.abs_change,str);
		}
	}
	else
	{
		if (abs_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(abs_change[0]) << ",";

		if (abs_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(abs_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.ch_event.abs_change,str);
	}

//
// Copy archive event properties
//

	if (archive_period == INT_MAX)
		conf.event_prop.arch_event.period = CORBA::string_dup(AlrmValueNotSpec);
	else
	{
		int per = (int)((double)archive_period);
		str << per;
		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.period,str);
	}

	if (fabs(archive_rel_change[0]) == fabs(archive_rel_change[1]))
	{
		if (archive_rel_change[0] == INT_MAX)
			conf.event_prop.arch_event.rel_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(archive_rel_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.rel_change,str);
		}
	}
	else
	{
		if (archive_rel_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(archive_rel_change[0]) << ",";

		if (archive_rel_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(archive_rel_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.rel_change,str);
	}

	if (fabs(archive_abs_change[0]) == fabs(archive_abs_change[1]))
	{
		if (archive_abs_change[0] == INT_MAX)
			conf.event_prop.arch_event.abs_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(archive_abs_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.abs_change,str);
		}
	}
	else
	{
		if (archive_abs_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(archive_abs_change[0]) << ",";

		if (archive_abs_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(archive_abs_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.abs_change,str);
	}
}

void Attribute::get_properties_5(Tango::AttributeConfig_5 &conf)
{

//
// Get config
//

	Tango::AttributeConfig_3 conf3;
	get_properties_3(conf3);

	conf.name = conf3.name;
	conf.writable = conf3.writable;
	conf.data_format = conf3.data_format;
	conf.data_type = conf3.data_type;
	conf.max_dim_x = conf3.max_dim_x;
	conf.max_dim_y = conf3.max_dim_y;
	conf.description = conf3.description;
	conf.label = conf3.label;
	conf.unit = conf3.unit;
	conf.standard_unit = conf3.standard_unit;
	conf.display_unit = conf3.display_unit;
	conf.format = conf3.format;
	conf.min_value = conf3.min_value;
	conf.max_value = conf3.max_value;
	conf.writable_attr_name = conf3.writable_attr_name;
	conf.level = conf3.level;
	conf.att_alarm = conf3.att_alarm;
	conf.event_prop = conf3.event_prop;
	conf.extensions = conf3.extensions;
	conf.sys_extensions = conf3.sys_extensions;

//
// Add AttributeConfig_5 specific data
//

	if (is_fwd_att() == true)
	{
		FwdAttribute *fwd = static_cast<FwdAttribute *>(this);
		string str(fwd->get_fwd_dev_name() + '/' + fwd->get_fwd_att_name());
		conf.root_attr_name = CORBA::string_dup(str.c_str());
	}
	else
		conf.root_attr_name = CORBA::string_dup(AlrmValueNotSpec);

	if (writable == WRITE || writable == READ_WRITE)
	{
		WAttribute *watt = static_cast<WAttribute *>(this);
		conf.memorized = watt->is_memorized();
		conf.mem_init = watt->is_memorized_init();
	}
	else
	{
		conf.memorized = false;
		conf.mem_init = false;
	}

//TODO:	conf.enum_labels;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_properties
//
// description :
//		Set the attribute properties value
//
// argument :
// 		in :
//			- conf : The new properties sent by client
//			- d : Pointer to the device object
//--------------------------------------------------------------------------------------------------------------------

void Attribute::set_properties(const Tango::AttributeConfig &conf,Tango::DeviceImpl *d)
{
	if (dev == NULL)
		dev = d;

	set_properties(conf,d->get_name());
}

void Attribute::set_properties(const Tango::AttributeConfig &conf,string &dev_name)
{

    if (name_lower == "state" || name_lower == "status")
        return;

//
// Check if the caller try to change "hard coded" properties. Throw exception in case of
//

	if (is_fwd_att() == true)
		set_hard_coded_properties(conf);
	else
		check_hard_coded_properties(conf);

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string means returns to its default value which could be the library default
// value or the user defined default value
//

    Tango::DeviceClass *dev_class = get_att_device_class(dev_name);

	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

// TODO : Add one call to refresh from DB the class properties

	vector<AttrProperty> &def_class_prop = att.get_class_properties();
	size_t nb_class = def_class_prop.size();

//
// First the description
//

	if(TG_strcasecmp(conf.description,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.description,DescNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		description = DescNotSpec;
	}
	else if (TG_strcasecmp(conf.description,NotANumber) == 0)
	{
		// set class default value if defined, otherwise use the user default or library defaults

		bool found = prop_in_list("description",description,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("description",description,nb_user,def_user_prop);
            if (found == false)
                description = DescNotSpec;
		}
	}
	else if (strlen(conf.description) == 0)
	{
		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("description",description,nb_user,def_user_prop);
        if (found == false)
            description = DescNotSpec;
	}
	else
	{
		// set property
		description = conf.description;
	}

	delete_startup_exception("description");

//
// The label
//

	if(TG_strcasecmp(conf.label,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.label,LabelNotSpec) == 0) ||
			(TG_strcasecmp(conf.label,name.c_str()) == 0))
	{
		// force library defaults (even if user defaults defined)
		label = name.c_str();
	}
	else if (TG_strcasecmp(conf.label,NotANumber) == 0)
	{
		// set class default value if defined, otherwise use the user default or library defaults

		bool found = prop_in_list("label",label,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("label",label,nb_user,def_user_prop);
            if (found == false)
                label = name.c_str();
		}
	}
	else if (strlen(conf.label) == 0)
	{

		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("label",label,nb_user,def_user_prop);
        if (found == false)
            label = name.c_str();
	}
	else
	{
		// set property
		label = conf.label;
	}

	delete_startup_exception("label");

//
// The unit
//

	if(TG_strcasecmp(conf.unit,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.unit,UnitNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		unit = UnitNotSpec;
	}
	else if(TG_strcasecmp(conf.unit,NotANumber) == 0)
	{
		// set class default value if defines, user default value if defined, otherwise use the library defaults

		bool found = prop_in_list("unit",unit,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("unit",unit,nb_user,def_user_prop);
            if (found == false)
                unit = UnitNotSpec;
		}
	}
	else if (strlen(conf.unit) ==  0)
    {

		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("unit",unit,nb_user,def_user_prop);
        if (found == false)
            unit = UnitNotSpec;
    }
	else
	{
		// set property
		unit = conf.unit;
	}

	delete_startup_exception("unit");

//
// The standard unit
//

	if(TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		standard_unit = StdUnitNotSpec;
	}
	else if(TG_strcasecmp(conf.standard_unit,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		bool found = prop_in_list("standard_unit",standard_unit,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("standard_unit",standard_unit,nb_user,def_user_prop);
            if (found == false)
                standard_unit = StdUnitNotSpec;
		}
	}
	else if (strlen(conf.standard_unit) == 0)
	{

		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("standard_unit",standard_unit,nb_user,def_user_prop);
        if (found == false)
            standard_unit = StdUnitNotSpec;
	}
	else
	{
		// set property
		standard_unit = conf.standard_unit;
	}

	delete_startup_exception("standard_unit");

//
// The display unit
//

	if(TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.display_unit,DispUnitNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		display_unit = DispUnitNotSpec;
	}
	else if(TG_strcasecmp(conf.display_unit,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		bool found = prop_in_list("display_unit",display_unit,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("display_unit",display_unit,nb_user,def_user_prop);
            if (found == false)
                display_unit = DispUnitNotSpec;
		}
	}
	else if (strlen(conf.display_unit) == 0)
	{

		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("display_unit",display_unit,nb_user,def_user_prop);
        if (found == false)
            display_unit = DispUnitNotSpec;
	}
	else
	{
		// set property
		display_unit = conf.display_unit;
	}

	delete_startup_exception("display_unit");

//
// The format
//

	bool format_not_spec = is_format_notspec(conf.format);

	if(TG_strcasecmp(conf.format,AlrmValueNotSpec) == 0 || (format_not_spec == true))
	{
		// force library defaults (even if user defaults defined)
		set_format_notspec();
	}
	else if(TG_strcasecmp(conf.format,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		bool found = prop_in_list("format",format,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("format",format,nb_user,def_user_prop);
            if (found == false)
                set_format_notspec();
		}
	}
	else if (strlen(conf.format) == 0)
    {
		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("format",format,nb_user,def_user_prop);
        if (found == false)
            set_format_notspec();
	}
	else
	{
		// set property
		format = conf.format;
	}

	delete_startup_exception("format");

//
// For the last four properties, convert their value to the right type.
// First min_value
//

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	string min_value_usr_def;
	double min_value_usr_def_db;
	string min_value_class_def;
	double min_value_class_def_db;
	bool usr_defaults = false;
	bool class_defaults = false;

	if(TG_strcasecmp(conf.min_value,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		min_value_str = AlrmValueNotSpec;
	}
	else if(TG_strcasecmp(conf.min_value,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		class_defaults = prop_in_list("min_value",min_value_class_def,nb_class,def_class_prop);
		if (class_defaults == false)
		{
            usr_defaults = prop_in_list("min_value",min_value_usr_def,nb_user,def_user_prop);
            if (usr_defaults == false)
                min_value_str = AlrmValueNotSpec;
            else
            {
				str.str("");
				str.clear();
				str << min_value_usr_def;
				str >> min_value_usr_def_db;
                min_value_str = min_value_usr_def;
			}
		}
		else
		{
			str.str("");
			str.clear();
			str << min_value_class_def;
			str >> min_value_class_def_db;
            min_value_str = min_value_class_def;
		}
	}
	else if (strlen(conf.min_value) == 0)
	{
		// set user default value if defined, otherwise use the library defaults

        usr_defaults = prop_in_list("min_value",min_value_usr_def,nb_user,def_user_prop);
        if (usr_defaults == false)
            min_value_str = AlrmValueNotSpec;
        else
        {
			str.str("");
			str.clear();
			str << min_value_usr_def;
			str >> min_value_usr_def_db;
            min_value_str = min_value_usr_def;
		}
	}
	else
	{
		// set property
		min_value_str = conf.min_value;
	}

	if (min_value_str == AlrmValueNotSpec)
		check_min_value = false;
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			convert_prop_value("min_value",min_value_str,min_value,dev_name);
			check_min_value = true;

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new  min_value is not above the already
// memorized value
//

            Tango::AttrWriteType w_type = att.get_writable();
            if ((w_type == Tango::READ_WRITE) || (w_type == Tango::WRITE))
            {
                WAttribute *w_att = static_cast<WAttribute *>(this);
                string mem_value;
                if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MIN,mem_value) == true))
                    throw_min_max_value(dev_name,mem_value,MIN);
            }
		}
		else
			throw_err_data_type("min_value",dev_name,"Attribute::set_properties()");
	}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

	if(class_defaults && min_value_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << min_value_str;
		if(str >> db && str.eof() && db == min_value_class_def_db)
			min_value_str = min_value_class_def;
	}
	else if(usr_defaults && min_value_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << min_value_str;
		if(str >> db && str.eof() && db == min_value_usr_def_db)
			min_value_str = min_value_usr_def;
	}

	delete_startup_exception("min_value");

//
// Max value case
//

	string max_value_usr_def;
	double max_value_usr_def_db;
	string max_value_class_def;
	double max_value_class_def_db;
	usr_defaults = false;
	class_defaults = false;

	if(TG_strcasecmp(conf.max_value,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		max_value_str = AlrmValueNotSpec;
	}
	else if(TG_strcasecmp(conf.max_value,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		class_defaults = prop_in_list("max_value",max_value_class_def,nb_class,def_class_prop);
		if (class_defaults == false)
		{
            usr_defaults = prop_in_list("max_value",max_value_usr_def,nb_user,def_user_prop);
            if (usr_defaults == false)
                max_value_str = AlrmValueNotSpec;
            else
            {
				str.str("");
				str.clear();
				str << max_value_usr_def;
				str >> max_value_usr_def_db;
                max_value_str = max_value_usr_def;
			}
		}
		else
		{
			str.str("");
			str.clear();
			str << max_value_class_def;
			str >> max_value_class_def_db;
            max_value_str = max_value_class_def;
		}
	}
	else if (strlen(conf.max_value) == 0)
	{
		// set user default value if defined, otherwise use the library defaults

        usr_defaults = prop_in_list("max_value",max_value_usr_def,nb_user,def_user_prop);
        if (usr_defaults == false)
            max_value_str = AlrmValueNotSpec;
        else
        {
			str.str("");
			str.clear();
			str << max_value_usr_def;
			str >> max_value_usr_def_db;
            max_value_str = max_value_usr_def;
		}
	}
	else
	{
		// set property
		max_value_str = conf.max_value;
	}


	if (max_value_str == AlrmValueNotSpec)
		check_max_value = false;
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			convert_prop_value("max_value",max_value_str,max_value,dev_name);
			check_max_value = true;

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new max_value is not above the already
// memorized value
//

            Tango::AttrWriteType w_type = att.get_writable();
            if ((w_type == Tango::READ_WRITE) || (w_type == Tango::WRITE))
            {
                WAttribute *w_att = static_cast<WAttribute *>(this);
                string mem_value;
                if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MAX,mem_value) == true))
                    throw_min_max_value(dev_name,mem_value,MAX);
            }
		}
		else
			throw_err_data_type("max_value",dev_name,"Attribute::set_properties()");
	}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

	if(class_defaults && max_value_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << max_value_str;
		if(str >> db && str.eof() && db == max_value_class_def_db)
			max_value_str = max_value_class_def;
	}
	else if(usr_defaults && max_value_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << max_value_str;
		if(str >> db && str.eof() && db == max_value_usr_def_db)
			max_value_str = max_value_usr_def;
	}

	delete_startup_exception("max_value");

//
// Min alarm case
//

	string min_alarm_usr_def;
	double min_alarm_usr_def_db;
	string min_alarm_class_def;
	double min_alarm_class_def_db;
	usr_defaults = false;
	class_defaults = false;

	if(TG_strcasecmp(conf.min_alarm,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		min_alarm_str = AlrmValueNotSpec;
	}
	else if(TG_strcasecmp(conf.min_alarm,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		class_defaults = prop_in_list("min_alarm",min_alarm_class_def,nb_class,def_class_prop);
		if (class_defaults == false)
		{
            usr_defaults = prop_in_list("min_alarm",min_alarm_usr_def,nb_user,def_user_prop);
            if (usr_defaults == false)
                min_alarm_str = AlrmValueNotSpec;
            else
            {
				str.str("");
				str.clear();
				str << min_alarm_usr_def;
				str >> min_alarm_usr_def_db;
                min_alarm_str = min_alarm_usr_def;
			}
		}
		else
		{
			str.str("");
			str.clear();
			str << min_alarm_class_def;
			str >> min_alarm_class_def_db;
            min_alarm_str = min_alarm_class_def;
		}
	}
	else if (strlen(conf.min_alarm) == 0)
	{
		// set user default value if defined, otherwise use the library defaults

        usr_defaults = prop_in_list("min_alarm",min_alarm_usr_def,nb_user,def_user_prop);
        if (usr_defaults == false)
            min_alarm_str = AlrmValueNotSpec;
        else
        {
			str.str("");
			str.clear();
			str << min_alarm_usr_def;
			str >> min_alarm_usr_def_db;
            min_alarm_str = min_alarm_usr_def;
		}
	}
	else
	{
		// set property
		min_alarm_str = conf.min_alarm;
	}


	if (min_alarm_str == AlrmValueNotSpec)
		alarm_conf.reset(min_level);
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			convert_prop_value("min_alarm",min_alarm_str,min_alarm,dev_name);
			alarm_conf.set(min_level);

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new min_alarm is not above the already
// memorized value
//

            Tango::AttrWriteType w_type = att.get_writable();
            if ((w_type == Tango::READ_WRITE) || (w_type == Tango::WRITE))
            {
                WAttribute *w_att = static_cast<WAttribute *>(this);
                string mem_value;
                if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MIN,mem_value) == true))
                    throw_min_max_value(dev_name,mem_value,MIN);
            }
		}
		else
			throw_err_data_type("min_alarm",dev_name,"Attribute::set_properties()");
	}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

	if(class_defaults && min_alarm_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << min_alarm_str;
		if(str >> db && str.eof() && db == min_alarm_class_def_db)
			min_alarm_str = min_alarm_class_def;
	}
	else if(usr_defaults && min_alarm_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << min_alarm_str;
		if(str >> db && str.eof() && db == min_alarm_usr_def_db)
			min_alarm_str = min_alarm_usr_def;
	}

	delete_startup_exception("min_alarm");

//
// Max alarm case
//

	string max_alarm_usr_def;
	double max_alarm_usr_def_db;
	string max_alarm_class_def;
	double max_alarm_class_def_db;
	usr_defaults = false;
	class_defaults = false;

	if(TG_strcasecmp(conf.max_alarm,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		max_alarm_str = AlrmValueNotSpec;
	}
	else if(TG_strcasecmp(conf.max_alarm,NotANumber) == 0)
	{
		// set class default if defined, user default value if defined, otherwise use the library defaults

		class_defaults = prop_in_list("max_alarm",max_alarm_class_def,nb_class,def_class_prop);
		if (class_defaults == false)
		{
            usr_defaults = prop_in_list("max_alarm",max_alarm_usr_def,nb_user,def_user_prop);
            if (usr_defaults == false)
                max_alarm_str = AlrmValueNotSpec;
            else
            {
				str.str("");
				str.clear();
				str << max_alarm_usr_def;
				str >> max_alarm_usr_def_db;
                max_alarm_str = max_alarm_usr_def;
			}
		}
		else
		{
			str.str("");
			str.clear();
			str << max_alarm_class_def;
			str >> max_alarm_class_def_db;
            max_alarm_str = max_alarm_class_def;
		}
	}
	else if (strlen(conf.max_alarm) == 0)
	{
		// set user default value if defined, otherwise use the library defaults

        usr_defaults = prop_in_list("max_alarm",max_alarm_usr_def,nb_user,def_user_prop);
        if (usr_defaults == false)
            max_alarm_str = AlrmValueNotSpec;
        else
        {
			str.str("");
			str.clear();
			str << max_alarm_usr_def;
			str >> max_alarm_usr_def_db;
            max_alarm_str = max_alarm_usr_def;
		}
	}
	else
	{
		// set property
		max_alarm_str = conf.max_alarm;
	}


	if (max_alarm_str == AlrmValueNotSpec)
		alarm_conf.reset(max_level);
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			convert_prop_value("max_alarm",max_alarm_str,max_alarm,dev_name);
			alarm_conf.set(max_level);

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new max_alarm is not above the already
// memorized value
//

            Tango::AttrWriteType w_type = att.get_writable();
            if ((w_type == Tango::READ_WRITE) || (w_type == Tango::WRITE))
            {
                WAttribute *w_att = static_cast<WAttribute *>(this);
                string mem_value;
                if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MIN,mem_value) == true))
                    throw_min_max_value(dev_name,mem_value,MIN);
            }
		}
		else
			throw_err_data_type("max_alarm",dev_name,"Attribute::set_properties()");
	}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

	if(class_defaults && max_alarm_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << max_alarm_str;
		if(str >> db && str.eof() && db == max_alarm_class_def_db)
			max_alarm_str = max_alarm_class_def;
	}
	else if(usr_defaults && max_alarm_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << max_alarm_str;
		if(str >> db && str.eof() && db == max_alarm_usr_def_db)
			max_alarm_str = max_alarm_usr_def;
	}

	delete_startup_exception("max_alarm");

}


void Attribute::set_properties(const Tango::AttributeConfig_3 &conf)
{
	set_properties(conf,d_name);
}

void Attribute::set_properties(const Tango::AttributeConfig_3 &conf,Tango::DeviceImpl *d)
{
	if (dev == NULL)
		dev = d;

	set_properties(conf,d->get_name());
}

void Attribute::set_properties(const Tango::AttributeConfig_3 &conf,string &dev_name)
{

//
// First, do all the job done by old set_property(ies)
//

	AttributeConfig tmp_conf;

	tmp_conf.name = conf.name;
	tmp_conf.data_type = conf.data_type;
	tmp_conf.data_format = conf.data_format;
	tmp_conf.writable = conf.writable;
	tmp_conf.writable_attr_name = conf.writable_attr_name;
	tmp_conf.max_dim_x = conf.max_dim_x;
	tmp_conf.max_dim_y = conf.max_dim_y;

	tmp_conf.description = conf.description;
	tmp_conf.label = conf.label;
	tmp_conf.unit = conf.unit;
	tmp_conf.standard_unit = conf.standard_unit;
	tmp_conf.display_unit = conf.display_unit;
	tmp_conf.format = conf.format;
	tmp_conf.min_value = conf.min_value;
	tmp_conf.max_value = conf.max_value;
	tmp_conf.min_alarm = conf.att_alarm.min_alarm;
	tmp_conf.max_alarm = conf.att_alarm.max_alarm;

	set_properties(tmp_conf,dev_name);

//
// Add a check of the display level property because it is not checked by the check_hard_coded_properties() template
// method called by the set_properties() method.
// Display level is available only in AttributeConfig_3
//

	if (is_fwd_att() == true)
		set_hard_coded_properties(conf);
	else
		check_hard_coded_properties(conf);

	if (conf.level != get_disp_level())
	{
		throw_hard_coded_prop("level");
	}

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string or NotANumber mean return to its default value which could be the
// library default value or the user defined default value if present. AlrmValueNotSpec brings the library
// default values regardless of the user defined defaults.
//

	vector<AttrProperty> fake_attr_prop;

	Tango::DeviceClass *dev_class = get_att_device_class(dev_name);
	Tango::Attr *att_ptr;
	bool state_or_status = false;

	if (name_lower == "state" || name_lower == "status")
		state_or_status = true;

	if (state_or_status == false)
	{
		Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
		att_ptr = &(mca->get_attr(name));
	}

	vector<AttrProperty> &def_user_prop = state_or_status == false ? att_ptr->get_user_default_properties() : fake_attr_prop;
	size_t nb_user = def_user_prop.size();

	vector<AttrProperty> &def_class_prop = state_or_status == false ? att_ptr->get_class_properties() : fake_attr_prop;
	size_t nb_class = def_class_prop.size();

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	if (state_or_status == false)
	{

//
// The min_warning case
//

		string min_warning_usr_def;
		double min_warning_usr_def_db;
		string min_warning_class_def;
		double min_warning_class_def_db;
		bool usr_defaults = false;
		bool class_defaults = false;

		if(TG_strcasecmp(conf.att_alarm.min_warning,AlrmValueNotSpec) == 0)
		{
			// force library defaults (even if user defaults defined)
			min_warning_str = AlrmValueNotSpec;
		}
		else if(TG_strcasecmp(conf.att_alarm.min_warning,NotANumber) == 0)
		{
			// set class default if defined, user default value if defined, otherwise use the library defaults

			class_defaults = prop_in_list("min_warning",min_warning_class_def,nb_class,def_class_prop);
			if (class_defaults == false)
			{
				usr_defaults = prop_in_list("min_warning",min_warning_usr_def,nb_user,def_user_prop);
				if (usr_defaults == false)
					min_warning_str = AlrmValueNotSpec;
				else
				{
					str.str("");
					str.clear();
					str << min_warning_usr_def;
					str >> min_warning_usr_def_db;
					min_warning_str = min_warning_usr_def;
				}
			}
			else
			{
				str.str("");
				str.clear();
				str << min_warning_class_def;
				str >> min_warning_class_def_db;
				min_warning_str = min_warning_class_def;
			}
		}
		else if (strlen(conf.att_alarm.min_warning) == 0)
		{
			// set user default value if defined, otherwise use the library defaults

			usr_defaults = prop_in_list("min_warning",min_warning_usr_def,nb_user,def_user_prop);
			if (usr_defaults == false)
				min_warning_str = AlrmValueNotSpec;
			else
			{
				str.str("");
				str.clear();
				str << min_warning_usr_def;
				str >> min_warning_usr_def_db;
				min_warning_str = min_warning_usr_def;
			}
		}
		else
		{
			// set property
			min_warning_str = conf.att_alarm.min_warning;
		}


		if (min_warning_str == AlrmValueNotSpec)
			alarm_conf.reset(min_warn);
		else
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				convert_prop_value("min_warning",min_warning_str,min_warning,dev_name);
				alarm_conf.set(min_warn);
			}
			else
				throw_err_data_type("min_warning",dev_name,"Attribute::set_properties()");
		}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

		if(class_defaults && min_warning_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << min_warning_str;
			if(str >> db && str.eof() && db == min_warning_class_def_db)
				min_warning_str = min_warning_class_def;
		}
		else if(usr_defaults && min_warning_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << min_warning_str;
			if(str >> db && str.eof() && db == min_warning_usr_def_db)
				min_warning_str = min_warning_usr_def;
		}

		delete_startup_exception("min_warning");

//
// Now, ladies and gentleman, the max warning case
//

		string max_warning_usr_def;
		double max_warning_usr_def_db;
		string max_warning_class_def;
		double max_warning_class_def_db;
		usr_defaults = false;
		class_defaults = false;

		if(TG_strcasecmp(conf.att_alarm.max_warning,AlrmValueNotSpec) == 0)
		{
			// force library defaults (even if user defaults defined)
			max_warning_str = AlrmValueNotSpec;
		}
		else if(TG_strcasecmp(conf.att_alarm.max_warning,NotANumber) == 0)
		{
			// set class default if defined, user default value if defined, otherwise use the library defaults

			class_defaults = prop_in_list("max_warning",max_warning_class_def,nb_class,def_class_prop);
			if (class_defaults == false)
			{
				usr_defaults = prop_in_list("max_warning",max_warning_usr_def,nb_user,def_user_prop);
				if (usr_defaults == false)
					max_warning_str = AlrmValueNotSpec;
				else
				{
					str.str("");
					str.clear();
					str << max_warning_usr_def;
					str >> max_warning_usr_def_db;
					max_warning_str = max_warning_usr_def;
				}
			}
			else
			{
				str.str("");
				str.clear();
				str << max_warning_class_def;
				str >> max_warning_class_def_db;
				max_warning_str = max_warning_class_def;
			}
		}
		else if (strlen(conf.att_alarm.max_warning) == 0)
		{
			// set user default value if defined, otherwise use the library defaults

			usr_defaults = prop_in_list("max_warning",max_warning_usr_def,nb_user,def_user_prop);
			if (usr_defaults == false)
				max_warning_str = AlrmValueNotSpec;
			else
			{
				str.str("");
				str.clear();
				str << max_warning_usr_def;
				str >> max_warning_usr_def_db;
				max_warning_str = max_warning_usr_def;
			}
		}
		else
		{
			// set property
			max_warning_str = conf.att_alarm.max_warning;
		}


		if (max_warning_str == AlrmValueNotSpec)
			alarm_conf.reset(max_warn);
		else
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				convert_prop_value("max_warning_value",max_warning_str,max_warning,dev_name);
				alarm_conf.set(max_warn);
			}
			else
				throw_err_data_type("max_warning",dev_name,"Attribute::set_properties()");
		}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

		if(class_defaults && max_warning_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << max_warning_str;
			if(str >> db && str.eof() && db == max_warning_class_def_db)
				max_warning_str = max_warning_class_def;
		}
		else if(usr_defaults && max_warning_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << max_warning_str;
			if(str >> db && str.eof() && db == max_warning_usr_def_db)
				max_warning_str = max_warning_usr_def;
		}

		delete_startup_exception("max_warning");

//
// Then, the delta_val
//

		bool delta_val_defined = false;
		string delta_val_usr_def;
		double delta_val_usr_def_db;
		string delta_val_class_def;
		double delta_val_class_def_db;
		usr_defaults = false;
		class_defaults = false;

		if(TG_strcasecmp(conf.att_alarm.delta_val,AlrmValueNotSpec) == 0)
		{
			// force library defaults (even if user defaults defined)
			delta_val_str = AlrmValueNotSpec;
		}
		else if(TG_strcasecmp(conf.att_alarm.delta_val,NotANumber) == 0)
		{
			// set class default if defined, user default value if defined, otherwise use the library defaults

			class_defaults = prop_in_list("delta_val",delta_val_class_def,nb_class,def_class_prop);
			if (class_defaults == false)
			{
				usr_defaults = prop_in_list("delta_val",delta_val_usr_def,nb_user,def_user_prop);
				if (usr_defaults == false)
					delta_val_str = AlrmValueNotSpec;
				else
				{
					str.str("");
					str.clear();
					str << delta_val_usr_def;
					str >> delta_val_usr_def_db;
					delta_val_str = delta_val_usr_def;
				}
			}
			else
			{
				str.str("");
				str.clear();
				str << delta_val_class_def;
				str >> delta_val_class_def_db;
				delta_val_str = delta_val_class_def;
			}
		}
		else if (strlen(conf.att_alarm.delta_val) == 0)
		{
			// set user default value if defined, otherwise use the library defaults

			usr_defaults = prop_in_list("delta_val",delta_val_usr_def,nb_user,def_user_prop);
			if (usr_defaults == false)
				delta_val_str = AlrmValueNotSpec;
			else
			{
				str.str("");
				str.clear();
				str << delta_val_usr_def;
				str >> delta_val_usr_def_db;
				delta_val_str = delta_val_usr_def;
			}
		}
		else
		{
			// set property
			delta_val_str = conf.att_alarm.delta_val;
		}


		if (delta_val_str != AlrmValueNotSpec)
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				convert_prop_value("delta_val",delta_val_str,delta_val,dev_name);
				delta_val_defined = true;
			}
			else
				throw_err_data_type("delta_val",dev_name,"Attribute::set_properties()");
		}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

		if(class_defaults && delta_val_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << delta_val_str;
			if(str >> db && str.eof() && db == delta_val_class_def_db)
				delta_val_str = delta_val_class_def;
		}
		else if(usr_defaults && delta_val_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << delta_val_str;
			if(str >> db && str.eof() && db == delta_val_usr_def_db)
				delta_val_str = delta_val_usr_def;
		}

		delete_startup_exception("delta_val");

//
// And the delta_t
//

		bool delta_t_defined = false;
		string delta_t_usr_def;
		double delta_t_usr_def_db = 0.0;
		string delta_t_class_def;
		double delta_t_class_def_db = 0.0;
		usr_defaults = false;
		class_defaults = false;
		if(TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0 ||
				TG_strcasecmp(conf.att_alarm.delta_t,"0") == 0 ||
				TG_strcasecmp(conf.att_alarm.delta_t,"0.0") == 0)
		{
			// force library defaults (even if user defaults defined)
			delta_t_str = "0";
		}
		else if (TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0)
		{

		// set class default if defined, user default value if defined, otherwise use the library defaults

			class_defaults = prop_in_list("delta_t",delta_t_class_def,nb_class,def_class_prop);
			if (class_defaults == false)
			{
				usr_defaults = prop_in_list("delta_t",delta_t_usr_def,nb_user,def_user_prop);
				if (usr_defaults == false)
					delta_t_str = "0";
				else
				{
					str.str("");
					str.clear();
					str << delta_t_usr_def;
					str >> delta_t_usr_def_db;
					delta_t_str = delta_t_usr_def;
				}
			}
			else
			{
				str.str("");
				str.clear();
				str << delta_t_class_def;
				str >> delta_t_class_def_db;
				delta_t_str = delta_t_class_def;
			}
		}
		else if (strlen(conf.att_alarm.delta_t) == 0)
		{
			// set user default value if defined, otherwise use the library defaults

			usr_defaults = prop_in_list("delta_t",delta_t_usr_def,nb_user,def_user_prop);
			if (usr_defaults == false)
				delta_t_str = "0";
			else
			{
				str.str("");
				str.clear();
				str << delta_t_usr_def;
				str >> delta_t_usr_def_db;
				delta_t_str = delta_t_usr_def;
			}
		}
		else
		{
			// set property
			delta_t_str = conf.att_alarm.delta_t;
		}

		if(delta_t_str != "0")
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				str.str("");
				str.clear();

				str << delta_t_str;
				double db;
				if (!(str >> db && str.eof()))
					throw_err_format("delta_t",dev_name,"Attribute::set_properties");
				delta_t = (long)db;
				str.str("");
				str.clear();
				str << delta_t;
				delta_t_str = str.str();
				delta_t_defined = true;
			}
			else
				throw_err_data_type("delta_t",dev_name,"Attribute::set_properties");
		}
		else
			delta_t = 0;

		if(class_defaults && delta_t_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << delta_t_str;
			if(str >> db && str.eof() && db == delta_t_class_def_db)
				delta_t_str = delta_t_class_def;
		}
		else if(usr_defaults && delta_t_str != AlrmValueNotSpec)
		{
			double db;
			str.str("");
			str.clear();
			str << delta_t_str;
			if(str >> db && str.eof() && db == delta_t_usr_def_db)
				delta_t_str = delta_t_usr_def;
		}

		delete_startup_exception("delta_t");

//
// Set RDS alarm flag only if both delta_t and delta_val are defined
//

		if(delta_t_defined && delta_val_defined)
		{
			alarm_conf.set(rds);
			delete_startup_exception("rds_alarm");
		}
		else if(delta_t_defined || delta_val_defined)
		{
			alarm_conf.reset(rds);

//
// Set device if not already done
//

			try
			{
				if (dev == NULL)
				{
					// TODO: check how to make cerr quiet
					cerr.setstate(ios::failbit);
					Tango::Util *tg = Tango::Util::instance();
					dev = tg->get_device_by_name(d_name);
					cerr.clear();
				}

				if (dev->get_logger()->is_warn_enabled())
						dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) incoherent in attribute " << name << " (only " << (delta_t_defined ? "delta_t" : "delta_val") << " is set) " << endl;
			}
			catch(...)
			{
				cerr.clear();
			}
		}
		else
		{
			alarm_conf.reset(rds);
			delete_startup_exception("rds_alarm");
		}

//
// Now, the 4 changes parameters (for change and archive events)
//

//
// Relative change
//

		string rel_change_str(conf.event_prop.ch_event.rel_change); // provided, comma separated min and/or max values for the property
		string rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
		string rel_change_class_str; // class default, comma separated min and/or max values for the property, if defined
		vector<double> rel_change_tmp; // vector containing min and max values of the property
		vector<double> rel_change_usr; // vector containing user default min and max values of the property
		vector<double> rel_change_class; // vector containing class default min and max values of the property
		bool rel_change_usr_def; // true if there are user defaults defined for the property
		bool rel_change_class_def; // true if there are class defaults defined for the property

		vector<bool> rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
		vector<bool> rel_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined


		rel_change_usr_def = prop_in_list("rel_change",rel_change_usr_str,nb_user,def_user_prop);

		rel_change_class_def = prop_in_list("rel_change",rel_change_class_str,nb_class,def_class_prop);

//
// Validate user or class default properties
//

		if(rel_change_usr_def)
			validate_change_properties(dev_name,"rel_change",rel_change_usr_str,rel_change_usr);

		if(rel_change_class_def)
			validate_change_properties(dev_name,"rel_change",rel_change_class_str,rel_change_class);

//
// Validate user provided values
//

		validate_change_properties(dev_name,"rel_change",rel_change_str,rel_change_tmp,rel_change_set_usr_def,rel_change_set_class_def);

//
// Set values
//

		if (rel_change_class_def)
		{
			if (rel_change_set_class_def[0] == true)
				rel_change[0] = rel_change_class[0];
			else if (rel_change_usr_def)
			{
				if (rel_change_set_usr_def[0] == true)
					rel_change[0] = rel_change_usr[0];
				else
					rel_change[0] = rel_change_tmp[0];
			}
			else
				rel_change[0] = rel_change_tmp[0];

			if (rel_change_set_class_def[1] == true)
				rel_change[1] = rel_change_class[1];
			else if (rel_change_usr_def)
			{
				if (rel_change_set_usr_def[1] == true)
					rel_change[1] = rel_change_usr[1];
				else
					rel_change[1] = rel_change_tmp[1];
			}
			else
				rel_change[1] = rel_change_tmp[1];
		}
		else if(rel_change_usr_def)
		{
			rel_change[0] = (rel_change_set_class_def[0]) ? rel_change_usr[0] : ((rel_change_set_usr_def[0]) ? rel_change_usr[0] : rel_change_tmp[0]);
			rel_change[1] = (rel_change_set_class_def[1]) ? rel_change_usr[1] : ((rel_change_set_usr_def[1]) ? rel_change_usr[1] : rel_change_tmp[1]);
		}
		else
		{
			rel_change[0] = rel_change_tmp[0];
			rel_change[1] = rel_change_tmp[1];
		}

		delete_startup_exception("rel_change");

//
// Absolute change
//

		string abs_change_str(conf.event_prop.ch_event.abs_change); // provided, comma separated min and/or max values for the property
		string abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
		string abs_change_class_str; // class default, comma separated min and/or max values for the property, if defined
		vector<double> abs_change_tmp; // vector containing min and max values of the property
		vector<double> abs_change_usr; // vector containing user default min and max values of the property
		vector<double> abs_change_class; // vector containing class default min and max values of the property
		bool abs_change_usr_def; // true if there are user defaults defined for the property
		bool abs_change_class_def; // true if there are class defaults defined for the property

		vector<bool> abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
		vector<bool> abs_change_set_class_def; // vector indicating if to use provided values for the property or the user defaults if defined

		abs_change_usr_def = prop_in_list("abs_change",abs_change_usr_str,nb_user,def_user_prop);

		abs_change_class_def = prop_in_list("abs_change",abs_change_class_str,nb_class,def_class_prop);

//
// Validate user or classs default properties
//

		if(abs_change_usr_def)
			validate_change_properties(dev_name,"abs_change",abs_change_usr_str,abs_change_usr);

		if(abs_change_class_def)
			validate_change_properties(dev_name,"abs_change",abs_change_class_str,abs_change_class);

//
// Validate user provided values
//

		validate_change_properties(dev_name,"abs_change",abs_change_str,abs_change_tmp,abs_change_set_usr_def,abs_change_set_class_def);

//
// Set values
//

		if (abs_change_class_def)
		{
			if (abs_change_set_class_def[0] == true)
				abs_change[0] = abs_change_class[0];
			else if (abs_change_usr_def)
			{
				if (abs_change_set_usr_def[0] == true)
					abs_change[0] = abs_change_usr[0];
				else
					abs_change[0] = abs_change_tmp[0];
			}
			else
				abs_change[0] = abs_change_tmp[0];

			if (abs_change_set_class_def[1] == true)
				abs_change[1] = abs_change_class[1];
			else if (abs_change_usr_def)
			{
				if (abs_change_set_usr_def[1] == true)
					abs_change[1] = abs_change_usr[1];
				else
					abs_change[1] = abs_change_tmp[1];
			}
			else
				abs_change[1] = abs_change_tmp[1];
		}
		else if(abs_change_usr_def)
		{
			abs_change[0] = (abs_change_set_class_def[0]) ? abs_change_usr[0] : ((abs_change_set_usr_def[0]) ? abs_change_usr[0] : abs_change_tmp[0]);
			abs_change[1] = (abs_change_set_class_def[1]) ? abs_change_usr[1] : ((abs_change_set_usr_def[1]) ? abs_change_usr[1] : abs_change_tmp[1]);
		}
		else
		{
			abs_change[0] = abs_change_tmp[0];
			abs_change[1] = abs_change_tmp[1];
		}

		delete_startup_exception("abs_change");

//
// Archive relative change
//

		string archive_rel_change_str(conf.event_prop.arch_event.rel_change); // provided, comma separated min and/or max values for the property
		string archive_rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
		string archive_rel_change_class_str; // class default, comma separated min and/or max values for the property, if defined
		vector<double> archive_rel_change_tmp; // vector containing min and max values of the property
		vector<double> archive_rel_change_usr; // vector containing user default min and max values of the property
		vector<double> archive_rel_change_class; // vector containing class default min and max values of the property
		bool archive_rel_change_usr_def; // true if there are user defaults defined for the property
		bool archive_rel_change_class_def; // true if there are class defaults defined for the property

		vector<bool> archive_rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
		vector<bool> archive_rel_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

		archive_rel_change_usr_def = prop_in_list("archive_rel_change",archive_rel_change_usr_str,nb_user,def_user_prop);

		archive_rel_change_class_def = prop_in_list("archive_rel_change",archive_rel_change_class_str,nb_class,def_class_prop);

//
// Validate user or classs default properties
//

		if(archive_rel_change_usr_def)
			validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_usr_str,archive_rel_change_usr);

		if(abs_change_class_def)
			validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_class_str,archive_rel_change_class);

//
// Validate user provided values
//

		validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_str,archive_rel_change_tmp,archive_rel_change_set_usr_def,archive_rel_change_set_class_def);

//
// Set values
//

		if (archive_rel_change_class_def)
		{
			if (archive_rel_change_set_class_def[0] == true)
				archive_rel_change[0] = archive_rel_change_class[0];
			else if (archive_rel_change_usr_def)
			{
				if (archive_rel_change_set_usr_def[0] == true)
					archive_rel_change[0] = archive_rel_change_usr[0];
				else
					archive_rel_change[0] = archive_rel_change_tmp[0];
			}
			else
				archive_rel_change[0] = archive_rel_change_tmp[0];

			if (archive_rel_change_set_class_def[1] == true)
				archive_rel_change[1] = archive_rel_change_class[1];
			else if (archive_rel_change_usr_def)
			{
				if (archive_rel_change_set_usr_def[1] == true)
					archive_rel_change[1] = archive_rel_change_usr[1];
				else
					archive_rel_change[1] = archive_rel_change_tmp[1];
			}
			else
				archive_rel_change[1] = archive_rel_change_tmp[1];
		}
		else if(archive_rel_change_usr_def)
		{
			archive_rel_change[0] = (archive_rel_change_set_class_def[0]) ? archive_rel_change_usr[0] : ((archive_rel_change_set_usr_def[0]) ? archive_rel_change_usr[0] : archive_rel_change_tmp[0]);
			archive_rel_change[1] = (archive_rel_change_set_class_def[1]) ? archive_rel_change_usr[1] : ((archive_rel_change_set_usr_def[1]) ? archive_rel_change_usr[1] : archive_rel_change_tmp[1]);
		}
		else
		{
			archive_rel_change[0] = archive_rel_change_tmp[0];
			archive_rel_change[1] = archive_rel_change_tmp[1];
		}

		delete_startup_exception("archive_rel_change");

//
// Archive absolute change
//

		string archive_abs_change_str(conf.event_prop.arch_event.abs_change); // provided, comma separated min and/or max values for the property
		string archive_abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
		string archive_abs_change_class_str; // class default, comma separated min and/or max values for the property, if defined
		vector<double> archive_abs_change_tmp; // vector containing min and max values of the property
		vector<double> archive_abs_change_usr; // vector containing user default min and max values of the property
		vector<double> archive_abs_change_class; // vector containing class default min and max values of the property
		bool archive_abs_change_usr_def; // true if there are user defaults defined for the property
		bool archive_abs_change_class_def; // true if there are class defaults defined for the property

		vector<bool> archive_abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
		vector<bool> archive_abs_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

		archive_abs_change_usr_def = prop_in_list("archive_abs_change",archive_abs_change_usr_str,nb_user,def_user_prop);

		archive_abs_change_class_def = prop_in_list("archive_abs_change",archive_abs_change_class_str,nb_class,def_class_prop);

//
// Validate user or classs default properties
//

		if(archive_abs_change_usr_def)
			validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_usr_str,archive_abs_change_usr);

		if(abs_change_class_def)
			validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_class_str,archive_abs_change_class);

//
// Validate user provided values
//

		validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_str,archive_abs_change_tmp,archive_abs_change_set_usr_def,archive_abs_change_set_class_def);

//
// Set values
//

		if (archive_abs_change_class_def)
		{
			if (archive_abs_change_set_class_def[0] == true)
				archive_abs_change[0] = archive_abs_change_class[0];
			else if (archive_abs_change_usr_def)
			{
				if (archive_abs_change_set_usr_def[0] == true)
					archive_abs_change[0] = archive_abs_change_usr[0];
				else
					archive_abs_change[0] = archive_abs_change_tmp[0];
			}
			else
				archive_abs_change[0] = archive_abs_change_tmp[0];

			if (archive_abs_change_set_class_def[1] == true)
				archive_abs_change[1] = archive_abs_change_class[1];
			else if (archive_abs_change_usr_def)
			{
				if (archive_abs_change_set_usr_def[1] == true)
					archive_abs_change[1] = archive_abs_change_usr[1];
				else
					archive_abs_change[1] = archive_abs_change_tmp[1];
			}
			else
				archive_abs_change[1] = archive_abs_change_tmp[1];
		}
		else if(archive_abs_change_usr_def)
		{
			archive_abs_change[0] = (archive_abs_change_set_class_def[0]) ? archive_abs_change_usr[0] : ((archive_abs_change_set_usr_def[0]) ? archive_abs_change_usr[0] : archive_abs_change_tmp[0]);
			archive_abs_change[1] = (archive_abs_change_set_class_def[1]) ? archive_abs_change_usr[1] : ((archive_abs_change_set_usr_def[1]) ? archive_abs_change_usr[1] : archive_abs_change_tmp[1]);
		}
		else
		{
			archive_abs_change[0] = archive_abs_change_tmp[0];
			archive_abs_change[1] = archive_abs_change_tmp[1];
		}

		delete_startup_exception("archive_abs_change");
	}

//
// And finally, the last two event periods
//

//
// Event period
//

	TangoSys_MemStream def_event_period;
	string tmp_event_period;
	def_event_period << (int)(DEFAULT_EVENT_PERIOD);
	if(TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(conf.event_prop.per_event.period,def_event_period.str().c_str()) == 0)
	{
		// force library defaults (even if user defaults defined)
		event_period = DEFAULT_EVENT_PERIOD;
	}
	else if (TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0)
	{
		bool found = prop_in_list("event_period",tmp_event_period,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("event_period",tmp_event_period,nb_user,def_user_prop);
            if (found == false)
                event_period = DEFAULT_EVENT_PERIOD;
            else
            {
                str.str("");
                str.clear();
                str << tmp_event_period;
                double db;
                if (!(str >> db && str.eof()))
                    throw_err_format("event_period",dev_name,"Attribute::set_properties()");
                event_period = (int)db;
            }
		}
		else
		{
            str.str("");
            str.clear();
            str << tmp_event_period;
            double db;
            if (!(str >> db && str.eof()))
                throw_err_format("event_period",dev_name,"Attribute::set_properties()");
            event_period = (int)db;
		}
	}
	else if (strlen(conf.event_prop.per_event.period) == 0)
	{
		// set user default value if defined, otherwise use the library defaults
        bool found = prop_in_list("event_period",tmp_event_period,nb_user,def_user_prop);
        if (found == false)
            event_period = DEFAULT_EVENT_PERIOD;
        else
        {
            str.str("");
            str.clear();
            str << tmp_event_period;
            double db;
            if (!(str >> db && str.eof()))
                throw_err_format("event_period",dev_name,"Attribute::set_properties()");
            event_period = (int)db;
        }
	}
	else
	{
		// set property
		str.str("");
		str.clear();
		str << conf.event_prop.per_event.period;
		double db;
		if (!(str >> db && str.eof()))
			throw_err_format("event_period",dev_name,"Attribute::set_properties()");
		event_period = (int)db;
	}

	delete_startup_exception("event_period");

//
// Archive event period
//

	TangoSys_MemStream def_archive_period;
	def_archive_period << (int)(INT_MAX);
	string tmp_archive_period;
	if(TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(conf.event_prop.arch_event.period,def_archive_period.str().c_str()) == 0)
	{
		// force library defaults (even if user defaults defined)
		archive_period = INT_MAX;
	}
	else if (TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0)
	{
        bool found = prop_in_list("archive_period",tmp_archive_period,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("archive_period",tmp_archive_period,nb_user,def_user_prop);
            if (found == false)
                archive_period = DEFAULT_EVENT_PERIOD;
            else
            {
                str.str("");
                str.clear();
                str << tmp_archive_period;
                double db;
                if (!(str >> db && str.eof()))
                    throw_err_format("archive_period",dev_name,"Attribute::set_properties()");
                archive_period = (int)db;
            }
		}
		else
		{
            str.str("");
            str.clear();
            str << tmp_archive_period;
            double db;
            if (!(str >> db && str.eof()))
                throw_err_format("archive_period",dev_name,"Attribute::set_properties()");
            archive_period = (int)db;
		}
	}
	else if (strlen(conf.event_prop.arch_event.period) == 0)
	{
		// set user default value if defined, otherwise use the library defaults
        bool found = prop_in_list("archive_period",tmp_archive_period,nb_user,def_user_prop);
        if (found == false)
            event_period = DEFAULT_EVENT_PERIOD;
        else
        {
            str.str("");
            str.clear();
            str << tmp_archive_period;
            double db;
            if (!(str >> db && str.eof()))
                throw_err_format("archive_period",dev_name,"Attribute::set_properties()");
            archive_period = (int)db;
        }
	}
	else
	{
		// set property
		str.str("");
		str.clear();
		str << conf.event_prop.arch_event.period;
		double db;
		if (!(str >> db && str.eof()))
			throw_err_format("archive_period",dev_name,"Attribute::set_properties()");
		archive_period = (int)db;
	}

	delete_startup_exception("archive_period");
}


void Attribute::set_upd_properties(const AttributeConfig_3 &conf,string &dev_name)
{

//
// Backup current configuration
//

	AttributeConfig_3 old_conf;
	if (is_fwd_att() == false)
		get_properties_3(old_conf);

//
// Set flags which disable attribute configuration roll back in case there are some device startup exceptions
//

	bool is_startup_exception = check_startup_exceptions;
	if(is_startup_exception == true)
		startup_exceptions_clear = false;

	try
	{

//
// Set properties locally. In case of exception bring the backed-up values
//

		set_properties(conf,dev_name);

//
// Check ranges coherence for min and max value
//

		if(check_min_value && check_max_value)
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				switch (data_type)
				{
				case Tango::DEV_SHORT:
					if(min_value.sh >= max_value.sh)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_LONG:
					if(min_value.lg >= max_value.lg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_LONG64:
					if(min_value.lg64 >= max_value.lg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_DOUBLE:
					if(min_value.db >= max_value.db)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_FLOAT:
					if(min_value.fl >= max_value.fl)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_USHORT:
					if(min_value.ush >= max_value.ush)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_UCHAR:
					if(min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ULONG:
					if(min_value.ulg >= max_value.ulg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ULONG64:
					if(min_value.ulg64 >= max_value.ulg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ENCODED:
					if(min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties()");
					break;
				}
			}
			else
				throw_err_data_type("min_value",dev_name,"Attribute::set_upd_properties()");
		}

//
// Check ranges coherence for min and max alarm
//

		if(alarm_conf.test(min_level) && alarm_conf.test(max_level))
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				switch (data_type)
				{
				case Tango::DEV_SHORT:
					if(min_alarm.sh >= max_alarm.sh)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_LONG:
					if(min_alarm.lg >= max_alarm.lg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_LONG64:
					if(min_alarm.lg64 >= max_alarm.lg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_DOUBLE:
					if(min_alarm.db >= max_alarm.db)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_FLOAT:
					if(min_alarm.fl >= max_alarm.fl)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_USHORT:
					if(min_alarm.ush >= max_alarm.ush)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_UCHAR:
					if(min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ULONG:
					if(min_alarm.ulg >= max_alarm.ulg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ULONG64:
					if(min_alarm.ulg64 >= max_alarm.ulg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ENCODED:
					if(min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties()");
					break;
				}
			}
			else
				throw_err_data_type("min_alarm",dev_name,"Attribute::set_upd_properties()");
		}

//
// Check ranges coherence for min and max warning
//

		if(alarm_conf.test(min_warn) && alarm_conf.test(max_warn))
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				switch (data_type)
				{
				case Tango::DEV_SHORT:
					if(min_warning.sh >= max_warning.sh)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_LONG:
					if(min_warning.lg >= max_warning.lg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_LONG64:
					if(min_warning.lg64 >= max_warning.lg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_DOUBLE:
					if(min_warning.db >= max_warning.db)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_FLOAT:
					if(min_warning.fl >= max_warning.fl)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_USHORT:
					if(min_warning.ush >= max_warning.ush)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_UCHAR:
					if(min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ULONG:
					if(min_warning.ulg >= max_warning.ulg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ULONG64:
					if(min_warning.ulg64 >= max_warning.ulg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;

				case Tango::DEV_ENCODED:
					if(min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties()");
					break;
				}
			}
			else
				throw_err_data_type("min_warning",dev_name,"Attribute::set_upd_properties()");
		}

//
// At this point the attribute configuration is correct. Clear the device startup exceptions flag
//

		startup_exceptions_clear = true;

//
// Update database
//

		try
		{
			upd_database(conf,dev_name);
		}
		catch(DevFailed &)
		{

//
// In case of exception, try to store old properties in the database and inform the user about the error
//

			try
			{
				upd_database(old_conf,dev_name);
			}
			catch(DevFailed &)
			{

//
// If the old values could not be restored, notify the user about possible database corruption
//

				TangoSys_OMemStream o;

				o << "Device " << dev_name << "-> Attribute : " << name;
				o << "\nDatabase error occurred whilst setting attribute properties. The database may be corrupted." << ends;
				Except::throw_exception((const char *)API_CorruptedDatabase,
							  o.str(),
							  (const char *)"Attribute::set_upd_properties()");
			}

			throw;
		}
	}
	catch(DevFailed &)
	{

//
// If there are any device startup exceptions, do not roll back the attribute configuration unless the new
// configuration is correct
//

		if(is_startup_exception == false && startup_exceptions_clear == true && is_fwd_att() == false)
			set_properties(old_conf,dev_name);
		throw;
	}
}

void Attribute::set_upd_properties(const AttributeConfig_5 &conf,string &dev_name)
{

//
// First check that the user does not require to change new hard coded params. defind in IDL 5
//

	check_hard_coded(conf);

//
// Build a temporary conf 3 and set attribute properties
//

	AttributeConfig_3 conf3;

	conf3.name = conf.name;
	conf3.writable = conf.writable;
	conf3.data_format = conf.data_format;
	conf3.data_type = conf.data_type;
	conf3.max_dim_x = conf.max_dim_x;
	conf3.max_dim_y = conf.max_dim_y;
	conf3.description = conf.description;
	conf3.label = conf.label;
	conf3.unit = conf.unit;
	conf3.standard_unit = conf.standard_unit;
	conf3.display_unit = conf.display_unit;
	conf3.format = conf.format;
	conf3.min_value = conf.min_value;
	conf3.max_value = conf.max_value;
	conf3.writable_attr_name = conf.writable_attr_name;
	conf3.level = conf.level;
	conf3.att_alarm = conf.att_alarm;
	conf3.event_prop = conf.event_prop;
	conf3.extensions = conf.extensions;
	conf3.sys_extensions = conf.sys_extensions;

	set_upd_properties(conf3,dev_name);
}


void Attribute::set_min_alarm(char *new_min_alarm_str)
{
	set_min_alarm(string(new_min_alarm_str));
}

void Attribute::set_min_alarm(const char *new_min_alarm_str)
{
	set_min_alarm(string(new_min_alarm_str));
}


void Attribute::set_max_alarm(char *new_max_alarm_str)
{
	set_max_alarm(string(new_max_alarm_str));
}

void Attribute::set_max_alarm(const char *new_max_alarm_str)
{
	set_max_alarm(string(new_max_alarm_str));
}


void Attribute::set_min_warning(char *new_min_warning_str)
{
	set_min_warning(string(new_min_warning_str));
}

void Attribute::set_min_warning(const char *new_min_warning_str)
{
	set_min_warning(string(new_min_warning_str));
}


void Attribute::set_max_warning(char *new_max_warning_str)
{
	set_max_warning(string(new_max_warning_str));
}

void Attribute::set_max_warning(const char *new_max_warning_str)
{
	set_max_warning(string(new_max_warning_str));
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::upd_database
//
// description :
//		Update the tango database with the new attribute values
//
// argument :
//		in :
//			- conf: The new attribute configuration
//			- dev_name: The device name
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::upd_database(const Tango::AttributeConfig &conf,string &dev_name)
{

//
// If the attribute is a forwarded one, do nothing
//

	if (is_fwd_att() == true)
		return;

	cout4 << "Entering upd_database method for attribute " << name << endl;

//
// Build the data sent to database
//

	Tango::DbData db_d;
	Tango::DbData db_del;

	db_d.push_back(DbDatum(name));
	db_del.push_back(DbDatum(name));

	long prop_to_update = 0;
	long prop_to_delete = 0;

//
// Get the vector of user default properties to store the library defaults in the data base in case there are user
// defaults declared for the property
//

	Tango::DeviceClass *dev_class = get_att_device_class(dev_name);
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	vector<AttrProperty> unused;

//
// Check if "string" properties must be updated in db and if needed insert a corresponding DbDatum object in the DbData
// vector
//

	check_str_prop(conf,db_d,prop_to_update,db_del,prop_to_delete,def_user_prop,unused);

//
// For the last four, if the data type is not string, checks that the input strings are really number
//

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	CHECK_PROP(conf.min_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_value",def_user_prop,unused);

	CHECK_PROP(conf.max_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_value",def_user_prop,unused);

	CHECK_PROP(conf.min_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_alarm",def_user_prop,unused);

	CHECK_PROP(conf.max_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_alarm",def_user_prop,unused);

//
// Update db only if needed
//

	if (prop_to_update != 0)
	{
		cout4 << prop_to_update << " properties to update in db" << endl;
		db_d[0] << prop_to_update;

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
				tg->get_database()->put_device_attribute_property(dev_name,db_d);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}

	if (prop_to_delete != 0)
	{
		cout4 << prop_to_delete << " properties to delete in db" << endl;

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
				tg->get_database()->delete_device_attribute_property(dev_name,db_del);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}

	cout4 << "Leaving upd_database method" << endl;
}

//
// TODO: merge under control until this point
//

void Attribute::upd_database(const Tango::AttributeConfig_3 &conf,string &dev_name)
{
//
// If the attribute is a forwarded one, do nothing
//

	if (is_fwd_att() == true)
		return;

	cout4 << "Entering upd_database method for attribute " << name << endl;

//
// Build the data sent to database
//

	Tango::DbData db_d;
	Tango::DbData db_del;

	db_d.push_back(DbDatum(name));
	db_del.push_back(DbDatum(name));

	long prop_to_update = 0;
	long prop_to_delete = 0;

//
// Get the vector of user default properties to store the library defaults in the data base in case there are user
// defaults declared for the property
//

	vector<AttrProperty> fake_attr_prop;

	Tango::DeviceClass *dev_class = get_att_device_class(dev_name);
	Tango::Attr *att_ptr;
	bool state_or_status = false;

	if (name_lower == "state" || name_lower == "status")
		state_or_status = true;

	if (state_or_status == false)
	{
		Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
		att_ptr = &(mca->get_attr(name));
	}

	vector<AttrProperty> &def_user_prop = state_or_status == false ? att_ptr->get_user_default_properties() : fake_attr_prop;
	vector<AttrProperty> &def_class_prop = state_or_status == false ? att_ptr->get_class_properties() : fake_attr_prop;
	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();
	string usr_def_val;

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	if (state_or_status == false)
	{

//
// Check if "string" properties must be updated in db and if needed insert a corresponding DbDatum object in the DbData
// vector
//

		AttributeConfig tmp_conf;
		tmp_conf.description = conf.description;
		tmp_conf.label = conf.label;
		tmp_conf.unit = conf.unit;
		tmp_conf.standard_unit = conf.standard_unit;
		tmp_conf.display_unit = conf.display_unit;
		tmp_conf.format = conf.format;

		check_str_prop(tmp_conf,db_d,prop_to_update,db_del,prop_to_delete,def_user_prop,def_class_prop);

//
// For the last two, if the data type is not string, checks that the input strings are really number
//


		CHECK_PROP(conf.min_value,str,dev_name,db_d,db_del,
				   prop_to_update,prop_to_delete,"min_value",def_user_prop,def_class_prop);

		CHECK_PROP(conf.max_value,str,dev_name,db_d,db_del,
				   prop_to_update,prop_to_delete,"max_value",def_user_prop,def_class_prop);

//
// Check for alarm related data
//

		CHECK_PROP(conf.att_alarm.min_alarm,str,dev_name,db_d,db_del,
				   prop_to_update,prop_to_delete,"min_alarm",def_user_prop,def_class_prop);

		CHECK_PROP(conf.att_alarm.max_alarm,str,dev_name,db_d,db_del,
				   prop_to_update,prop_to_delete,"max_alarm",def_user_prop,def_class_prop);

		CHECK_PROP(conf.att_alarm.min_warning,str,dev_name,db_d,db_del,
				   prop_to_update,prop_to_delete,"min_warning",def_user_prop,def_class_prop);

		CHECK_PROP(conf.att_alarm.max_warning,str,dev_name,db_d,db_del,
				   prop_to_update,prop_to_delete,"max_warning",def_user_prop,def_class_prop);


//
// RDS alarm values (delta_val and delta_t) are stored in or deleted from the database only if both are set or both not
// specified.
//

//
// delta_val
//

		string delta_val_tmp_str = conf.att_alarm.delta_val.in();
		string delta_val_usr_def_val;
		string delta_val_class_def_val;
		bool delta_val_is_number = true;
		bool delta_val_user_defaults;
		bool delta_val_class_defaults;

		delta_val_user_defaults = prop_in_list("delta_val",delta_val_usr_def_val,nb_user,def_user_prop);

		delta_val_class_defaults = prop_in_list("delta_val",delta_val_class_def_val,nb_class,def_class_prop);


		if (delta_val_class_defaults)
		{
			if ((TG_strcasecmp(conf.att_alarm.delta_val,NotANumber) == 0) ||
				(strcmp(conf.att_alarm.delta_val,delta_val_class_def_val.c_str()) == 0))
			{
				delta_val_tmp_str = delta_val_class_def_val;
				delta_val_is_number = false;
			}
			else if (strlen(conf.att_alarm.delta_val) == 0)
			{
				if (delta_val_user_defaults)
				{
					delta_val_tmp_str = delta_val_usr_def_val;
					delta_val_is_number = false;
				}
				else
				{
					delta_val_tmp_str = AlrmValueNotSpec;
					delta_val_is_number = false;
				}
			}
			else if(TG_strcasecmp(conf.att_alarm.delta_val,AlrmValueNotSpec) == 0)
			{
				delta_val_tmp_str = AlrmValueNotSpec;
				delta_val_is_number = false;
			}
		}
		else if(delta_val_user_defaults)
		{
			if ((TG_strcasecmp(conf.att_alarm.delta_val,NotANumber) == 0) ||
					(strcmp(conf.att_alarm.delta_val,delta_val_usr_def_val.c_str()) == 0) ||
					(strlen(conf.att_alarm.delta_val) == 0))
			{
				delta_val_tmp_str = delta_val_usr_def_val;
				delta_val_is_number = false;
			}
			else if(TG_strcasecmp(conf.att_alarm.delta_val,AlrmValueNotSpec) == 0)
			{
				delta_val_tmp_str = AlrmValueNotSpec;
				delta_val_is_number = false;
			}
		}
		else if ((TG_strcasecmp(conf.att_alarm.delta_val,AlrmValueNotSpec) == 0) ||
					(TG_strcasecmp(conf.att_alarm.delta_val,NotANumber) == 0) ||
					(strlen(conf.att_alarm.delta_val) == 0))
		{
			delta_val_tmp_str = AlrmValueNotSpec;
			delta_val_is_number = false;
		}

		if(delta_val_is_number)
		{
			if ((data_type != Tango::DEV_STRING) &&
				(data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				double db;

				str.str("");
				str.clear();
				str << conf.att_alarm.delta_val;
				if (!(str >> db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::upd_database()");
				switch (data_type)
				{
				case Tango::DEV_SHORT:
					str.str("");
					str.clear();
					str << (DevShort)db;
					break;

				case Tango::DEV_LONG:
					str.str("");
					str.clear();
					str << (DevLong)db;
					break;

				case Tango::DEV_LONG64:
					str.str("");
					str.clear();
					str << (DevLong64)db;
					break;

				case Tango::DEV_DOUBLE:
					break;

				case Tango::DEV_FLOAT:
					break;

				case Tango::DEV_USHORT:
					str.str("");
					str.clear();
					(db < 0.0) ? str << (DevUShort)(-db) : str << (DevUShort)db;
					break;

				case Tango::DEV_UCHAR:
					str.str("");
					str.clear();
					(db < 0.0) ? str << (short)((DevUChar)(-db)) : str << (short)((DevUChar)db);
					break;

				case Tango::DEV_ULONG:
					str.str("");
					str.clear();
					(db < 0.0) ? str << (DevULong)(-db) : str << (DevULong)db;
					break;

				case Tango::DEV_ULONG64:
					str.str("");
					str.clear();
					(db < 0.0) ? str << (DevULong64)(-db) : str << (DevULong64)db;
					break;

				case Tango::DEV_ENCODED:
					str.str("");
					str.clear();
					(db < 0.0) ? str << (short)(DevUChar)(-db) : str << (short)(DevUChar)db;
					break;
				}
				if (data_type != Tango::DEV_FLOAT && data_type != Tango::DEV_DOUBLE)
					delta_val_tmp_str = str.str();
			}
			else
				throw_err_data_type("delta_val",dev_name,"Attribute::upd_database()");
		}

//
// delta_t
//

        string delta_t_tmp_str = conf.att_alarm.delta_t.in();
        string delta_t_usr_def_val;
        string delta_t_class_def_val;
        bool delta_t_user_defaults;
        bool delta_t_class_defaults;
        bool delta_t_is_number = true;

        delta_t_user_defaults = prop_in_list("delta_t",delta_t_usr_def_val,nb_user,def_user_prop);

        delta_t_class_defaults = prop_in_list("delta_t",delta_t_class_def_val,nb_class,def_class_prop);

        if (delta_t_class_defaults)
        {
            if ((TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0) ||
                (strcmp(conf.att_alarm.delta_t,delta_val_class_def_val.c_str()) == 0))
            {
                delta_t_tmp_str = delta_t_class_def_val;
                delta_t_is_number = false;
            }
            else if (strlen(conf.att_alarm.delta_t) == 0)
            {
                if (delta_t_user_defaults)
                {
                    delta_t_tmp_str = delta_t_usr_def_val;
                    delta_t_is_number = false;
                }
                else
                {
                    delta_t_tmp_str = "0";
                    delta_t_is_number = false;
                }
            }
            else if(TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0)
            {
                delta_t_tmp_str = "0";
                delta_t_is_number = false;
            }
        }
        else if(delta_t_user_defaults)
        {
            if ((TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0) ||
                    (strcmp(conf.att_alarm.delta_t,delta_t_usr_def_val.c_str()) == 0) ||
                    (strlen(conf.att_alarm.delta_t) == 0))
            {
                delta_t_tmp_str = delta_t_usr_def_val;
                delta_t_is_number = false;
            }
            else if((TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0) ||
                    (TG_strcasecmp(conf.att_alarm.delta_t,"0") == 0) ||
                    (TG_strcasecmp(conf.att_alarm.delta_t,"0.0") == 0))
            {
                delta_t_tmp_str = "0";
                delta_t_is_number = false;
            }
        }
        else if ((TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0) ||
                    (TG_strcasecmp(conf.att_alarm.delta_t,"0") == 0) ||
                    (TG_strcasecmp(conf.att_alarm.delta_t,"0.0") == 0) ||
                    (TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0) ||
                    (strlen(conf.att_alarm.delta_t) == 0))
        {
            delta_t_tmp_str = "0";
            delta_t_is_number = false;
        }

        if(delta_t_is_number)
        {
            if ((data_type != Tango::DEV_STRING) &&
                (data_type != Tango::DEV_BOOLEAN) &&
                (data_type != Tango::DEV_STATE))
            {
                str.str("");
                str.clear();
                str << conf.att_alarm.delta_t;
                double db;
                if (!(str >> db && str.eof()))
                    throw_err_format("delta_t",dev_name,"Attribute::upd_database()");
                str.str("");
                str.clear();
                str << (long)db;
                delta_t_tmp_str = str.str();
            }
            else
                throw_err_data_type("delta_t",dev_name,"Attribute::upd_database()");
        }

//
// Check if to store both delta_val and delta_t in database, delete or do nothing
//

        bool rds_store_both = false;
        bool rds_delete_both = false;

        if(delta_val_class_defaults || delta_t_class_defaults)
        {
            if((TG_strcasecmp(delta_val_tmp_str.c_str(),delta_val_class_def_val.c_str()) == 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),delta_t_class_def_val.c_str()) == 0))
                rds_delete_both = true;
            else if(((TG_strcasecmp(delta_val_tmp_str.c_str(),AlrmValueNotSpec) == 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),"0") == 0)) ||
                        ((TG_strcasecmp(delta_val_tmp_str.c_str(),AlrmValueNotSpec) != 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),"0") != 0)))
                rds_store_both = true;
        }
        else if(delta_val_user_defaults || delta_t_user_defaults)
        {
            if((TG_strcasecmp(delta_val_tmp_str.c_str(),delta_val_usr_def_val.c_str()) == 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),delta_t_usr_def_val.c_str()) == 0))
                rds_delete_both = true;
            else if(((TG_strcasecmp(delta_val_tmp_str.c_str(),AlrmValueNotSpec) == 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),"0") == 0)) ||
                        ((TG_strcasecmp(delta_val_tmp_str.c_str(),AlrmValueNotSpec) != 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),"0") != 0)))
                rds_store_both = true;
        }
        else if((TG_strcasecmp(delta_val_tmp_str.c_str(),AlrmValueNotSpec) != 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),"0") != 0))
            rds_store_both = true;
        else if((TG_strcasecmp(delta_val_tmp_str.c_str(),AlrmValueNotSpec) == 0) && (TG_strcasecmp(delta_t_tmp_str.c_str(),"0") == 0))
            rds_delete_both = true;

		if(rds_store_both)
		{
			DbDatum dd("delta_val");
			dd << delta_val_tmp_str.c_str();
			db_d.push_back(dd);
			prop_to_update++;

			DbDatum deltat("delta_t");
			deltat << delta_t_tmp_str.c_str();
			db_d.push_back(deltat);
			prop_to_update++;
		}

		if(rds_delete_both)
		{
			DbDatum del_dd("delta_val");
			db_del.push_back(del_dd);
			prop_to_delete++;

			DbDatum del_deltat("delta_t");
			db_del.push_back(del_deltat);
			prop_to_delete++;
		}

//
// Check for event related data
//

//
// Relative change
//

        string rel_change_str(conf.event_prop.ch_event.rel_change); // provided, comma separated min and/or max values for the property
        string rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
        string rel_change_class_str; // user default, comma separated min and/or max values for the property, if defined
        vector<double> rel_change_tmp; // vector containing min and max values of the property
        vector<double> rel_change_usr; // vector containing user default min and max values of the property
        vector<double> rel_change_class; // vector containing class default min and max values of the property
        bool rel_change_usr_def; // true if there are user defaults defined for the property
        bool rel_change_class_def; // true if there are class defaults defined for the property

        vector<bool> rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
        vector<bool> rel_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

        rel_change_usr_def = prop_in_list("rel_change",rel_change_usr_str,nb_user,def_user_prop);

        rel_change_class_def = prop_in_list("rel_change",rel_change_class_str,nb_class,def_class_prop);

//
// Validate user or classs default properties
//

        if(rel_change_usr_def)
            validate_change_properties(dev_name,"rel_change",rel_change_usr_str,rel_change_usr);

        if(rel_change_class_def)
            validate_change_properties(dev_name,"rel_change",rel_change_class_str,rel_change_class);

//
// Validate user provided values
//

        validate_change_properties(dev_name,"rel_change",rel_change_str,rel_change_tmp,rel_change_set_usr_def,rel_change_set_class_def);

//
// Overwrite rel_change values according to user request to return to class/user defaults
//

        if (rel_change_class_def)
        {
            if (rel_change_set_class_def[0] == true)
            	rel_change_tmp[0] = rel_change_class[0];
            else if (rel_change_usr_def)
            {
                if (rel_change_set_usr_def[0] == true)
                	rel_change_tmp[0] = rel_change_usr[0];
            }

            if (rel_change_set_class_def[1] == true)
            	rel_change_tmp[1] = rel_change_class[1];
            else if (rel_change_usr_def)
            {
                if (rel_change_set_usr_def[1] == true)
                	rel_change_tmp[1] = rel_change_usr[1];
            }
        }
        else if(rel_change_usr_def)
        {
        	rel_change_tmp[0] = (rel_change_set_class_def[0]) ? rel_change_usr[0] : ((rel_change_set_usr_def[0]) ? rel_change_usr[0] : rel_change_tmp[0]);
        	rel_change_tmp[1] = (rel_change_set_class_def[1]) ? rel_change_usr[1] : ((rel_change_set_usr_def[1]) ? rel_change_usr[1] : rel_change_tmp[1]);
        }

        rel_change_tmp[0] = fabs(rel_change_tmp[0]);
        rel_change_tmp[1] = fabs(rel_change_tmp[1]);

//
// Manage db
//

        if (rel_change_tmp[0] == rel_change_tmp[1])
        {
            if (rel_change_tmp[0] == INT_MAX)
            {
                if (rel_change_usr_def == true || rel_change_class_def == true)
                {
                    DbDatum desc("rel_change");
                    desc << rel_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("rel_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (rel_change_usr_def == true && rel_change_tmp[0] == fabs(rel_change_usr[0]))
            {
                if (rel_change_class_def == true)
                {
                    DbDatum desc("rel_change");
                    desc << rel_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("rel_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (rel_change_class_def == true && rel_change_tmp[0] == fabs(rel_change_class[0]))
            {
                DbDatum del_desc("rel_change");
                db_del.push_back(del_desc);
                prop_to_delete++;
            }
            else
            {
                DbDatum desc("rel_change");
                desc << rel_change_tmp[0];
                db_d.push_back(desc);
                prop_to_update++;
            }
        }
        else
        {
            if (rel_change_class_def)
            {
                event_prop_db("rel_change",rel_change_tmp,rel_change_class,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else if(rel_change_usr_def)
            {
                event_prop_db("rel_change",rel_change_tmp,rel_change_usr,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else
            {
                DbDatum relchange("rel_change");
                relchange << rel_change_tmp;
                db_d.push_back(relchange);
                prop_to_update++;
            }
        }

//
// Absolute change
//

        string abs_change_str(conf.event_prop.ch_event.abs_change); // provided, comma separated min and/or max values for the property
        string abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
        string abs_change_class_str; // class default, comma separated min and/or max values for the property, if defined
        vector<double> abs_change_tmp; // vector containing min and max values of the property
        vector<double> abs_change_usr; // vector containing user default min and max values of the property
        vector<double> abs_change_class; // vector containing class default min and max values of the property
        bool abs_change_usr_def; // true if there are user defaults defined for the property
        bool abs_change_class_def; // true if there are user defaults defined for the property

        vector<bool> abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
        vector<bool> abs_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

        abs_change_usr_def = prop_in_list("abs_change",abs_change_usr_str,nb_user,def_user_prop);

        abs_change_class_def = prop_in_list("abs_change",abs_change_class_str,nb_class,def_class_prop);

//
// Validate user or class default properties
//

        if(abs_change_usr_def)
            validate_change_properties(dev_name,"abs_change",abs_change_usr_str,abs_change_usr);

        if(abs_change_class_def)
            validate_change_properties(dev_name,"abs_change",abs_change_class_str,abs_change_class);

//
// Validate user provided values
//

        validate_change_properties(dev_name,"abs_change",abs_change_str,abs_change_tmp,abs_change_set_usr_def,abs_change_set_class_def);

//
// Overwrite rel_change values according to user request to return to class/user defaults
//

        if (abs_change_class_def)
        {
            if (abs_change_set_class_def[0] == true)
                abs_change_tmp[0] = abs_change_class[0];
            else if (abs_change_usr_def)
            {
                if (abs_change_set_usr_def[0] == true)
                    abs_change_tmp[0] = abs_change_usr[0];
            }

            if (abs_change_set_class_def[1] == true)
                abs_change_tmp[1] = abs_change_class[1];
            else if (abs_change_usr_def)
            {
                if (abs_change_set_usr_def[1] == true)
                    abs_change_tmp[1] = abs_change_usr[1];
            }
        }
        else if(abs_change_usr_def)
        {
            abs_change_tmp[0] = (abs_change_set_class_def[0]) ? abs_change_usr[0] : ((abs_change_set_usr_def[0]) ? abs_change_usr[0] : abs_change_tmp[0]);
            abs_change_tmp[1] = (abs_change_set_class_def[1]) ? abs_change_usr[1] : ((abs_change_set_usr_def[1]) ? abs_change_usr[1] : abs_change_tmp[1]);
        }

        abs_change_tmp[0] = fabs(abs_change_tmp[0]);
        abs_change_tmp[1] = fabs(abs_change_tmp[1]);

//
// Manage db
//

        if (abs_change_tmp[0] == abs_change_tmp[1])
        {
            if (abs_change_tmp[0] == INT_MAX)
            {
                if (abs_change_usr_def == true || abs_change_class_def == true)
                {
                    DbDatum desc("abs_change");
                    desc << abs_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("abs_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (abs_change_usr_def == true && abs_change_tmp[0] == fabs(abs_change_usr[0]))
            {
                if (abs_change_class_def == true)
                {
                    DbDatum desc("abs_change");
                    desc << abs_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("abs_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (abs_change_class_def == true && abs_change_tmp[0] == fabs(abs_change_class[0]))
            {
                DbDatum del_desc("abs_change");
                db_del.push_back(del_desc);
                prop_to_delete++;
            }
            else
            {
                DbDatum desc("abs_change");
                desc << abs_change_tmp[0];
                db_d.push_back(desc);
                prop_to_update++;
            }
        }
        else
        {
            if (abs_change_class_def)
            {
                event_prop_db("abs_change",abs_change_tmp,abs_change_class,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else if(abs_change_usr_def)
            {
                event_prop_db("abs_change",abs_change_tmp,abs_change_usr,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else
            {
                DbDatum abschange("abs_change");
                abschange << abs_change_tmp;
                db_d.push_back(abschange);
                prop_to_update++;
            }
        }

//
// Archive relative change
//

        string archive_rel_change_str(conf.event_prop.arch_event.rel_change); // provided, comma separated min and/or max values for the property
        string archive_rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
        string archive_rel_change_class_str; // class default, comma separated min and/or max values for the property, if defined
        vector<double> archive_rel_change_tmp; // vector containing min and max values of the property
        vector<double> archive_rel_change_usr; // vector containing user default min and max values of the property
        vector<double> archive_rel_change_class; // vector containing class default min and max values of the property
        bool archive_rel_change_usr_def; // true if there are user defaults defined for the property
        bool archive_rel_change_class_def; // true if there are class defaults defined for the property

        vector<bool> archive_rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
        vector<bool> archive_rel_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

        archive_rel_change_usr_def = prop_in_list("archive_rel_change",archive_rel_change_usr_str,nb_user,def_user_prop);

        archive_rel_change_class_def = prop_in_list("archive_rel_change",archive_rel_change_class_str,nb_class,def_class_prop);

//
// Validate user or class default properties
//

        if(archive_rel_change_usr_def)
            validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_usr_str,archive_rel_change_usr);

        if(archive_rel_change_class_def)
            validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_class_str,archive_rel_change_class);

//
// Validate user provided values
//

        validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_str,archive_rel_change_tmp,archive_rel_change_set_usr_def,archive_rel_change_set_class_def);

//
// Overwrite archive_rel_change values according to user request to return to class/user defaults
//

        if (archive_rel_change_class_def)
        {
            if (archive_rel_change_set_class_def[0] == true)
                archive_rel_change_tmp[0] = archive_rel_change_class[0];
            else if (archive_rel_change_usr_def)
            {
                if (archive_rel_change_set_usr_def[0] == true)
                	archive_rel_change_tmp[0] = archive_rel_change_usr[0];
            }

            if (archive_rel_change_set_class_def[1] == true)
            	archive_rel_change_tmp[1] = archive_rel_change_class[1];
            else if (archive_rel_change_usr_def)
            {
                if (archive_rel_change_set_usr_def[1] == true)
                	archive_rel_change_tmp[1] = archive_rel_change_usr[1];
            }
        }
        else if(archive_rel_change_usr_def)
        {
        	archive_rel_change_tmp[0] = (archive_rel_change_set_class_def[0]) ? archive_rel_change_usr[0] : ((archive_rel_change_set_usr_def[0]) ? archive_rel_change_usr[0] : archive_rel_change_tmp[0]);
        	archive_rel_change_tmp[1] = (archive_rel_change_set_class_def[1]) ? archive_rel_change_usr[1] : ((archive_rel_change_set_usr_def[1]) ? archive_rel_change_usr[1] : archive_rel_change_tmp[1]);
        }

        archive_rel_change_tmp[0] = fabs(archive_rel_change_tmp[0]);
        archive_rel_change_tmp[1] = fabs(archive_rel_change_tmp[1]);

//
// Manage db
//

        if(archive_rel_change_tmp[0] == archive_rel_change_tmp[1])
        {
            if (archive_rel_change_tmp[0] == INT_MAX)
            {
                if (archive_rel_change_usr_def == true || archive_rel_change_class_def == true)
                {
                    DbDatum desc("archive_rel_change");
                    desc << archive_rel_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("archive_rel_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (archive_rel_change_usr_def == true && archive_rel_change_tmp[0] == fabs(archive_rel_change_usr[0]))
            {
                if (archive_rel_change_class_def == true)
                {
                    DbDatum desc("archive_rel_change");
                    desc << archive_rel_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("archive_rel_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (archive_rel_change_class_def == true && archive_rel_change_tmp[0] == fabs(archive_rel_change_class[0]))
            {
                DbDatum del_desc("archive_rel_change");
                db_del.push_back(del_desc);
                prop_to_delete++;
            }
            else
            {
                DbDatum desc("archive_rel_change");
                desc << archive_rel_change_tmp[0];
                db_d.push_back(desc);
                prop_to_update++;
            }
        }
        else
        {
            if (archive_rel_change_class_def)
            {
                event_prop_db("archive_rel_change",archive_rel_change_tmp,archive_rel_change_class,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else if(archive_rel_change_usr_def)
            {
                event_prop_db("archive_rel_change",archive_rel_change_tmp,archive_rel_change_usr,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else
            {
                DbDatum archiverelchange("archive_rel_change");
                archiverelchange << archive_rel_change_tmp;
                db_d.push_back(archiverelchange);
                prop_to_update++;
            }
        }

//
// Archive absolute change
//

        string archive_abs_change_str(conf.event_prop.arch_event.abs_change); // provided, comma separated min and/or max values for the property
        string archive_abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
        string archive_abs_change_class_str; // class default, comma separated min and/or max values for the property, if defined
        vector<double> archive_abs_change_tmp; // vector containing min and max values of the property
        vector<double> archive_abs_change_usr; // vector containing user default min and max values of the property
        vector<double> archive_abs_change_class; // vector containing class default min and max values of the property
        bool archive_abs_change_usr_def; // true if there are user defaults defined for the property
        bool archive_abs_change_class_def; // true if there are class defaults defined for the property

        vector<bool> archive_abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
        vector<bool> archive_abs_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

        archive_abs_change_usr_def = prop_in_list("archive_abs_change",archive_abs_change_usr_str,nb_user,def_user_prop);

        archive_abs_change_class_def = prop_in_list("archive_abs_change",archive_abs_change_class_str,nb_class,def_class_prop);

//
// Validate user or class default properties
//

        if(archive_abs_change_usr_def)
            validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_usr_str,archive_abs_change_usr);

        if(archive_abs_change_class_def)
            validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_class_str,archive_abs_change_class);

//
// Validate user provided values
//

        validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_str,archive_abs_change_tmp,archive_abs_change_set_usr_def,archive_abs_change_set_class_def);

//
// Overwrite archive_abs_change values according to user request to return to class/user defaults
//

        if (archive_abs_change_class_def)
        {
            if (archive_abs_change_set_class_def[0] == true)
            	archive_abs_change_tmp[0] = archive_abs_change_class[0];
            else if (archive_abs_change_usr_def)
            {
                if (archive_abs_change_set_usr_def[0] == true)
                	archive_abs_change_tmp[0] = archive_abs_change_usr[0];
            }

            if (archive_abs_change_set_class_def[1] == true)
            	archive_abs_change_tmp[1] = archive_abs_change_class[1];
            else if (archive_abs_change_usr_def)
            {
                if (archive_abs_change_set_usr_def[1] == true)
                	archive_abs_change_tmp[1] = archive_abs_change_usr[1];
            }
        }
        else if(archive_abs_change_usr_def)
        {
        	archive_abs_change_tmp[0] = (archive_abs_change_set_class_def[0]) ? archive_abs_change_usr[0] : ((archive_abs_change_set_usr_def[0]) ? archive_abs_change_usr[0] : archive_abs_change_tmp[0]);
        	archive_abs_change_tmp[1] = (archive_abs_change_set_class_def[1]) ? archive_abs_change_usr[1] : ((archive_abs_change_set_usr_def[1]) ? archive_abs_change_usr[1] : archive_abs_change_tmp[1]);
        }

        archive_abs_change_tmp[0] = fabs(archive_abs_change_tmp[0]);
        archive_abs_change_tmp[1] = fabs(archive_abs_change_tmp[1]);

//
// Manage db
//

        if(archive_abs_change_tmp[0] == archive_abs_change_tmp[1])
        {
            if (archive_abs_change_tmp[0] == INT_MAX)
            {
                if (archive_abs_change_usr_def == true || archive_abs_change_class_def == true)
                {
                    DbDatum desc("archive_abs_change");
                    desc << archive_abs_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("archive_abs_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (archive_abs_change_usr_def == true && archive_abs_change_tmp[0] == fabs(archive_abs_change_usr[0]))
            {
                if (archive_abs_change_class_def == true)
                {
                    DbDatum desc("archive_abs_change");
                    desc << archive_abs_change_tmp[0];
                    db_d.push_back(desc);
                    prop_to_update++;
                }
                else
                {
                    DbDatum del_desc("archive_abs_change");
                    db_del.push_back(del_desc);
                    prop_to_delete++;
                }
            }
            else if (archive_abs_change_class_def == true && archive_abs_change_tmp[0] == fabs(archive_abs_change_class[0]))
            {
                DbDatum del_desc("archive_abs_change");
                db_del.push_back(del_desc);
                prop_to_delete++;
            }
            else
            {
                DbDatum desc("archive_abs_change");
                desc << archive_abs_change_tmp[0];
                db_d.push_back(desc);
                prop_to_update++;
            }
        }
        else
        {
            if (archive_abs_change_class_def)
            {
                event_prop_db("archive_abs_change",archive_abs_change_tmp,archive_abs_change_class,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else if(archive_abs_change_usr_def)
            {
                event_prop_db("archive_abs_change",archive_abs_change_tmp,archive_abs_change_usr,db_del,prop_to_delete,db_d,prop_to_update);
            }
            else
            {
                DbDatum archiveabschange("archive_abs_change");
                archiveabschange << archive_abs_change_tmp;
                db_d.push_back(archiveabschange);
                prop_to_update++;
            }
        }
    }

//
// Event period
//

    string class_def_val;

	TangoSys_MemStream def_event_period_str;
	string def_event_period;
	def_event_period_str << (int)(DEFAULT_EVENT_PERIOD);
	def_event_period_str >> def_event_period;
	bool user_defaults;
	bool class_defaults;
	bool store_in_db = true;


    user_defaults = prop_in_list("event_period",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("event_period",class_def_val,nb_class,def_class_prop);
    if (class_defaults)
    {
        if((TG_strcasecmp(class_def_val.c_str(),AlrmValueNotSpec) == 0) ||
                (TG_strcasecmp(class_def_val.c_str(),NotANumber) == 0) ||
                (strlen(class_def_val.c_str()) == 0))
        {
            TangoSys_MemStream str;
            str << (int)(DEFAULT_EVENT_PERIOD);
            class_def_val = str.str();
        }
    }

    if (class_defaults)
    {
    	bool input_equal_def = false;
		str.str("");
		str.clear();
		str << conf.event_prop.per_event.period.in();
		double db;
		if (str >> db && str.eof())
		{
			str.str("");
			str.clear();
			str << class_def_val;
			int i;
			str >> i;
			if((int)db == i)
				input_equal_def = true;
		}
		if ((TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0) ||
				(strcmp(conf.event_prop.per_event.period,class_def_val.c_str()) == 0) ||
				(input_equal_def == true))
			store_in_db = false;
    }
	else if(user_defaults)
	{
    	bool input_equal_def = false;
		str.str("");
		str.clear();
		str << conf.event_prop.per_event.period.in();
		double db;
		if (str >> db && str.eof())
		{
			str.str("");
			str.clear();
			str << usr_def_val;
			int i;
			str >> i;
			if((int)db == i)
				input_equal_def = true;
		}
		if ((TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0) ||
				(strcmp(conf.event_prop.per_event.period,usr_def_val.c_str()) == 0) ||
				(strlen(conf.event_prop.per_event.period) == 0) ||
				(input_equal_def == true))
			store_in_db = false;
	}
	else
	{
    	bool input_equal_def = false;
		str.str("");
		str.clear();
		str << conf.event_prop.per_event.period.in();
		double db;
		if (str >> db && str.eof())
		{
			if((int)db == (int)(DEFAULT_EVENT_PERIOD))
				input_equal_def = true;
		}
		if ((TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.event_prop.per_event.period,def_event_period.c_str()) == 0) ||
				(TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0) ||
				(strlen(conf.event_prop.per_event.period) == 0) ||
				(input_equal_def == true))
			store_in_db = false;
	}

	if(store_in_db)
	{
		string tmp = conf.event_prop.per_event.period.in();
		if (TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0)
		{
            tmp = def_event_period;
        }
		else if (strlen(conf.event_prop.per_event.period) == 0)
        {
            if (class_defaults && user_defaults)
                tmp = usr_def_val;
            else
                tmp = def_event_period;
        }
		else
		{
			str.str("");
			str.clear();
			str << conf.event_prop.per_event.period.in();
			double db;
			if (!(str >> db && str.eof()))
				throw_err_format("event_period",dev_name,"Attribute::upd_database");
			str.str("");
			str.clear();
			str << (int)db;
			tmp = str.str();
		}
		DbDatum eventperiod("event_period");
		eventperiod << tmp.c_str();
		db_d.push_back(eventperiod);
		prop_to_update++;
	}
	else
	{
		DbDatum del_eventperiod("event_period");
		db_del.push_back(del_eventperiod);
		prop_to_delete++;
	}

//
// Archive event period
//

	TangoSys_MemStream def_arch_event_period_str;
	string def_arch_event_period;
	def_arch_event_period_str << (int)(INT_MAX);
	def_arch_event_period_str >> def_arch_event_period;
	store_in_db = true;


    user_defaults = prop_in_list("archive_period",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("archive_period",class_def_val,nb_class,def_class_prop);
    if (class_defaults)
    {
        if((TG_strcasecmp(class_def_val.c_str(),AlrmValueNotSpec) == 0) ||
                (TG_strcasecmp(class_def_val.c_str(),NotANumber) == 0) ||
                (strlen(class_def_val.c_str()) == 0))
        {
            TangoSys_MemStream str;
            str << (int)(INT_MAX);
            class_def_val = str.str();
        }
    }

    if (class_defaults)
    {
    	bool input_equal_def = false;
		str.str("");
		str.clear();
		str << conf.event_prop.arch_event.period.in();
		double db;
		if (str >> db && str.eof())
		{
			str.str("");
			str.clear();
			str << class_def_val;
			int i;
			str >> i;
			if((int)db == i)
				input_equal_def = true;
		}
		if ((TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0) ||
				(strcmp(conf.event_prop.arch_event.period,class_def_val.c_str()) == 0) ||
				(input_equal_def == true))
			store_in_db = false;
    }
	else if(user_defaults)
	{
    	bool input_equal_def = false;
		str.str("");
		str.clear();
		str << conf.event_prop.arch_event.period.in();
		double db;
		if (str >> db && str.eof())
		{
			str.str("");
			str.clear();
			str << usr_def_val;
			int i;
			str >> i;
			if((int)db == i)
				input_equal_def = true;
		}
		if ((TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0) ||
				(strcmp(conf.event_prop.arch_event.period,usr_def_val.c_str()) == 0) ||
				(strlen(conf.event_prop.arch_event.period) == 0) ||
				(input_equal_def == true))
			store_in_db = false;
	}
	else
	{
    	bool input_equal_def = false;
		str.str("");
		str.clear();
		str << conf.event_prop.arch_event.period.in();
		double db;
		if (str >> db && str.eof())
		{
			if((int)db == (int)(INT_MAX))
				input_equal_def = true;
		}
		if ((TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.event_prop.arch_event.period,def_arch_event_period.c_str()) == 0) ||
				(TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0) ||
				(strlen(conf.event_prop.arch_event.period) == 0) ||
				(input_equal_def == true))
			store_in_db = false;
	}

	if(store_in_db)
	{
		string tmp = conf.event_prop.arch_event.period.in();
		if (TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0)
		{
            tmp = def_arch_event_period;
        }
		else if (strlen(conf.event_prop.arch_event.period) == 0)
        {
            if (class_defaults && user_defaults)
                tmp = usr_def_val;
            else
                tmp = def_arch_event_period;
        }
		else
		{
			str.str("");
			str.clear();
			str << conf.event_prop.arch_event.period;
			double db;
			if (!(str >> db && str.eof()))
				throw_err_format("archive_period",dev_name,"Attribute::upd_database()");
			str.str("");
			str.clear();
			str << (int)db;
			tmp = str.str();
		}
		DbDatum archeventperiod("archive_period");
		archeventperiod << tmp.c_str();
		db_d.push_back(archeventperiod);
		prop_to_update++;
	}
	else
	{
		DbDatum del_archeventperiod("archive_period");
		db_del.push_back(del_archeventperiod);
		prop_to_delete++;
	}

//
// Update db only if needed
//

	if (prop_to_update != 0)
	{
		cout4 << prop_to_update << " properties to update in db" << endl;
		db_d[0] << prop_to_update;

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
				tg->get_database()->put_device_attribute_property(dev_name,db_d);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}

	if (prop_to_delete != 0)
	{
		cout4 << prop_to_delete << " properties to delete in db" << endl;
		db_del[0] << prop_to_delete;

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
				tg->get_database()->delete_device_attribute_property(dev_name,db_del);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}

	cout4 << "Leaving upd_database method" << endl;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::check_str_prop
//
// description :
//		Check if attribute properties which are real string needs to be updated in db
//
// argument :
// 		in :
//			- conf : The attribute configuration structure
//			- db_d : The DbData which will be used to update database
//			- prop_to_update : The number of property(ies) to update in database
//          - db_del : The DbData which will be used to delete database
//          - prop_to_delete :  The number of property(ies) to delete from database
//          - def_user_prop :
//          - def_class_prop :
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::check_str_prop(const Tango::AttributeConfig &conf,
			       Tango::DbData &db_d,long &prop_to_update,
			       Tango::DbData &db_del,long &prop_to_delete,
			       vector<AttrProperty> &def_user_prop,
			       vector<AttrProperty> &def_class_prop)
{

//
// Check if the value of attribute property is to be stored in the database.
// There are 5 value alternatives distinguished: value (Val), empty string (""),
// NotANumber (NaN), AlrmValueNotSpec (AVNS) and default library value (Def).
// There are also 2 use cases, for which the above value alternatives imply
// different behaviour:
// 1) No user default values defined
// 		"", NaN, Def and AVNS - delete database entry for the property
//		Val - store value in database
// 2) User defaul values defined
// 		"", NaN - delete database entry for the property
//		AVNS, Def - store library default value (Def) in the database
//		value - store value in the database
//

	bool user_defaults, class_defaults;
	string usr_def_val, class_def_val;
	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

//
// Description
//

    user_defaults = prop_in_list("description",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("description",class_def_val,nb_class,def_class_prop);

    if (TG_strcasecmp(conf.description,AlrmValueNotSpec) == 0)
    {
        if (user_defaults == true || class_defaults == true)
        {
			DbDatum desc("description");
			desc << DescNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("description");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (strlen(conf.description) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("description");
			if (user_defaults)
                desc << usr_def_val.c_str();
			else
                desc << DescNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("description");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (TG_strcasecmp(conf.description,NotANumber) == 0)
    {
        DbDatum del_desc("description");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (user_defaults == true && strcmp(conf.description,usr_def_val.c_str()) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("description");
			desc << usr_def_val.c_str();
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("description");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (class_defaults == true && strcmp(conf.description,class_def_val.c_str()) == 0)
    {
        DbDatum del_desc("description");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (class_defaults == false && TG_strcasecmp(conf.description,DescNotSpec) == 0)
    {
        DbDatum del_desc("description");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else
    {
        DbDatum desc("description");
        const char *tmp = conf.description.in();
        desc << tmp;
        db_d.push_back(desc);
        prop_to_update++;
    }

//
// Label
//

    user_defaults = prop_in_list("label",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("label",class_def_val,nb_class,def_class_prop);

    if (TG_strcasecmp(conf.label,AlrmValueNotSpec) == 0)
    {
        if (user_defaults == true || class_defaults == true)
        {
			DbDatum desc("label");
			desc << LabelNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("label");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (strlen(conf.label) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("label");
			if (user_defaults)
                desc << usr_def_val.c_str();
			else
                desc << LabelNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("label");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (TG_strcasecmp(conf.label,NotANumber) == 0)
    {
        DbDatum del_desc("label");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (user_defaults == true && strcmp(conf.label,usr_def_val.c_str()) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("label");
			desc << usr_def_val.c_str();
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("label");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (class_defaults == true && strcmp(conf.label,class_def_val.c_str()) == 0)
    {
        DbDatum del_desc("label");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (class_defaults == false && TG_strcasecmp(conf.label,name.c_str()) == 0)
    {
        DbDatum del_desc("label");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else
    {
        DbDatum desc("label");
        const char *tmp = conf.label.in();
        desc << tmp;
        db_d.push_back(desc);
        prop_to_update++;
    }

//
// Unit
//

    user_defaults = prop_in_list("unit",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("unit",class_def_val,nb_class,def_class_prop);

    if (TG_strcasecmp(conf.unit,AlrmValueNotSpec) == 0)
    {
        if (user_defaults == true || class_defaults == true)
        {
			DbDatum desc("unit");
			desc << UnitNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (strlen(conf.unit) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("unit");
			if (user_defaults)
                desc << usr_def_val.c_str();
            else
                desc << UnitNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (TG_strcasecmp(conf.unit,NotANumber) == 0)
    {
        DbDatum del_desc("unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (user_defaults == true && strcmp(conf.unit,usr_def_val.c_str()) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("unit");
			desc << usr_def_val.c_str();
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (class_defaults == true && strcmp(conf.unit,class_def_val.c_str()) == 0)
    {
        DbDatum del_desc("unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (class_defaults == false && TG_strcasecmp(conf.unit,UnitNotSpec) == 0)
    {
        DbDatum del_desc("unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else
    {
        DbDatum desc("unit");
        const char *tmp = conf.unit.in();
        desc << tmp;
        db_d.push_back(desc);
        prop_to_update++;
    }

//
// Standard unit
//

    user_defaults = prop_in_list("standard_unit",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("standard_unit",class_def_val,nb_class,def_class_prop);

    if (TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) == 0)
    {
        if (user_defaults == true || class_defaults == true)
        {
			DbDatum desc("standard_unit");
			desc << StdUnitNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("standard_unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (strlen(conf.standard_unit) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("standard_unit");
			if (user_defaults)
                desc << usr_def_val.c_str();
            else
                desc << StdUnitNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("standard_unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (TG_strcasecmp(conf.standard_unit,NotANumber) == 0)
    {
        DbDatum del_desc("standard_unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (user_defaults == true && strcmp(conf.standard_unit,usr_def_val.c_str()) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("standard_unit");
			desc << usr_def_val.c_str();
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("standard_unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (class_defaults == true && strcmp(conf.standard_unit,class_def_val.c_str()) == 0)
    {
        DbDatum del_desc("standard_unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (class_defaults == false && TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) == 0)
    {
        DbDatum del_desc("standard_unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else
    {
        DbDatum desc("standard_unit");
        const char *tmp = conf.standard_unit.in();
        desc << tmp;
        db_d.push_back(desc);
        prop_to_update++;
    }

//
// Display unit
//

    user_defaults = prop_in_list("display_unit",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("display_unit",class_def_val,nb_class,def_class_prop);

    if (TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) == 0)
    {
        if (user_defaults == true || class_defaults == true)
        {
			DbDatum desc("display_unit");
			desc << DispUnitNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("display_unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (strlen(conf.display_unit) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("display_unit");
			if (user_defaults)
                desc << usr_def_val.c_str();
            else
                desc << DispUnitNotSpec;
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("display_unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (TG_strcasecmp(conf.display_unit,NotANumber) == 0)
    {
        DbDatum del_desc("display_unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (user_defaults == true && strcmp(conf.display_unit,usr_def_val.c_str()) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("display_unit");
			desc << usr_def_val.c_str();
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("display_unit");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (class_defaults == true && strcmp(conf.display_unit,class_def_val.c_str()) == 0)
    {
        DbDatum del_desc("display_unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (class_defaults == false && TG_strcasecmp(conf.display_unit,DispUnitNotSpec) == 0)
    {
        DbDatum del_desc("display_unit");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else
    {
        DbDatum desc("display_unit");
        const char *tmp = conf.display_unit.in();
        desc << tmp;
        db_d.push_back(desc);
        prop_to_update++;
    }

//
// Format
//

    user_defaults = prop_in_list("format",usr_def_val,nb_user,def_user_prop);

    class_defaults = prop_in_list("format",class_def_val,nb_class,def_class_prop);

    if (TG_strcasecmp(conf.format,AlrmValueNotSpec) == 0)
    {
        if (user_defaults == true || class_defaults == true)
        {
			DbDatum desc("format");
			def_format_in_dbdatum(desc);
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("format");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (strlen(conf.format) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("format");
			if (user_defaults)
                desc << usr_def_val.c_str();
            else
                def_format_in_dbdatum(desc);
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("format");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (TG_strcasecmp(conf.format,NotANumber) == 0)
    {
        DbDatum del_desc("format");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (user_defaults == true && strcmp(conf.format,usr_def_val.c_str()) == 0)
    {
        if (class_defaults == true)
        {
			DbDatum desc("format");
			desc << usr_def_val.c_str();
			db_d.push_back(desc);
			prop_to_update++;
        }
        else
        {
			DbDatum del_desc("format");
			db_del.push_back(del_desc);
			prop_to_delete++;
        }
    }
    else if (class_defaults == true && strcmp(conf.format,class_def_val.c_str()) == 0)
    {
        DbDatum del_desc("format");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else if (class_defaults == false && is_format_notspec(conf.format) == true)
    {
        DbDatum del_desc("format");
        db_del.push_back(del_desc);
        prop_to_delete++;
    }
    else
    {
        DbDatum desc("format");
        const char *tmp = conf.format.in();
        desc << tmp;
        db_d.push_back(desc);
        prop_to_update++;
    }
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::validate_change_properties
//
// description :
//		Check if attribute change properties are properly defined
//
// argument :
// 		in :
//			- dev_name : The name of the device
//			- prop_name : The name of the property
//		    - change_prop_str : A string representing the change property (in the form of either "value" or
//								"value1,value2")
//
// 		out :
//			- validated_prop : A vector of parsed change properties values in the form of numbers of type double
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::validate_change_properties(const string &dev_name, const char *prop_name, string &change_prop_str, vector<double> &validated_prop)
{
	vector<bool> bring_usr_def;
	vector<bool> bring_class_def;

	validate_change_properties(dev_name, prop_name, change_prop_str, validated_prop, bring_usr_def,bring_class_def);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::validate_change_properties
//
// description :
//		Check if attribute change properties are properly defined
//
// argument :
// 		in :
//			- dev_name : The name of the device
//			- prop_name : The name of the property
//			- change_prop_str : A string representing the change property (in the form of either "value" or
//								"value1,value2")
//
// 		out :
//			- validated_prop : A vector of parsed change properties values in the form of numbers of type double
//			- bring_usr_def : A vector of boolean values indicating if for a corresponding value in validated_prop
//							  vector there was a request to restore the user defined value
//			- bring_class_def : A vector of boolean values indicating if for a corresponding value in validated_prop
//								vector there was a request to restore the class defined value
//
//------------------------------------------------------------------------------------------------------------------

void Attribute::validate_change_properties(const string &dev_name, const char *prop_name, string &change_prop_str, vector<double> &validated_prop,
                                           vector<bool> &bring_usr_def,vector<bool> &bring_class_def)
{
	// by default, values for event change properties are set to INT_MAX
	validated_prop.clear();
	validated_prop.push_back(INT_MAX);
	validated_prop.push_back(INT_MAX);

	// by default, bring user and class defined properties flags are set to false
	bring_usr_def.clear();
	bring_usr_def.push_back(false);
	bring_usr_def.push_back(false);

	bring_class_def.clear();
	bring_class_def.push_back(false);
	bring_class_def.push_back(false);

	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);
	string prop_min;
	string prop_max;
	bool one_param = true;
	double prop_tmp;

	size_t pos = change_prop_str.find(',');
	if(pos != string::npos)
	{
		prop_min = change_prop_str.substr(0,pos);
		prop_max = change_prop_str.erase(0,pos+1);
		one_param = false;
	}
	else
		prop_min = change_prop_str;

	if(TG_strcasecmp(prop_min.c_str(),AlrmValueNotSpec) == 0)
	{
		validated_prop[0] = INT_MAX;
		validated_prop[1] = INT_MAX;
	}
	else if(TG_strcasecmp(prop_min.c_str(),NotANumber) == 0)
	{
		bring_class_def[0] = true;
		bring_class_def[1] = true;

		bring_usr_def[0] = true;
		bring_usr_def[1] = true;
	}
	else if(prop_min == "")
	{
		bring_usr_def[0] = true;
		bring_usr_def[1] = true;
	}
	else
	{
		str << prop_min;
		if(str >> prop_tmp && str.eof())
		{
			if (fabs(prop_tmp) > 0 && prop_tmp != INT_MAX)
			{
				if ((data_type != Tango::DEV_STRING) &&
				    (data_type != Tango::DEV_BOOLEAN) &&
				    (data_type != Tango::DEV_STATE))
				{
					validated_prop[0] = -fabs(prop_tmp);
					validated_prop[1] = fabs(prop_tmp);
				}
				else
					throw_err_data_type(prop_name,const_cast<string&>(dev_name),"Attribute::validate_change_properties()");
			}
		}
		else
			throw_err_format(prop_name,dev_name,"Attribute::validate_change_properties()");
	}

	if(!one_param)
	{
		validated_prop[1] = INT_MAX;
		bring_usr_def[1] = false;
		bring_class_def[1] = false;

		if(TG_strcasecmp(prop_max.c_str(),AlrmValueNotSpec) == 0)
			validated_prop[1] = INT_MAX;
		else if(TG_strcasecmp(prop_max.c_str(),NotANumber) == 0)
			bring_class_def[1] = true;
		else if(prop_max == "")
			bring_usr_def[1] = true;
		else
		{
			str.str("");
			str.clear();
			str << prop_max;
			if(str >> prop_tmp && str.eof())
			{
				if (fabs(prop_tmp) > 0 && prop_tmp != INT_MAX)
				{
					if ((data_type != Tango::DEV_STRING) &&
					    (data_type != Tango::DEV_BOOLEAN) &&
					    (data_type != Tango::DEV_STATE))
					{
						validated_prop[1] = fabs(prop_tmp);
					}
					else
						throw_err_data_type(prop_name,const_cast<string&>(dev_name),"Attribute::validate_change_properties()");
				}
			}
			else
				throw_err_format(prop_name,dev_name,"Attribute::validate_change_properties()");
		}
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::event_prop_db
//
// description :
//
// argument :
// 		in :
//			- prop_name :
//          - rel_change_tmp :
//			- rel_change_str :
//			- db_del :
//			- prop_to_delete :
//          - db_d :
//          - prop_to_update :
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::event_prop_db(const char *prop_name,vector<double> &rel_change_tmp,vector<double> &rel_change_usr,
							  DbData &db_del,long &prop_to_delete,DbData &db_d,long &prop_to_update)
{
    vector<string> rel_change_str_tmp(2);
    vector<bool> rel_change_usr_def_tmp(2);
    rel_change_usr_def_tmp[0] = rel_change_usr_def_tmp[1] = false;

    if(rel_change_tmp[0] == fabs(rel_change_usr[0]))
    {
        rel_change_str_tmp[0] = NotANumber;
        rel_change_usr_def_tmp[0] = true;
    }
    if(rel_change_tmp[1] == fabs(rel_change_usr[1]))
    {
        rel_change_str_tmp[1] = NotANumber;
        rel_change_usr_def_tmp[1] = true;
    }

    if(rel_change_usr_def_tmp[0] && rel_change_usr_def_tmp[1])
    {
        DbDatum del_relchange(prop_name);
        db_del.push_back(del_relchange);
        prop_to_delete++;
    }
    else if(!rel_change_usr_def_tmp[0] && !rel_change_usr_def_tmp[1])
    {
        DbDatum relchange(prop_name);
        relchange << rel_change_tmp;
        db_d.push_back(relchange);
        prop_to_update++;
    }
    else
    {
        TangoSys_MemStream str;
        str.precision(TANGO_FLOAT_PRECISION);
        if(rel_change_usr_def_tmp[0] && !rel_change_usr_def_tmp[1])
        {
            str << rel_change_tmp[1];
            rel_change_str_tmp[1] = str.str();
        }
        else
        {
            str << rel_change_tmp[0];
            rel_change_str_tmp[0] = str.str();
        }
        DbDatum relchange(prop_name);
        relchange << rel_change_str_tmp;
        db_d.push_back(relchange);
        prop_to_update++;
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::check_hard_coded()
//
// description :
//		Check if the user tries to change attribute properties considered as hard coded added by IDL 5
//      Throw exception in case of
//
// args :
// 		in :
//			- user_conf : The attribute configuration sent by the user
//
//------------------------------------------------------------------------------------------------------------------

void Attribute::check_hard_coded(const AttributeConfig_5 &user_conf)
{

//
// Check root attribute name
//

	if (is_fwd_att() == true)
	{
		FwdAttribute *fwd = static_cast<FwdAttribute *>(this);
		string root_attr_name(fwd->get_fwd_dev_name() + '/' + fwd->get_fwd_att_name());
		string user_root_att_name(user_conf.root_attr_name.in());
		transform(user_root_att_name.begin(),user_root_att_name.end(),user_root_att_name.begin(),::tolower);
		if (user_root_att_name != root_attr_name)
		{
			throw_hard_coded_prop("root_attr_name");
		}
	}

//
// Memorized config
//

	if (writable == WRITE || writable == READ_WRITE)
	{
		WAttribute *watt = static_cast<WAttribute *>(this);
		if (watt->is_memorized() != user_conf.memorized || watt->is_memorized_init() != user_conf.mem_init)
		{
			throw_hard_coded_prop("memorized");
		}
	}

// TODO: enum_labels
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::convert_prop_value()
//
// description :
//
//
// args :
// 		in :
//			- prop_name : The attribute property name
//			- dev_name : The device name
//			- value_str : Attribute property value stored as a string
//		out :
//			- val : Attribute property value stored as a number
//
//------------------------------------------------------------------------------------------------------------------

void Attribute::convert_prop_value(const char *prop_name,string &value_str,Attr_CheckVal &val,const string &dev_name)
{
	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	str << value_str;
	if (!(str >> val.db && str.eof()))
		throw_err_format(prop_name,dev_name,"Attribute::convert_prop_value()");

	switch (data_type)
	{
	case Tango::DEV_SHORT:
		val.sh = (DevShort)val.db;
		str.str("");
		str.clear();
		str << val.sh;
		break;

	case Tango::DEV_LONG:
		val.lg = (DevLong)val.db;
		str.str("");
		str.clear();
		str << val.lg;
		break;

	case Tango::DEV_LONG64:
		val.lg64 = (DevLong64)val.db;
		str.str("");
		str.clear();
		str << val.lg64;
		break;

	case Tango::DEV_DOUBLE:
		break;

	case Tango::DEV_FLOAT:
		val.fl = (DevFloat)val.db;
		break;

	case Tango::DEV_USHORT:
		(val.db < 0.0) ? val.ush = (DevUShort)(-val.db) : val.ush = (DevUShort)val.db;
		str.str("");
		str.clear();
		str << val.ush;
		break;

	case Tango::DEV_UCHAR:
		(val.db < 0.0) ? val.uch = (DevUChar)(-val.db) : val.uch = (DevUChar)val.db;
		str.str("");
		str.clear();
		str << (short)val.uch;
		break;

	case Tango::DEV_ULONG:
		(val.db < 0.0) ? val.ulg = (DevULong)(-val.db) : val.ulg = (DevULong)val.db;
		str.str("");
		str.clear();
		str << val.ulg;
		break;

	case Tango::DEV_ULONG64:
		(val.db < 0.0) ? val.ulg64 = (DevULong64)(-val.db) : val.ulg64 = (DevULong64)val.db;
		str.str("");
		str.clear();
		str << val.ulg64;
		break;

	case Tango::DEV_ENCODED:
		(val.db < 0.0) ? val.uch = (DevUChar)(-val.db) : val.uch = (DevUChar)val.db;
		str.str("");
		str.clear();
		str << (short)val.uch;
		break;
	}
	if (data_type != Tango::DEV_FLOAT && data_type != Tango::DEV_DOUBLE)
        	value_str = str.str();
}

} // End of Tango namespace
