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
// Copyright (C) :      2013,2014,2015
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
#endif /* _TG_WINDOWS_ */

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
	conf.name = Tango::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = Tango::string_dup(label.c_str());
	conf.description = Tango::string_dup(description.c_str());
	conf.unit = Tango::string_dup(unit.c_str());
	conf.standard_unit = Tango::string_dup(standard_unit.c_str());
	conf.display_unit = Tango::string_dup(display_unit.c_str());
	conf.format = Tango::string_dup(format.c_str());
	conf.writable_attr_name = Tango::string_dup(writable_attr_name.c_str());
	conf.min_alarm = Tango::string_dup(min_alarm_str.c_str());
	conf.max_alarm = Tango::string_dup(max_alarm_str.c_str());
	conf.min_value = Tango::string_dup(min_value_str.c_str());
	conf.max_value = Tango::string_dup(max_value_str.c_str());
}

void Attribute::get_properties(Tango::AttributeConfig_2 &conf)
{

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = Tango::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = Tango::string_dup(label.c_str());
	conf.description = Tango::string_dup(description.c_str());
	conf.unit = Tango::string_dup(unit.c_str());
	conf.standard_unit = Tango::string_dup(standard_unit.c_str());
	conf.display_unit = Tango::string_dup(display_unit.c_str());
	conf.format = Tango::string_dup(format.c_str());
	conf.writable_attr_name = Tango::string_dup(writable_attr_name.c_str());
	conf.min_alarm = Tango::string_dup(min_alarm_str.c_str());
	conf.max_alarm = Tango::string_dup(max_alarm_str.c_str());
	conf.min_value = Tango::string_dup(min_value_str.c_str());
	conf.max_value = Tango::string_dup(max_value_str.c_str());

	conf.level = disp_level;
}

void Attribute::get_properties(Tango::AttributeConfig_3 &conf)
{

//
// Throw exception in case of Fwd attribute and the att configuration is not yet received
//

	if (data_type == DATA_TYPE_UNKNOWN)
	{
		std::string desc("Attribute ");
		FwdAttribute *fwd = static_cast<FwdAttribute *>(this);
		desc = desc + get_name() + " is a forwarded attribute and its root device (";
		desc = desc + fwd->get_fwd_dev_name();
		desc = desc + ") is not yet available";
		Tango::Except::throw_exception(API_AttrConfig,desc,"Attribute::get_properties");
	}

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = Tango::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = Tango::string_dup(label.c_str());
	conf.description = Tango::string_dup(description.c_str());
	conf.unit = Tango::string_dup(unit.c_str());
	conf.standard_unit = Tango::string_dup(standard_unit.c_str());
	conf.display_unit = Tango::string_dup(display_unit.c_str());
	conf.format = Tango::string_dup(format.c_str());
	conf.writable_attr_name = Tango::string_dup(writable_attr_name.c_str());
	conf.min_value = Tango::string_dup(min_value_str.c_str());
	conf.max_value = Tango::string_dup(max_value_str.c_str());
	conf.level = disp_level;

//
// Copy alarm properties
//

	conf.att_alarm.min_alarm = Tango::string_dup(min_alarm_str.c_str());
	conf.att_alarm.max_alarm = Tango::string_dup(max_alarm_str.c_str());
	conf.att_alarm.min_warning = Tango::string_dup(min_warning_str.c_str());
	conf.att_alarm.max_warning = Tango::string_dup(max_warning_str.c_str());
	if (delta_t == 0)
		conf.att_alarm.delta_t = Tango::string_dup(AlrmValueNotSpec);
	else
		conf.att_alarm.delta_t = Tango::string_dup(delta_t_str.c_str());
	conf.att_alarm.delta_val = Tango::string_dup(delta_val_str.c_str());

//
// Copy periodic event property
//

	TangoSys_OMemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	if (event_period == INT_MAX)
	{
		str << DEFAULT_EVENT_PERIOD;
	}
	else
	{
		str << event_period;
	}

	MEM_STREAM_2_CORBA(conf.event_prop.per_event.period,str);

//
// Copy change event properties
//

	if (fabs(rel_change[0]) == fabs(rel_change[1]))
	{
		if (rel_change[0] == INT_MAX)
			conf.event_prop.ch_event.rel_change = Tango::string_dup(AlrmValueNotSpec);
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
			conf.event_prop.ch_event.abs_change = Tango::string_dup(AlrmValueNotSpec);
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
		conf.event_prop.arch_event.period = Tango::string_dup(AlrmValueNotSpec);
	else
	{
		int per = (int)((double)archive_period);
		str << per;
		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.period,str);
	}

	if (fabs(archive_rel_change[0]) == fabs(archive_rel_change[1]))
	{
		if (archive_rel_change[0] == INT_MAX)
			conf.event_prop.arch_event.rel_change = Tango::string_dup(AlrmValueNotSpec);
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
			conf.event_prop.arch_event.abs_change = Tango::string_dup(AlrmValueNotSpec);
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

void Attribute::get_properties(Tango::AttributeConfig_5 &conf)
{

//
// Get config
//

	Tango::AttributeConfig_3 conf3;
	get_properties(conf3);

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

	add_config_5_specific(conf);

}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::add_config_5_specific
//
// description :
//		Add into the received structure configuration release 5 specific data (root att, mem info,...)
//
// argument :
// 		out :
//			- conf : The attribute config. structure
//
//--------------------------------------------------------------------------------------------------------------------
void Attribute::add_config_5_specific(AttributeConfig_5 &conf)
{

//
// Root attribute
//

	if (is_fwd_att() == true)
	{
		FwdAttribute *fwd = static_cast<FwdAttribute *>(this);
		std::string str(fwd->get_fwd_dev_name() + '/' + fwd->get_fwd_att_name());
		conf.root_attr_name = Tango::string_dup(str.c_str());
	}
	else
		conf.root_attr_name = Tango::string_dup(AlrmValueNotSpec);

//
// Memorized info
//

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

//
// Enum labels
//

	if (enum_labels.empty() == false)
	{
		conf.enum_labels.length(enum_labels.size());
		for (size_t loop = 0;loop < enum_labels.size();loop++)
			conf.enum_labels[loop] = Tango::string_dup(enum_labels[loop].c_str());
	}

}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_properties
//
// description :
//		Set the attribute properties value for AttributeConfig
//
// argument :
// 		in :
//			- conf : The new properties sent by client
//			- dev_name : The device name
//			- from_ds :
//		out :
//			- v_db : Vector of data used for database update/delete
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::set_properties(const Tango::AttributeConfig &conf,std::string &dev_name,TANGO_UNUSED(bool from_ds),std::vector<AttPropDb> &v_db)
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

	std::vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	std::vector<AttrProperty> &def_class_prop = att.get_class_properties();

//
// First the string properties
//

	set_one_str_prop("description",conf.description,description,v_db,def_user_prop,def_class_prop,DescNotSpec);
	delete_startup_exception("description",dev_name);

	set_one_str_prop("label",conf.label,label,v_db,def_user_prop,def_class_prop,name.c_str());
	delete_startup_exception("label",dev_name);

	set_one_str_prop("unit",conf.unit,unit,v_db,def_user_prop,def_class_prop,UnitNotSpec);
	delete_startup_exception("unit",dev_name);

	set_one_str_prop("standard_unit",conf.standard_unit,standard_unit,v_db,def_user_prop,def_class_prop,StdUnitNotSpec);
	delete_startup_exception("standard_unit",dev_name);

	set_one_str_prop("display_unit",conf.display_unit,display_unit,v_db,def_user_prop,def_class_prop,DispUnitNotSpec);
	delete_startup_exception("display_unit",dev_name);

	set_one_str_prop("format",conf.format,format,v_db,def_user_prop,def_class_prop,FormatNotSpec);
	delete_startup_exception("format",dev_name);

//
// Min, max and most of the alarm related properties
//

    set_one_alarm_prop("min_value",conf.min_value,min_value_str,min_value,v_db,def_user_prop,def_class_prop,check_min_value);
    delete_startup_exception("min_value",dev_name);

    set_one_alarm_prop("max_value",conf.max_value,max_value_str,max_value,v_db,def_user_prop,def_class_prop,check_max_value);
    delete_startup_exception("max_value",dev_name);

    bool alrm_set;
    set_one_alarm_prop("min_alarm",conf.min_alarm,min_alarm_str,min_alarm,v_db,def_user_prop,def_class_prop,alrm_set);
    if (alrm_set == false)
        alarm_conf.reset(min_level);
    else
        alarm_conf.set(min_level);
    delete_startup_exception("min_alarm",dev_name);

    set_one_alarm_prop("max_alarm",conf.max_alarm,max_alarm_str,max_alarm,v_db,def_user_prop,def_class_prop,alrm_set);
    if (alrm_set == false)
        alarm_conf.reset(max_level);
    else
        alarm_conf.set(max_level);
    delete_startup_exception("max_alarm",dev_name);
}


void Attribute::set_properties(const Tango::AttributeConfig_3 &conf,std::string &dev_name,bool from_ds,std::vector<AttPropDb> &v_db)
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

	set_properties(tmp_conf,dev_name,from_ds,v_db);

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
// For Config 3 specific properties
//

	bool state_or_status = false;
	std::vector<AttrProperty> fake_attr_prop;

    if (name_lower == "state" || name_lower == "status")
        state_or_status = true;

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string means returns to its default value which could be the library default
// value or the user defined default value
//

    Tango::DeviceClass *dev_class = get_att_device_class(dev_name);
	Tango::Attr *att_ptr;

	if (state_or_status == false)
	{
		Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
		att_ptr = &(mca->get_attr(name));
	}

	std::vector<AttrProperty> &def_user_prop = state_or_status == false ? att_ptr->get_user_default_properties() : fake_attr_prop;
	std::vector<AttrProperty> &def_class_prop = state_or_status == false ? att_ptr->get_class_properties() : fake_attr_prop;

	if ((state_or_status == false) &&
     	((data_type != Tango::DEV_STRING) &&
		 (data_type != Tango::DEV_BOOLEAN) &&
		 (data_type != Tango::DEV_STATE)))
	{

//
// Alarm related properties
//

		bool alrm_set;

		set_one_alarm_prop("min_warning",conf.att_alarm.min_warning,min_warning_str,min_warning,v_db,def_user_prop,def_class_prop,alrm_set);
		if (alrm_set == false)
			alarm_conf.reset(min_warn);
		else
			alarm_conf.set(min_warn);
		delete_startup_exception("min_warning",dev_name);

		set_one_alarm_prop("max_warning",conf.att_alarm.max_warning,max_warning_str,max_warning,v_db,def_user_prop,def_class_prop,alrm_set);
		if (alrm_set == false)
			alarm_conf.reset(max_warn);
		else
			alarm_conf.set(max_warn);
		delete_startup_exception("max_warning",dev_name);

//
// RDS related properties
//

		set_rds_prop(conf.att_alarm,dev_name,v_db,def_user_prop,def_class_prop);

//
// Event related properties (except period)
//

		set_one_event_prop("rel_change",conf.event_prop.ch_event.rel_change,rel_change,v_db,def_user_prop,def_class_prop);
		delete_startup_exception("rel_change",dev_name);

		set_one_event_prop("abs_change",conf.event_prop.ch_event.abs_change,abs_change,v_db,def_user_prop,def_class_prop);
		delete_startup_exception("rel_change",dev_name);

		set_one_event_prop("archive_rel_change",conf.event_prop.arch_event.rel_change,archive_rel_change,v_db,def_user_prop,def_class_prop);
		delete_startup_exception("archive_rel_change",dev_name);

		set_one_event_prop("archive_abs_change",conf.event_prop.arch_event.abs_change,archive_abs_change,v_db,def_user_prop,def_class_prop);
		delete_startup_exception("archive_abs_change",dev_name);
	}

//
// Event periods
//

	set_one_event_period("event_period",conf.event_prop.per_event.period,event_period,DEFAULT_EVENT_PERIOD,v_db,def_user_prop,def_class_prop);
	delete_startup_exception("event_period",dev_name);

	set_one_event_period("archive_period",conf.event_prop.arch_event.period,archive_period,INT_MAX,v_db,def_user_prop,def_class_prop);
	delete_startup_exception("archive_period",dev_name);
}


void Attribute::set_properties(const Tango::AttributeConfig_5 &conf,std::string &dev_name,bool from_ds,std::vector<AttPropDb> &v_db)
{
//
// Check that the request is not to change some unmutable properties added in IDL 5
//

	check_hard_coded(conf);

//
// Do all the job done by old set_property(ies)
//

	AttributeConfig_3 tmp_conf;

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
	tmp_conf.level = conf.level;

	tmp_conf.att_alarm = conf.att_alarm;
	tmp_conf.event_prop = conf.event_prop;

	set_properties(tmp_conf,dev_name,from_ds,v_db);

//
// Now some IDL 5 specific properties
//

	set_prop_5_specific(conf,dev_name,from_ds,v_db);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_one_str_prop
//
// description :
//		Analyse one of the string properties. String properties are description, label, unit, standard_unit,
//		display_unit and format
//
// argument :
// 		in :
//			- prop_name : The property name
//			- conf_val : The new property value
//			- def_user_prop : The set of user defined default values
//			- def_class_prop : The set of class defined default values
//			- lib_def : The property library default value
//		out :
//			- att_conf : The new property in Attribute object
//			- v_db : Vector of data used for database update/delete
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::set_one_str_prop(const char *prop_name,const CORBA::String_member &conf_val,
								 std::string &att_conf,std::vector<AttPropDb> &v_db,std::vector<AttrProperty> &def_user_prop,
								std::vector<AttrProperty> &def_class_prop,const char *lib_def)
{
	AttPropDb apd;
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

		std::string old_val = att_conf;
		bool fmt_changed = false;

		if (strcmp(prop_name,"format") == 0)
		{
			set_format_notspec();
			if (format != old_val)
				fmt_changed = true;
		}
		else
			att_conf = lib_def;

		if (old_val != att_conf || fmt_changed == true)
		{
			if (user_defaults == true || class_defaults == true)
			{
				apd.dba = UPD;
				apd.db_value = att_conf;
				v_db.push_back(apd);
			}
			else
			{
				apd.dba = DEL;
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

		std::string old_val = att_conf;
		bool fmt_changed = false;

        if (user_defaults == true)
			att_conf = usr_def_val;
		else
		{
			if (strcmp(prop_name,"format") == 0)
			{
				set_format_notspec();
				if (format != old_val)
					fmt_changed = true;
			}
			else
            	att_conf = lib_def;
		}

		if (old_val != att_conf || fmt_changed == true)
		{
			if (class_defaults == true)
			{
				apd.dba = UPD;
				apd.db_value = att_conf;
				v_db.push_back(apd);
			}
			else
			{
				apd.dba = DEL;
				v_db.push_back(apd);
			}
		}
	}
	else if (TG_strcasecmp(conf_val,NotANumber) == 0)
	{

//
// Return to class default or user default or lib default
//

		std::string old_val = att_conf;
		bool fmt_changed = false;

		if (class_defaults == true)
			att_conf = class_def_val;
		else if (user_defaults == true)
			att_conf = usr_def_val;
		else
		{
			if (strcmp(prop_name,"format") == 0)
			{
				set_format_notspec();
				if (format != old_val)
					fmt_changed = true;
			}
			else
				att_conf = lib_def;
		}

		if (old_val != att_conf || fmt_changed == true)
		{
			apd.dba = DEL;
			v_db.push_back(apd);
		}
	}
	else
	{

//
// Set property
//

		std::string old_val = att_conf;
		att_conf = conf_val;

		if (user_defaults == true && att_conf == usr_def_val)
		{

//
// Property value is the same than the user default value
//

			if (old_val != att_conf)
			{
				if (class_defaults == true)
				{
					apd.dba = UPD;
					apd.db_value = att_conf;
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
			}
		}
		else if (class_defaults == true && att_conf == class_def_val)
		{

//
// Property value is the same than the class default value
//

			if (old_val != att_conf)
			{
				apd.dba = DEL;
				v_db.push_back(apd);
			}
		}
   	 	else if (class_defaults == false && TG_strcasecmp(att_conf.c_str(),lib_def) == 0)
		{

//
// Property value is the same than the lib default value
//

			if (old_val != att_conf)
			{
				apd.dba = DEL;
				v_db.push_back(apd);
			}
		}
		else if (class_defaults == false && strcmp(prop_name,"label") == 0)
		{

//
// Property Label: Property value is the same than the lib default value
//

			if (old_val != att_conf)
			{
				if (TG_strcasecmp(att_conf.c_str(),LabelNotSpec) == 0)
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = UPD;
					apd.db_value = att_conf;
					v_db.push_back(apd);
				}
			}
		}
		else if (class_defaults == false && strcmp(prop_name,"format") == 0)
		{

//
// Property Format: Property value is the same than the lib default value
//

			if (old_val != att_conf)
			{
				if (is_format_notspec(conf_val) == true)
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = UPD;
					apd.db_value = att_conf;
					v_db.push_back(apd);
				}
			}
		}
		else
		{
			if (old_val != att_conf)
			{
				apd.dba = UPD;
				apd.db_value = att_conf;
				v_db.push_back(apd);
			}
		}
	}
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_one_alarm_prop()
//
// description :
// 		Comment 1: If the user input value is equal to user default value do not store it in the database.
//
// 		Comment 2: User default value (if defined) has to be converted to double before it is compared with the user
//		input to determine if to store the new value in the database. String comparison is not appropriate in case of
//		floating point numbers, e.g. "5.0" is numerically equal to "5.00" but the strings differ.
//
// 		Comment 3: If user defaults are defined - at first place the input string is converted to double to determine
// 		if it is a number. If so, the double value is cast to the type corresponding with the type of the attribute and
//		further compared with the double representation of the user default value.
// 		The purpose of casting is as follows. Lets take an example of an attribute of DevShort data type with user
//		default value for min_alarm set to 5. Now, if the user inputs "5.678" as a new value for min_alarm
// 		it would normally be cast to DevShort and stored in the database as "5" (this is the standard behaviour if the
//		user inputs a floating point value for a property of non-floating point type). But now the outcome "5" is equal
//		to the user default value 5 and should not be stored in the database. This is why there is the cast of the
//		user input value to the attribute data type before comparison with the user default value.
//
// Arguments:
//		in :
//			- prop_name : Property name
//			- conf_val : User value
//			- def_user_prop : User defined default prop.
//			- def_class_prop : Class defined default prop.
//		out :
//			- att_conf_str : New prop value as a string
//			- att_conf : New prop value converted as att data type
//			- v_db : Vector of info for db update/delete
//			- check_it : Flag set to true for alarm bits management
//
//---------------------------------------------------------------------------------------------------------------------

void Attribute::set_one_alarm_prop(const char *prop_name,const CORBA::String_member &conf_val,
								std::string &att_conf_str,Tango::Attr_CheckVal &att_conf, std::vector<AttPropDb> &v_db,
								std::vector<AttrProperty> &def_user_prop,
								std::vector<AttrProperty> &def_class_prop,bool &check_it)
{

	if (((data_type == Tango::DEV_STRING) ||
		(data_type == Tango::DEV_BOOLEAN) ||
		(data_type == Tango::DEV_STATE)) &&
        (TG_strcasecmp(conf_val,AlrmValueNotSpec) != 0 &&
         strlen(conf_val) != 0 &&
         TG_strcasecmp(conf_val,NotANumber) != 0))
			throw_err_data_type(prop_name,d_name,"Attribute::set_one_alarm_prop()");


	AttPropDb apd;
	apd.name = prop_name;

	bool user_defaults, class_defaults;
	std::string usr_def_val, class_def_val;
	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

    user_defaults = prop_in_list(prop_name,usr_def_val,nb_user,def_user_prop);
    class_defaults = prop_in_list(prop_name,class_def_val,nb_class,def_class_prop);

	std::stringstream str;
	str.precision(TANGO_FLOAT_PRECISION);

	double alrm_usr_def_db;
	std::string alrm_class_def;
	double alrm_class_def_db;

	bool store_in_db = false;
	bool del_from_db = false;
	bool avns = false;
	bool user_val = false;

	if(TG_strcasecmp(conf_val,AlrmValueNotSpec) == 0)
	{
//
// Return to lib default. If something defined as user default or class default, put entry in DB to overwrite
// these defaults
//

		std::string old_val = att_conf_str;
		att_conf_str = AlrmValueNotSpec;

		if (old_val != att_conf_str)
		{
			if (user_defaults == true || class_defaults == true)
			{
				store_in_db = true;
				avns = true;
			}
			else
			{
				del_from_db = true;
			}
		}
	}
	else if (strlen(conf_val) == 0)
	{
//
// Return to user default or lib default. If something defined as class default, put entry in DB in order to
// overwrite this default value.
//

		std::string old_val = att_conf_str;

        if (user_defaults == false)
            att_conf_str = AlrmValueNotSpec;
        else
        {
			str.str("");
			str.clear();
			str << usr_def_val;
			str >> alrm_usr_def_db;
            att_conf_str = usr_def_val;
		}

		if (old_val != att_conf_str)
		{
			if (class_defaults == true)
			{
				store_in_db = true;
				if (user_defaults)
					user_val = true;
				else
					avns = true;
			}
			else
			{
				del_from_db = true;
			}
		}
	}
	else if(TG_strcasecmp(conf_val,NotANumber) == 0)
	{
// set class default if defined, user default value if defined, otherwise use the library defaults

		std::string old_val = att_conf_str;

		if (class_defaults == false)
		{
            if (user_defaults == false)
                att_conf_str = AlrmValueNotSpec;
            else
            {
				str.str("");
				str.clear();
				str << usr_def_val;
				str >> alrm_usr_def_db;
                att_conf_str = usr_def_val;
			}
		}
		else
		{
			str.str("");
			str.clear();
			str << class_def_val;
			str >> alrm_class_def_db;
            att_conf_str = class_def_val;
		}

		if (old_val != att_conf_str)
			del_from_db = true;
	}
	else
	{
//
// Set property
//

		std::string old_val = att_conf_str;
		att_conf_str = conf_val;

		bool equal_class_def = false;
		bool equal_user_def = false;

		if (old_val != att_conf_str)
		{

//
// Equal to user default?
//

			if (user_defaults == true)
			{
				std::stringstream ss1;
				ss1 << usr_def_val;
				ss1 >> alrm_usr_def_db;

				double db;
				std::stringstream ss;
				ss.precision(TANGO_FLOAT_PRECISION);

				ss << conf_val;
				if (ss >> db && ss.eof())
				{
					switch (data_type)
					{
					case Tango::DEV_SHORT:
					case Tango::DEV_ENUM:
						if((DevShort)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_LONG:
						if((DevLong)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_LONG64:
						if((DevLong64)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_DOUBLE:
						if (db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_FLOAT:
						if(db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_USHORT:
						if((DevUShort)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_UCHAR:
						if((DevUChar)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_ULONG:
						if((DevULong)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_ULONG64:
						if((DevULong64)db == alrm_usr_def_db)
							equal_user_def = true;
						break;

					case Tango::DEV_ENCODED:
						if((DevUChar)db == alrm_usr_def_db)
							equal_user_def = true;
						break;
					}
				}
			}

//
// Equal to class default?
//

			if (class_defaults == true)
			{
				std::stringstream ss1;
				ss1 << class_def_val;
				ss1 >> alrm_class_def_db;

				double db;
				std::stringstream ss;
				ss.precision(TANGO_FLOAT_PRECISION);

				ss << conf_val;
				if (ss >> db && ss.eof())
				{
					switch (data_type)
					{
					case Tango::DEV_SHORT:
					case Tango::DEV_ENUM:
						if((DevShort)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_LONG:
						if((DevLong)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_LONG64:
						if((DevLong64)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_DOUBLE:
						if(db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_FLOAT:
						if(db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_USHORT:
						if((DevUShort)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_UCHAR:
						if((DevUChar)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_ULONG:
						if((DevULong)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_ULONG64:
						if((DevULong64)db == alrm_class_def_db)
							equal_class_def = true;
						break;

					case Tango::DEV_ENCODED:
						if((DevUChar)db == alrm_class_def_db)
							equal_class_def = true;
						break;
					}
				}
			}

			if (user_defaults == true && equal_user_def == true)
			{
				if (class_defaults == true)
				{
					store_in_db = true;
					user_val = true;
				}
				else
				{
					del_from_db = true;
				}
			}
			else if (class_defaults == true && equal_class_def == true)
			{
				del_from_db = true;
			}
			else
				store_in_db = true;
		}
	}


//
// Convert prop to attribute data type
//

	if (att_conf_str == AlrmValueNotSpec)
		check_it = false;
	else
	{
		convert_prop_value(prop_name,att_conf_str,att_conf,d_name);
		check_it = true;

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new  min_value is not above the already
// memorized value
//

		Tango::AttrWriteType w_type = get_writable();
		if ((w_type == Tango::READ_WRITE) || (w_type == Tango::WRITE))
		{
			WAttribute *w_att = static_cast<WAttribute *>(this);
			std::string mem_value;
			if (strcmp(prop_name,"min_value") == 0)
			{
				if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MIN,mem_value) == true))
					throw_min_max_value(d_name,mem_value,MIN);
			}
			else if (strcmp(prop_name,"max_value") == 0)
			{
				if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MAX,mem_value) == true))
					throw_min_max_value(d_name,mem_value,MAX);
			}
		}
	}

//
// For string representation: If there is a default (class or user) and if the user
// entered a value equal to the default but with a different precision (3.2 and 3.20), take the
// default one instead of the user one
//

	if(class_defaults && att_conf_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << att_conf_str;
		if(str >> db && str.eof() && db == alrm_class_def_db)
			att_conf_str = class_def_val;
	}
	else if(user_defaults && att_conf_str != AlrmValueNotSpec)
	{
		double db;
		str.str("");
		str.clear();
		str << att_conf_str;
		if(str >> db && str.eof() && db == alrm_usr_def_db)
			att_conf_str = usr_def_val;
	}

//
// Get info to be stored in DB
//

	if (store_in_db == true)
	{
		std::string tmp = conf_val.in();
		if (user_val == true)
            tmp = usr_def_val.c_str();
		else if (avns == true)
			tmp = AlrmValueNotSpec;
		else
		{
			double db;

			std::stringstream ss;
			ss.precision(TANGO_FLOAT_PRECISION);

			ss << conf_val;
			if (!(ss >> db && ss.eof()))
			{
				throw_err_format(prop_name,d_name,"Attribute::set_one_alarm_prop()");
			}

			ss.str("");
			ss.clear();

			switch (data_type)
			{
			case Tango::DEV_SHORT:
			case Tango::DEV_ENUM:
				ss << (DevShort)db;
				break;

			case Tango::DEV_LONG:
				ss << (DevLong)db;
				break;

			case Tango::DEV_LONG64:
				ss << (DevLong64)db;
				break;

			case Tango::DEV_DOUBLE:
				break;

			case Tango::DEV_FLOAT:
				break;

			case Tango::DEV_USHORT:
				(db < 0.0) ? ss << (DevUShort)(-db) : ss << (DevUShort)db;
				break;

			case Tango::DEV_UCHAR:
				(db < 0.0) ? ss << (short)((DevUChar)(-db)) : ss << (short)((DevUChar)db);
				break;

			case Tango::DEV_ULONG:
				(db < 0.0) ? ss << (DevULong)(-db) : ss << (DevULong)db;
				break;

			case Tango::DEV_ULONG64:
				(db < 0.0) ? ss << (DevULong64)(-db) : ss << (DevULong64)db;
				break;
			}

			if (data_type != Tango::DEV_FLOAT && data_type != Tango::DEV_DOUBLE)
				tmp = ss.str();
		}

		apd.dba = UPD;
		apd.db_value = tmp;
		v_db.push_back(apd);
	}

	if (del_from_db == true)
	{
		apd.dba = DEL;
		v_db.push_back(apd);
	}
}


//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_rds_prop()
//
// description :
//		Manage RDS related properties (delta_val and delta_t)
//
// Arguments:
//		in :
//			- att_alarm : New properties value
//			- dev_name : The device name
//			- def_user_prop : User defined default prop.
//			- def_class_prop : Class defined default prop.
//		out :
//			- v_db : Vector of info for db update/delete
//
//---------------------------------------------------------------------------------------------------------------------


void Attribute::set_rds_prop(const AttributeAlarm &att_alarm, std::string &dev_name,
							 std::vector<AttPropDb> &v_db,
							 std::vector<AttrProperty> &def_user_prop,
							 std::vector<AttrProperty> &def_class_prop)
{
	Tango::Attr_CheckVal old_delta_val = delta_val;
	long old_delta_t = delta_t;

	set_rds_prop_val(att_alarm,dev_name,def_user_prop,def_class_prop);

	bool delta_val_changed = false;
	switch(data_type)
	{
		case Tango::DEV_SHORT:
		if (old_delta_val.sh != delta_val.sh)
			delta_val_changed = true;
		break;

		case Tango::DEV_LONG:
		if (old_delta_val.lg != delta_val.lg)
			delta_val_changed = true;
		break;

		case Tango::DEV_LONG64:
		if (old_delta_val.lg64 != delta_val.lg64)
			delta_val_changed = true;
		break;

		case Tango::DEV_DOUBLE:
		if (old_delta_val.db != delta_val.db)
			delta_val_changed = true;
		break;

		case Tango::DEV_FLOAT:
		if (old_delta_val.fl != delta_val.fl)
			delta_val_changed = true;
		break;

		case Tango::DEV_USHORT:
		if (old_delta_val.ush != delta_val.ush)
			delta_val_changed = true;
		break;

		case Tango::DEV_UCHAR:
		if (old_delta_val.uch != delta_val.uch)
			delta_val_changed = true;
		break;

		case Tango::DEV_ULONG:
		if (old_delta_val.ulg != delta_val.ulg)
			delta_val_changed = true;
		break;

		case Tango::DEV_ULONG64:
		if (old_delta_val.ulg64 != delta_val.ulg64)
			delta_val_changed = true;
		break;

		case Tango::DEV_STATE:
		if (old_delta_val.d_sta != delta_val.d_sta)
			delta_val_changed = true;
		break;
	}

	if (old_delta_t != delta_t || delta_val_changed == true)
		set_rds_prop_db(att_alarm,v_db,def_user_prop,def_class_prop);
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_rds_prop_val()
//
// description :
//		Write into the attribute object the properties related to rds (delta_val and delta_t)
//
// Arguments:
//		in :
//			- att_alarm : New attribute properties
//			- dev_name : The device name
//			- def_user_prop : User defined default prop.
//			- def_class_prop : Class defined default prop.
//
//---------------------------------------------------------------------------------------------------------------------


void Attribute::set_rds_prop_val(const AttributeAlarm &att_alarm, std::string &dev_name,
							 std::vector<AttrProperty> &def_user_prop,
							 std::vector<AttrProperty> &def_class_prop)
{
	std::stringstream str;
	str.precision(TANGO_FLOAT_PRECISION);

	bool delta_val_defined = false;
	std::string delta_val_usr_def;
	double delta_val_usr_def_db;
	std::string delta_val_class_def;
	double delta_val_class_def_db;
	bool usr_defaults = false;
	bool class_defaults = false;

	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

	if(TG_strcasecmp(att_alarm.delta_val,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		delta_val_str = AlrmValueNotSpec;
	}
	else if(TG_strcasecmp(att_alarm.delta_val,NotANumber) == 0)
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
	else if (strlen(att_alarm.delta_val) == 0)
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
		delta_val_str = att_alarm.delta_val;
	}


	if (delta_val_str != AlrmValueNotSpec)
	{
		if ((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			convert_prop_value("delta_val",delta_val_str,delta_val,d_name);
			delta_val_defined = true;
		}
		else
			throw_err_data_type("delta_val",d_name,"Attribute::set_rds_prop_val()");
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

	delete_startup_exception("delta_val",dev_name);

//
// And the delta_t
//

	bool delta_t_defined = false;
	std::string delta_t_usr_def;
	double delta_t_usr_def_db = 0.0;
	std::string delta_t_class_def;
	double delta_t_class_def_db = 0.0;
	usr_defaults = false;
	class_defaults = false;
	if(TG_strcasecmp(att_alarm.delta_t,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(att_alarm.delta_t,"0") == 0 ||
			TG_strcasecmp(att_alarm.delta_t,"0.0") == 0)
	{
		// force library defaults (even if user defaults defined)
		delta_t_str = "0";
	}
	else if (TG_strcasecmp(att_alarm.delta_t,NotANumber) == 0)
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
	else if (strlen(att_alarm.delta_t) == 0)
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
		delta_t_str = att_alarm.delta_t;
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
				throw_err_format("delta_t",d_name,"Attribute::set_rds_prop_val");
			delta_t = (long)db;
			str.str("");
			str.clear();
			str << delta_t;
			delta_t_str = str.str();
			delta_t_defined = true;
		}
		else
			throw_err_data_type("delta_t",d_name,"Attribute::set_rds_prop_val");
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

	delete_startup_exception("delta_t",dev_name);

//
// Set RDS alarm flag only if both delta_t and delta_val are defined
//

	if(delta_t_defined && delta_val_defined)
	{
		alarm_conf.set(rds);
		delete_startup_exception("rds_alarm",dev_name);
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
				std::cerr.setstate(std::ios::failbit);
				Tango::Util *tg = Tango::Util::instance();
				dev = tg->get_device_by_name(d_name);
        std::cerr.clear();
			}

			if (dev->get_logger()->is_warn_enabled())
					dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) incoherent in attribute " << name << " (only " << (delta_t_defined ? "delta_t" : "delta_val") << " is set) " << std::endl;
		}
		catch(...)
		{
      std::cerr.clear();
		}
	}
	else
	{
		alarm_conf.reset(rds);
		delete_startup_exception("rds_alarm",dev_name);
	}
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_rds_prop_db()
//
// description :
//		Check which of the two properties related to RDS (delta_val, delta_t) has to be updated/deleted in Db
//
// Arguments:
//		in :
//			- att_alarm : New attribute properties
//			- def_user_prop : User defined default prop.
//			- def_class_prop : Class defined default prop.
//		out :
//			- v_db : Info for database update/delete
//
//---------------------------------------------------------------------------------------------------------------------


void Attribute::set_rds_prop_db(const AttributeAlarm &att_alarm,
							 std::vector<AttPropDb> &v_db,
							 std::vector<AttrProperty> &def_user_prop,
							 std::vector<AttrProperty> &def_class_prop)
{
	std::stringstream str;
	str.precision(TANGO_FLOAT_PRECISION);

	std::string delta_val_usr_def_val, delta_val_class_def_val;
	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

	bool delta_val_user_defaults;
	bool delta_val_class_defaults;

//
// RDS alarm values (delta_val and delta_t) are stored in or deleted from the database only if both are set or both not
// specified.
//

//
// delta_val
//

	std::string delta_val_tmp_str = att_alarm.delta_val.in();
	bool delta_val_is_number = true;

	delta_val_user_defaults = prop_in_list("delta_val",delta_val_usr_def_val,nb_user,def_user_prop);
	delta_val_class_defaults = prop_in_list("delta_val",delta_val_class_def_val,nb_class,def_class_prop);


	if (delta_val_class_defaults)
	{
		if ((TG_strcasecmp(att_alarm.delta_val,NotANumber) == 0) ||
			(strcmp(att_alarm.delta_val,delta_val_class_def_val.c_str()) == 0))
		{
			delta_val_tmp_str = delta_val_class_def_val;
			delta_val_is_number = false;
		}
		else if (strlen(att_alarm.delta_val) == 0)
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
		else if(TG_strcasecmp(att_alarm.delta_val,AlrmValueNotSpec) == 0)
		{
			delta_val_tmp_str = AlrmValueNotSpec;
			delta_val_is_number = false;
		}
	}
	else if(delta_val_user_defaults)
	{
		if ((TG_strcasecmp(att_alarm.delta_val,NotANumber) == 0) ||
				(strcmp(att_alarm.delta_val,delta_val_usr_def_val.c_str()) == 0) ||
				(strlen(att_alarm.delta_val) == 0))
		{
			delta_val_tmp_str = delta_val_usr_def_val;
			delta_val_is_number = false;
		}
		else if(TG_strcasecmp(att_alarm.delta_val,AlrmValueNotSpec) == 0)
		{
			delta_val_tmp_str = AlrmValueNotSpec;
			delta_val_is_number = false;
		}
	}
	else if ((TG_strcasecmp(att_alarm.delta_val,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(att_alarm.delta_val,NotANumber) == 0) ||
				(strlen(att_alarm.delta_val) == 0))
	{
		delta_val_tmp_str = AlrmValueNotSpec;
		delta_val_is_number = false;
	}

	if(delta_val_is_number)
	{
		if ((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE) &&
			(data_type != Tango::DEV_ENUM))
		{
			double db;

			str.str("");
			str.clear();
			str << att_alarm.delta_val;
			if (!(str >> db && str.eof()))
				throw_err_format("delta_val",d_name,"Attribute::set_rds_prop_db)");
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
			throw_err_data_type("delta_val",d_name,"Attribute::set_rds_prop_db()");
	}

//
// delta_t
//

	std::string delta_t_tmp_str = att_alarm.delta_t.in();
	std::string delta_t_usr_def_val;
	std::string delta_t_class_def_val;
	bool delta_t_user_defaults;
	bool delta_t_class_defaults;
	bool delta_t_is_number = true;

	delta_t_user_defaults = prop_in_list("delta_t",delta_t_usr_def_val,nb_user,def_user_prop);
	delta_t_class_defaults = prop_in_list("delta_t",delta_t_class_def_val,nb_class,def_class_prop);

	if (delta_t_class_defaults)
	{
		if ((TG_strcasecmp(att_alarm.delta_t,NotANumber) == 0) ||
			(strcmp(att_alarm.delta_t,delta_val_class_def_val.c_str()) == 0))
		{
			delta_t_tmp_str = delta_t_class_def_val;
			delta_t_is_number = false;
		}
		else if (strlen(att_alarm.delta_t) == 0)
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
		else if(TG_strcasecmp(att_alarm.delta_t,AlrmValueNotSpec) == 0)
		{
			delta_t_tmp_str = "0";
			delta_t_is_number = false;
		}
	}
	else if(delta_t_user_defaults)
	{
		if ((TG_strcasecmp(att_alarm.delta_t,NotANumber) == 0) ||
				(strcmp(att_alarm.delta_t,delta_t_usr_def_val.c_str()) == 0) ||
				(strlen(att_alarm.delta_t) == 0))
		{
			delta_t_tmp_str = delta_t_usr_def_val;
			delta_t_is_number = false;
		}
		else if((TG_strcasecmp(att_alarm.delta_t,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(att_alarm.delta_t,"0") == 0) ||
				(TG_strcasecmp(att_alarm.delta_t,"0.0") == 0))
		{
			delta_t_tmp_str = "0";
			delta_t_is_number = false;
		}
	}
	else if ((TG_strcasecmp(att_alarm.delta_t,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(att_alarm.delta_t,"0") == 0) ||
				(TG_strcasecmp(att_alarm.delta_t,"0.0") == 0) ||
				(TG_strcasecmp(att_alarm.delta_t,NotANumber) == 0) ||
				(strlen(att_alarm.delta_t) == 0))
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
			str << att_alarm.delta_t;
			double db;
			if (!(str >> db && str.eof()))
				throw_err_format("delta_t",d_name,"Attribute::set_rds_prop_db()");
			str.str("");
			str.clear();
			str << (long)db;
			delta_t_tmp_str = str.str();
		}
		else
			throw_err_data_type("delta_t",d_name,"Attribute::set_rds_prop_db()");
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
		AttPropDb apd;
		apd.name = "delta_val";
		apd.dba = UPD;
		apd.db_value = delta_val_tmp_str.c_str();
		v_db.push_back(apd);

		apd.name = "delta_t";
		apd.db_value = delta_t_tmp_str.c_str();
		v_db.push_back(apd);
	}

	if(rds_delete_both)
	{
		AttPropDb apd;
		apd.name = "delta_val";
		apd.dba = DEL;
		v_db.push_back(apd);

		apd.name = "delta_t";
		v_db.push_back(apd);
	}
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_one_event_prop()
//
// description :
//		Analyse new property when they are related to event. We have four of these properties: rel_change, abs_change,
//		archive_rel_change, archive_abs_change
//
// Arguments:
//		in :
//			- prop_name : The property name
//			- conf_val : The new property value
//			- def_user_prop : User defined default prop.
//			- def_class_prop : Class defined default prop.
//		out :
//			- prop_val : The new property in Attribute object
//			- v_db : Info for database update/delete
//
//---------------------------------------------------------------------------------------------------------------------

void Attribute::set_one_event_prop(const char *prop_name,const CORBA::String_member &conf_val,double *prop_val,
								std::vector<AttPropDb> &v_db,std::vector<AttrProperty> &def_user_prop,
								std::vector<AttrProperty> &def_class_prop)
{
	AttPropDb apd;
	apd.name = prop_name;

	std::string rel_change_str(conf_val); // provided, comma separated min and/or max values for the property
	std::string rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	std::string rel_change_class_str; // class default, comma separated min and/or max values for the property, if defined
	std::vector<double> rel_change_tmp; // vector containing min and max values of the property
	std::vector<double> rel_change_usr; // vector containing user default min and max values of the property
	std::vector<double> rel_change_class; // vector containing class default min and max values of the property
	bool rel_change_usr_def; // true if there are user defaults defined for the property
	bool rel_change_class_def; // true if there are class defaults defined for the property

	std::vector<bool> rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined
	std::vector<bool> rel_change_set_class_def; // vector indicating if to use provided values for the property or the class defaults if defined

	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

	rel_change_usr_def = prop_in_list(prop_name,rel_change_usr_str,nb_user,def_user_prop);
	rel_change_class_def = prop_in_list(prop_name,rel_change_class_str,nb_class,def_class_prop);

//
// Validate user or class default properties
//

	if(rel_change_usr_def)
		validate_change_properties(d_name,prop_name,rel_change_usr_str,rel_change_usr);

	if(rel_change_class_def)
		validate_change_properties(d_name,prop_name,rel_change_class_str,rel_change_class);

//
// Validate user provided values
//

	validate_change_properties(d_name,prop_name,rel_change_str,rel_change_tmp,rel_change_set_usr_def,rel_change_set_class_def);

//
// Set values
//

	double old_val[2];
	old_val[0] = prop_val[0];
	old_val[1] = prop_val[1];

	if (rel_change_class_def)
	{
		if (rel_change_set_class_def[0] == true)
			prop_val[0] = rel_change_class[0];
		else if (rel_change_usr_def)
		{
			if (rel_change_set_usr_def[0] == true)
				prop_val[0] = rel_change_usr[0];
			else
				prop_val[0] = rel_change_tmp[0];
		}
		else
			prop_val[0] = rel_change_tmp[0];

		if (rel_change_set_class_def[1] == true)
			prop_val[1] = rel_change_class[1];
		else if (rel_change_usr_def)
		{
			if (rel_change_set_usr_def[1] == true)
				prop_val[1] = rel_change_usr[1];
			else
				prop_val[1] = rel_change_tmp[1];
		}
		else
			prop_val[1] = rel_change_tmp[1];
	}
	else if(rel_change_usr_def)
	{
		prop_val[0] = (rel_change_set_class_def[0]) ? rel_change_usr[0] : ((rel_change_set_usr_def[0]) ? rel_change_usr[0] : rel_change_tmp[0]);
		prop_val[1] = (rel_change_set_class_def[1]) ? rel_change_usr[1] : ((rel_change_set_usr_def[1]) ? rel_change_usr[1] : rel_change_tmp[1]);
	}
	else
	{
		prop_val[0] = rel_change_tmp[0];
		prop_val[1] = rel_change_tmp[1];
	}

//
// Manage db
//

	if (old_val[0] != prop_val[0] || old_val[1] != prop_val[1])
	{
		if (prop_val[0] == prop_val[1])
		{
			if (prop_val[0] == INT_MAX)
			{
				if (rel_change_usr_def == true || rel_change_class_def == true)
				{
					apd.dba = UPD_FROM_DB;
					apd.db_value_db.clear();
					apd.db_value_db.push_back(prop_val[0]);
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
			}
			else if (rel_change_usr_def == true && prop_val[0] == fabs(rel_change_usr[0]))
			{
				if (rel_change_class_def == true)
				{
					apd.dba = UPD_FROM_DB;
					apd.db_value_db.clear();
					apd.db_value_db.push_back(prop_val[0]);
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
			}
			else if (rel_change_class_def == true && prop_val[0] == fabs(rel_change_class[0]))
			{
				apd.dba = DEL;
				v_db.push_back(apd);
			}
			else
			{
				apd.dba = UPD_FROM_DB;
				apd.db_value_db.clear();
				apd.db_value_db.push_back(prop_val[0]);
				v_db.push_back(apd);
			}
		}
		else
		{
			if (rel_change_class_def)
			{
				rel_change_tmp.clear();
				rel_change_tmp.push_back(prop_val[0]);
				rel_change_tmp.push_back(prop_val[1]);

				event_prop_db_xxx(rel_change_tmp,rel_change_class,v_db,apd);
			}
			else if(rel_change_usr_def)
			{
				rel_change_tmp.clear();
				rel_change_tmp.push_back(prop_val[0]);
				rel_change_tmp.push_back(prop_val[1]);

				event_prop_db_xxx(rel_change_tmp,rel_change_usr,v_db,apd);
			}
			else
			{
				apd.dba = UPD_FROM_DB;
				apd.db_value_db = rel_change_tmp;
				v_db.push_back(apd);
			}
		}
	}
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_prop_5_specific
//
// description :
//		Set IDl 5 specific attribute configuration. It's only the enum labels for attribute of the DEV_ENUM data type.
//		The other IDL 5 specific attribute properties are non-mutable.
//
// argument :
//		in :
//			- conf : The new attribute configuration
//			- dev_name: The device name
//			- from_ds : Flag set to true if the request comes from a DS code
//		out :
//			- v_db : Info for Db update/delete
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::set_prop_5_specific(const AttributeConfig_5 &conf,std::string &dev_name,bool from_ds,std::vector<AttPropDb> &v_db)
{
	if (data_type == Tango::DEV_ENUM)
	{

		AttPropDb apd;
		apd.name = "enum_labels";

//
// Some error cases: Zero length in labels vector
//

		if (conf.enum_labels.length() == 0)
		{
			std::stringstream ss;
			ss << "Device " << dev_name << "- Attribute : " << name;
			ss << "- No value defined for the property enum_labels";

			Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::set_prop_5_specific()");
		}

		if (from_ds == false)
		{
			if (!(conf.enum_labels.length() == 1 &&
				(TG_strcasecmp(conf.enum_labels[0],AlrmValueNotSpec) == 0 ||
				TG_strcasecmp(conf.enum_labels[0],NotANumber) == 0 ||
				strlen(conf.enum_labels[0]) == 0)))
			{
				if (conf.enum_labels.length() != enum_labels.size())
				{
					std::stringstream ss;
					ss << "Device " << dev_name << "-> Attribute : " << name;
					ss << "\nIt's not supported to change enumeration labels number from outside the Tango device class code";

					Except::throw_exception(API_NotSupportedFeature,ss.str(),"Attribute::set_prop_5_specific()");
				}
			}
		}

//
// Set some data used by the algo like the user or class properties
//

		Tango::DeviceClass *dev_class = get_att_device_class(dev_name);
		Tango::Attr *att_ptr;

		Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
		att_ptr = &(mca->get_attr(name));

		std::vector<AttrProperty> &def_user_prop = att_ptr->get_user_default_properties();
		size_t nb_user = def_user_prop.size();

		std::vector<AttrProperty> &def_class_prop = att_ptr->get_class_properties();
		size_t nb_class = def_class_prop.size();

//
// Set the enum labels
//

		std::string enum_labels_usr_def;
		std::string enum_labels_class_def;
		bool usr_defaults = false;
		bool class_defaults = false;

		usr_defaults = prop_in_list("enum_labels",enum_labels_usr_def,nb_user,def_user_prop);
		class_defaults = prop_in_list("enum_labels",enum_labels_class_def,nb_class,def_class_prop);

		std::vector<std::string> old_labels = enum_labels;

		if(TG_strcasecmp(conf.enum_labels[0],AlrmValueNotSpec) == 0)
		{
			// no library defaults for enum
			std::stringstream ss;
			ss << "Device " << dev_name << "-> Attribute : " << name;
			ss << "\nNo enumeration label(s) default library value for attribute of the Tango::DEV_ENUM data type";

			Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::set_prop_5_specific()");
		}
		else if (strlen(conf.enum_labels[0]) == 0)
		{
			// set user default value if defined, otherwise use the library defaults

			if (usr_defaults == false)
			{
				std::stringstream ss;
				ss << "Device " << dev_name << "-> Attribute : " << name;
				ss << "\nNo enumeration labels default library value for attribute of the Tango::DEV_ENUM data type";

				Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::set_prop_5_specific()");
			}
			else
			{
				build_check_enum_labels(enum_labels_usr_def);
				if (enum_labels != old_labels)
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
			}
		}
		else if(TG_strcasecmp(conf.enum_labels[0],NotANumber) == 0)
		{
			// set class default if defined, user default value if defined, otherwise use the library defaults

			if (class_defaults == false)
			{
				if (usr_defaults == false)
				{
					std::stringstream ss;
					ss << "Device " << dev_name << "-> Attribute : " << name;
					ss << "\nNo enumeration labels default library value for attribute of the Tango::DEV_ENUM data type";

					Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::set_prop_5_specific()");
				}
				else
				{
					build_check_enum_labels(enum_labels_usr_def);
					if (enum_labels != old_labels)
					{
						apd.dba = DEL;
						v_db.push_back(apd);
					}
				}
			}
			else
			{
				build_check_enum_labels(enum_labels_class_def);
				if (enum_labels != old_labels)
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
			}
		}
		else
		{
			// set property

			std::string labs;
			for (size_t loop = 0;loop < conf.enum_labels.length();loop++)
			{
				labs = labs + conf.enum_labels[loop].in();
				if (loop != conf.enum_labels.length() - 1)
					labs = labs + ',';
			}
			build_check_enum_labels(labs);

			if (enum_labels != old_labels)
			{
				if (usr_defaults == true && labs == enum_labels_usr_def)
				{
					if (class_defaults == true)
					{
						apd.dba = UPD_FROM_VECT_STR;
						apd.db_value_v_str = enum_labels;
					}
					else
					{
						apd.dba = DEL;
					}
					v_db.push_back(apd);
				}
				else if (class_defaults == true && labs == enum_labels_class_def)
				{
					apd.dba = DEL;
					v_db.push_back(apd);
				}
				else
				{
					apd.dba = UPD_FROM_VECT_STR;
					apd.db_value_v_str = enum_labels;
					v_db.push_back(apd);
				}
			}
		}

		delete_startup_exception("enum_labels",dev_name);
	}
}

void Attribute::set_min_alarm(char *new_min_alarm_str)
{
	set_min_alarm(std::string(new_min_alarm_str));
}

void Attribute::set_min_alarm(const char *new_min_alarm_str)
{
	set_min_alarm(std::string(new_min_alarm_str));
}


void Attribute::set_max_alarm(char *new_max_alarm_str)
{
	set_max_alarm(std::string(new_max_alarm_str));
}

void Attribute::set_max_alarm(const char *new_max_alarm_str)
{
	set_max_alarm(std::string(new_max_alarm_str));
}


void Attribute::set_min_warning(char *new_min_warning_str)
{
	set_min_warning(std::string(new_min_warning_str));
}

void Attribute::set_min_warning(const char *new_min_warning_str)
{
	set_min_warning(std::string(new_min_warning_str));
}


void Attribute::set_max_warning(char *new_max_warning_str)
{
	set_max_warning(std::string(new_max_warning_str));
}

void Attribute::set_max_warning(const char *new_max_warning_str)
{
	set_max_warning(std::string(new_max_warning_str));
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::check_range_coherency
//
// description :
//		Check coherency between min and max value for properties where a min and a max is used
//
// argument :
//		in :
//			- dev_name: The device name
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::check_range_coherency(std::string &dev_name)
{

//
// Check ranges coherence for min and max value
//

	if(check_min_value && check_max_value)
	{
		if ((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE) &&
			(data_type != Tango::DEV_ENUM))
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
			(data_type != Tango::DEV_STATE) &&
			(data_type != Tango::DEV_ENUM))
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
			(data_type != Tango::DEV_STATE) &&
			(data_type != Tango::DEV_ENUM))
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
}



//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::db_access
//
// description :
//		Update / Delete info in database
//
// argument :
// 		in :
//			- cosp : The structure with all infos needed to access the database
//			- dev_name : The device name
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::db_access(Attribute::CheckOneStrProp &cosp,std::string &dev_name)
{

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
				tg->get_database()->put_device_attribute_property(dev_name,*cosp.db_d);
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
				tg->get_database()->delete_device_attribute_property(dev_name,*cosp.db_del);
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

void Attribute::validate_change_properties(const std::string &dev_name, const char *prop_name, std::string &change_prop_str, std::vector<double> &validated_prop)
{
	std::vector<bool> bring_usr_def;
	std::vector<bool> bring_class_def;

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

void Attribute::validate_change_properties(const std::string &dev_name, const char *prop_name, std::string &change_prop_str, std::vector<double> &validated_prop,
                                           std::vector<bool> &bring_usr_def,std::vector<bool> &bring_class_def)
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

	std::stringstream str;
	str.precision(TANGO_FLOAT_PRECISION);
	std::string prop_min;
	std::string prop_max;
	bool one_param = true;
	double prop_tmp;

	size_t pos = change_prop_str.find(',');
	if(pos != std::string::npos)
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
					throw_err_data_type(prop_name,const_cast<std::string&>(dev_name),"Attribute::validate_change_properties()");
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
						throw_err_data_type(prop_name,const_cast<std::string&>(dev_name),"Attribute::validate_change_properties()");
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
//		Attribute::event_prop_db_xxx
//
// description :
//
// argument :
// 		in :
//          - rel_change_tmp :
//			- rel_change_usr :
//			- v_db :
//		out :
//			- apd :
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::event_prop_db_xxx(std::vector<double> &rel_change_tmp,std::vector<double> &rel_change_usr,std::vector<AttPropDb> &v_db,AttPropDb &apd)
{

    std::vector<std::string> rel_change_str_tmp(2);
    std::vector<bool> rel_change_usr_def_tmp(2);
    rel_change_usr_def_tmp[0] = rel_change_usr_def_tmp[1] = false;

    if(rel_change_tmp[0] == rel_change_usr[0])
    {
        rel_change_str_tmp[0] = NotANumber;
        rel_change_usr_def_tmp[0] = true;
    }

    if(rel_change_tmp[1] == rel_change_usr[1])
    {
        rel_change_str_tmp[1] = NotANumber;
        rel_change_usr_def_tmp[1] = true;
    }

    if(rel_change_usr_def_tmp[0] && rel_change_usr_def_tmp[1])
    {
    	apd.dba = DEL;
    	v_db.push_back(apd);
    }
    else if(!rel_change_usr_def_tmp[0] && !rel_change_usr_def_tmp[1])
    {
    	apd.dba = UPD_FROM_DB;
    	apd.db_value_db = rel_change_tmp;
    	v_db.push_back(apd);
    }
    else
    {
        std::stringstream str;
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
        apd.dba = UPD_FROM_VECT_STR;
        apd.db_value_v_str = rel_change_str_tmp;
        v_db.push_back(apd);
    }
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_one_event_period()
//
// description :
//		Analyse new property when for event period. We have two of them.
//
// Arguments:
//		in :
//			- prop_name : The property name
//			- conf_val : The new property value
//			- prop_def : Property lib default value
//			- def_user_prop : User defined default prop.
//			- def_class_prop : Class defined default prop.
//		out :
//			- prop_val : The new property in Attribute object
//			- v_db : Info for database update/delete
//
//---------------------------------------------------------------------------------------------------------------------

void Attribute::set_one_event_period(const char *prop_name,const CORBA::String_member &conf_val,int &prop_val,const int &prop_def,
								std::vector<AttPropDb> &v_db,std::vector<AttrProperty> &def_user_prop,
								std::vector<AttrProperty> &def_class_prop)
{
	AttPropDb apd;
	apd.name = prop_name;

	size_t nb_user = def_user_prop.size();
	size_t nb_class = def_class_prop.size();

	std::stringstream def_event_period;
	def_event_period << (int)(prop_def);

    std::string class_def_val;
    std::string usr_def_val;

	bool user_defaults = prop_in_list(prop_name,usr_def_val,nb_user,def_user_prop);
	bool class_defaults = prop_in_list(prop_name,class_def_val,nb_class,def_class_prop);

	std::stringstream str;
	str.precision(TANGO_FLOAT_PRECISION);

	int old_prop_val = prop_val;

	if(TG_strcasecmp(conf_val,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(conf_val,def_event_period.str().c_str()) == 0)
	{
		// force library defaults (even if user defaults defined)
		prop_val = prop_def;
	}
	else if (TG_strcasecmp(conf_val,NotANumber) == 0)
	{
		if (class_defaults == false)
		{
            if (user_defaults == false)
                prop_val = prop_def;
            else
            {
                str.str("");
                str.clear();
                str << usr_def_val;
                double db;
                if (!(str >> db && str.eof()))
                    throw_err_format(prop_name,d_name,"Attribute::set_one_event_period()");
                prop_val = (int)db;
            }
		}
		else
		{
            str.str("");
            str.clear();
            str << class_def_val;
            double db;
            if (!(str >> db && str.eof()))
                throw_err_format(prop_name,d_name,"Attribute::set_one_event_period()");
            prop_val = (int)db;
		}
	}
	else if (strlen(conf_val) == 0)
	{
		// set user default value if defined, otherwise use the library defaults
        if (user_defaults == false)
            prop_val = prop_def;
        else
        {
            str.str("");
            str.clear();
            str << usr_def_val;
            double db;
            if (!(str >> db && str.eof()))
                throw_err_format(prop_name,d_name,"Attribute::set_one_event_period()");
            prop_val = (int)db;
        }
	}
	else
	{
		// set property
		str.str("");
		str.clear();
		str << conf_val;
		double db;
		if (!(str >> db && str.eof()))
			throw_err_format(prop_name,d_name,"Attribute::set_one_event_period()");
		prop_val = (int)db;
	}

//
// Manage db
//

	bool store_in_db = true;

	if (prop_val != old_prop_val)
	{
		if (class_defaults)
		{
			if((TG_strcasecmp(class_def_val.c_str(),AlrmValueNotSpec) == 0) ||
					(TG_strcasecmp(class_def_val.c_str(),NotANumber) == 0) ||
					(strlen(class_def_val.c_str()) == 0))
			{
				std::stringstream str;
				str << (int)(prop_def);
				class_def_val = str.str();
			}

			bool input_equal_def = false;
			str.str("");
			str.clear();
			str << conf_val.in();
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
			if ((TG_strcasecmp(conf_val,NotANumber) == 0) ||
					(strcmp(conf_val.in(),class_def_val.c_str()) == 0) ||
					(input_equal_def == true))
				store_in_db = false;
		}
		else if (user_defaults)
		{
			bool input_equal_def = false;
			str.str("");
			str.clear();
			str << conf_val.in();
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
			if ((TG_strcasecmp(conf_val,NotANumber) == 0) ||
					(strcmp(conf_val.in(),usr_def_val.c_str()) == 0) ||
					(strlen(conf_val) == 0) ||
					(input_equal_def == true))
				store_in_db = false;
		}
		else
		{
			bool input_equal_def = false;
			str.str("");
			str.clear();
			str << conf_val.in();
			double db;
			if (str >> db && str.eof())
			{
				if((int)db == (int)(prop_def))
					input_equal_def = true;
			}
			if ((TG_strcasecmp(conf_val,AlrmValueNotSpec) == 0) ||
					(TG_strcasecmp(conf_val,def_event_period.str().c_str()) == 0) ||
					(TG_strcasecmp(conf_val,NotANumber) == 0) ||
					(strlen(conf_val) == 0) ||
					(input_equal_def == true))
				store_in_db = false;
		}

		if(store_in_db)
		{
			std::string tmp = conf_val.in();
			if (TG_strcasecmp(conf_val,AlrmValueNotSpec) == 0)
			{
				tmp = def_event_period.str();
			}
			else if (strlen(conf_val) == 0)
			{
				if (class_defaults && user_defaults)
					tmp = usr_def_val;
				else
					tmp = def_event_period.str();
			}
			else
			{
				str.str("");
				str.clear();
				str << conf_val.in();
				double db;
				if (!(str >> db && str.eof()))
					throw_err_format(prop_name,d_name,"Attribute::set_one_event_period()");
				str.str("");
				str.clear();
				str << (int)db;
				tmp = str.str();
			}

			apd.dba = UPD;
			apd.db_value = tmp;
			v_db.push_back(apd);
		}
		else
		{
			apd.dba = DEL;
			v_db.push_back(apd);
		}
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
		std::string root_attr_name(fwd->get_fwd_dev_name() + '/' + fwd->get_fwd_att_name());
		std::string user_root_att_name(user_conf.root_attr_name.in());
		std::transform(user_root_att_name.begin(),user_root_att_name.end(),user_root_att_name.begin(),::tolower);
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
		if (watt->is_memorized() != user_conf.memorized)
		{
			throw_hard_coded_prop("memorized");
		}
		if (watt->is_memorized() == true)
		{
			if (watt->is_memorized_init() != user_conf.mem_init)
			{
				throw_hard_coded_prop("memorized");
			}
		}
	}

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

void Attribute::convert_prop_value(const char *prop_name,std::string &value_str,Attr_CheckVal &val,const std::string &dev_name)
{
	std::stringstream str;
	str.precision(TANGO_FLOAT_PRECISION);

	str << value_str;
	if (!(str >> val.db && str.eof()))
		throw_err_format(prop_name,dev_name,"Attribute::convert_prop_value()");

	switch (data_type)
	{
	case Tango::DEV_SHORT:
	case Tango::DEV_ENUM:
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

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::upd_database
//
// description :
//		Update database according to the info received in the v_db parameter. We update or delete info in db for
//		attribute configuration.
//
// Arguments:
//		in :
//			- v_db : The vector of info with what has to be done in DB
//
//---------------------------------------------------------------------------------------------------------------------

void Attribute::upd_database(std::vector<AttPropDb> &v_db)
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

	std::vector<AttPropDb>::iterator ite;

//
// A loop for each db action
//

	for (ite = v_db.begin();ite != v_db.end();++ite)
	{
		switch (ite->dba)
		{
			case UPD:
			{
				DbDatum desc(ite->name);
				desc << ite->db_value;
				db_d.push_back(desc);
				prop_to_update++;
			}
			break;

			case UPD_FROM_DB:
			{
				DbDatum desc(ite->name);
				desc << ite->db_value_db;
				db_d.push_back(desc);
				prop_to_update++;
			}
			break;

			case UPD_FROM_VECT_STR:
			{
				DbDatum desc(ite->name);
				desc << ite->db_value_v_str;
				db_d.push_back(desc);
				prop_to_update++;
			}
			break;

			case DEL:
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

	struct CheckOneStrProp cosp;
	cosp.prop_to_delete = &prop_to_delete;
	cosp.prop_to_update = &prop_to_update;
	cosp.db_d = &db_d;
	cosp.db_del = &db_del;

	db_access(cosp,d_name);
}

} // End of Tango namespace
