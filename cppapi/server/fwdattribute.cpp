static const char *RcsId = "$Id$\n$Name$";

//====================================================================================================================
//
// file :               fwdattribute.cpp
//
// description :        C++ source code for the FwdAttribute class. This class is used to manage attribute.
//						A Tango Device object instance has one MultiAttribute object which is an aggregate of
//						Attribute, FwdAttribute, WAttribute and FwdWAttribute objects
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
#include <fwdattribute.h>

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::FwdAttribute
//
// description :
//		Constructor for the FwdAttribute class from the attribute property vector, its type and the device name
//
// argument :
//		in :
//			- prop_list : The attribute property list
//			- tmp_attr :
//			- dev_name : The device name
//			- idx :
//
//--------------------------------------------------------------------------------------------------------------------

FwdAttribute::FwdAttribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx)
:Attribute(prop_list,tmp_attr,dev_name,idx)
{
	FwdAttr &attr = static_cast<FwdAttr &>(tmp_attr);

//
// Init forwarded attribute specific data
//

	fwd_dev_name = attr.get_fwd_dev_name();
	fwd_att_name = attr.get_fwd_root_att();

//
// Clear Attrdesc in Root attribute registry
//

//	RootAttRegistry &fdp = Util::instance()->get_root_att_reg();
//	fdp.clear_attrdesc(fwd_dev_name,fwd_att_name);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::~FwdAttribute
//
// description :
//		Destructor for the FwdAttribute class
//
//--------------------------------------------------------------------------------------------------------------------

FwdAttribute::~FwdAttribute()
{
	RootAttRegistry &fdp = Util::instance()->get_root_att_reg();
	fdp.remove_root_att(fwd_dev_name,fwd_att_name);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::set_att_config
//
// description :
//		Set attribute configuration. In this case, it is much easier than for classical attribute. The config. comes
//		from the root attribute and therefore, all tests have already been done in the root attribute and we don't
//		need to store anything in DB
//		Note that the main attribute characteristics (format, type, memorized...) are set via FwdAttr instance and are
//		not changeable here
//
//		This method is called when a new attribute configuration is received via the Callback installed for
//		forwarded attribute
//
// argument :
//		in :
//			- conf : The attribute new configuration
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttribute::set_att_config(const Tango::AttributeConfig_5 &conf)
{
	description = conf.description;
	label = conf.label;
	unit = conf.unit;
	standard_unit = conf.standard_unit;
	display_unit = conf.display_unit;
	format = conf.format;

	min_value_str = conf.min_value;
	max_value_str = conf.max_value;

	disp_level = conf.level;

	min_alarm_str = conf.att_alarm.min_alarm;
	max_alarm_str = conf.att_alarm.max_alarm;
	min_warning_str = conf.att_alarm.min_warning;
	max_warning_str = conf.att_alarm.max_warning;
	delta_val_str = conf.att_alarm.delta_val;

	stringstream ss;
	string tmp_prop = conf.att_alarm.delta_t.in();
	if (tmp_prop == AlrmValueNotSpec)
	{
		delta_t_str = "0";
		delta_t = 0;
	}
	else
	{
		ss << tmp_prop;
		ss >> delta_t;
		delta_t_str = tmp_prop;
	}

//
// Set event related properties (not stored as strings)
//

	tmp_prop = conf.event_prop.ch_event.rel_change;
	convert_event_prop(tmp_prop,rel_change);

	tmp_prop = conf.event_prop.ch_event.abs_change;
	convert_event_prop(tmp_prop,abs_change);

	ss.str("");
	ss.clear();

	tmp_prop = conf.event_prop.per_event.period;
	if (tmp_prop == AlrmValueNotSpec)
		event_period = DEFAULT_EVENT_PERIOD;
	else
	{
		ss << tmp_prop;
		ss >> event_period;
	}

	tmp_prop = conf.event_prop.arch_event.rel_change;
	convert_event_prop(tmp_prop,archive_rel_change);

	tmp_prop = conf.event_prop.arch_event.abs_change;
	convert_event_prop(tmp_prop,archive_abs_change);

	ss.str("");
	ss.clear();

	tmp_prop = conf.event_prop.arch_event.period;
	if (tmp_prop == AlrmValueNotSpec)
		archive_period = INT_MAX;
	else
	{
		ss << tmp_prop;
		ss >> archive_period;
	}
}

//
// Same method than previous but the argument is not the same and this one is called when a fowrwarded
// attribute configuration is written via a set_attribute_config
//

void FwdAttribute::set_att_config(AttributeInfoEx *aie_ptr)
{
	data_format = aie_ptr->data_format;
	writable = aie_ptr->writable;
	data_type = aie_ptr->data_type;
	disp_level = aie_ptr->disp_level;
//	a_ptr->get_memorized();			For WAttribute
//	a_ptr->get_memorized_init();	For WAttribute
//	a_ptr->get_assoc();
	max_x = aie_ptr->max_dim_x;
	max_y = aie_ptr->max_dim_y;

	label = aie_ptr->label;
	description = aie_ptr->description;
	unit = aie_ptr->unit;
	standard_unit = aie_ptr->standard_unit;
	display_unit = aie_ptr->display_unit;
	format = aie_ptr->format;

	min_value_str = aie_ptr->min_value;
	max_value_str = aie_ptr->max_value;

	disp_level = aie_ptr->disp_level;

	min_alarm_str = aie_ptr->alarms.min_alarm;
	max_alarm_str = aie_ptr->alarms.max_alarm;
	min_warning_str = aie_ptr->alarms.min_warning;
	max_warning_str = aie_ptr->alarms.max_warning;
	delta_val_str = aie_ptr->alarms.delta_val;
	delta_t_str = aie_ptr->alarms.delta_t;

	stringstream ss;
	ss << delta_t_str;
	ss >> delta_t;

//
// Set event related properties (not stored as strings)
//

	string tmp_prop(aie_ptr->events.ch_event.rel_change);
	convert_event_prop(tmp_prop,rel_change);

	tmp_prop = aie_ptr->events.ch_event.abs_change;
	convert_event_prop(tmp_prop,abs_change);

	ss.str("");
	ss.clear();

	if (aie_ptr->events.per_event.period != AlrmValueNotSpec)
	{
		ss << aie_ptr->events.per_event.period;
		ss >> event_period;
	}

	tmp_prop = aie_ptr->events.arch_event.archive_rel_change;
	convert_event_prop(tmp_prop,archive_rel_change);

	tmp_prop = aie_ptr->events.arch_event.archive_abs_change;
	convert_event_prop(tmp_prop,archive_abs_change);

	ss.str("");
	ss.clear();

	if (aie_ptr->events.arch_event.archive_period != AlrmValueNotSpec)
	{
		ss << aie_ptr->events.arch_event.archive_period;
		ss >> archive_period;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::convert_event_prop
//
// description :
//		Convert event property received as a string into double
//
// argument :
//		in :
//			- prop_str : The attribute property as a string
//		out :
//			- ptr : Pointer to where the converted vlue must be stored
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttribute::convert_event_prop(string &prop_str,double *ptr)
{
	stringstream ss;
	double tmp_db;

	if (prop_str != AlrmValueNotSpec)
	{
		string::size_type pos = prop_str.find(',');
		if (pos == string::npos)
		{
			ss << prop_str;
			ss >> tmp_db;
			ptr[0] = tmp_db;
			ptr[1] = tmp_db;
		}
		else
		{
			string first = prop_str.substr(0,pos);
			string second = prop_str.substr(pos + 1);

			ss << first;
			ss >> ptr[0];

			ss.str("");
			ss.clear();

			ss << second;
			ss >> ptr[1];
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::upd_att_config
//
// description :
//		Update a forwarded attribute config. Only label is maintained locally. Send att conf. to root device only
//		if it is modified compared to what we have
//
// argument :
//		in :
//			- conf : The attribute new configuration
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttribute::upd_att_config(const Tango::AttributeConfig_5 &conf)
{
	cout4 << "Entering FwdAttribute::upd_att_config for attribute " << conf.name << endl;

//
// A new label (the only att property stored locally)
//

	if (string(conf.label.in()) != label)
		upd_att_label(conf.label.in());

//
// Send new config to root attribute if received configuration if different than the one we already have
//

	if (new_att_conf(conf) == true)
	{
		AttributeInfoListEx aile;
		AttributeInfoEx aie;
		aie = &conf;
		aie.label = LabelNotSpec;

		RootAttRegistry &rar = Util::instance()->get_root_att_reg();
		DeviceProxy *dev = rar.get_root_att_dp(fwd_dev_name);

		aie.name = fwd_att_name;
		aile.push_back(aie);

		cout4 << "Sending att conf to root device " << fwd_dev_name << endl;
		dev->set_attribute_config(aile);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::new_att_conf
//
// description :
//		Check if the new attribute config is the same than the one we have
//
// argument :
//		in :
//			- conf : The attribute new configuration
//
// return :
//		A boolean set to true if the new config is different than the one we have
//
//--------------------------------------------------------------------------------------------------------------------

bool FwdAttribute::new_att_conf(const Tango::AttributeConfig_5 &conf)
{
	if (string(conf.name.in()) != name)
		return true;

	if (conf.writable != writable)
		return true;

	if (conf.data_format != data_format)
		return true;

	if (conf.data_type != data_type)
		return true;

// TODO: memorized, mem_init

	if (string(conf.description.in()) != description)
		return true;

	if (string(conf.unit.in()) != unit)
		return true;

	if (string(conf.standard_unit.in()) != standard_unit)
		return true;

	if (string(conf.display_unit.in()) != display_unit)
		return true;

	if (string(conf.format.in()) != format)
		return true;

	if (string(conf.min_value.in()) != min_value_str)
		return true;

	if (string(conf.max_value.in()) != max_value_str)
		return true;

	if (conf.level != disp_level)
		return true;

	if (conf.writable_attr_name.in() != writable_attr_name)
		return true;

	if (string(conf.att_alarm.min_alarm.in()) != min_alarm_str)
		return true;

	if (string(conf.att_alarm.max_alarm.in()) != max_alarm_str)
		return true;

	if (string(conf.att_alarm.min_warning.in()) != min_warning_str)
		return true;

	if (string(conf.att_alarm.max_warning.in()) != max_warning_str)
		return true;

	string tmp_prop(conf.att_alarm.delta_t);
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (delta_t_str != "0")
			return true;
	}
	else
	{
		if (tmp_prop != delta_t_str)
			return true;
	}

	tmp_prop = conf.att_alarm.delta_val;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (delta_val_str != AlrmValueNotSpec)
			return true;
	}
	else
	{
		if (tmp_prop != delta_val_str)
			return true;
	}

	double tmp_array[2];

//
// rel_change
//

	tmp_prop = conf.event_prop.ch_event.rel_change;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (rel_change[0] != INT_MAX || rel_change[1] != INT_MAX)
			return true;
	}
	else
	{
		if (rel_change[0] == INT_MAX && rel_change[1] == INT_MAX)
			return true;
		else
		{
			convert_event_prop(tmp_prop,tmp_array);
			if (tmp_array[0] != rel_change[0] || tmp_array[1] != rel_change[1])
				return true;
		}
	}

//
// abs_change
//

	tmp_prop = conf.event_prop.ch_event.abs_change;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (abs_change[0] != INT_MAX || abs_change[1] != INT_MAX)
			return true;
	}
	else
	{
		if (abs_change[0] == INT_MAX && abs_change[1] == INT_MAX)
			return true;
		else
		{
			convert_event_prop(tmp_prop,tmp_array);
			if (tmp_array[0] != abs_change[0] || tmp_array[1] != abs_change[1])
				return true;
		}
	}

//
// archive rel_change
//

	tmp_prop = conf.event_prop.arch_event.rel_change;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (archive_rel_change[0] != INT_MAX || archive_rel_change[1] != INT_MAX)
			return true;
	}
	else
	{
		if (archive_rel_change[0] == INT_MAX && archive_rel_change[1] == INT_MAX)
			return true;
		else
		{
			convert_event_prop(tmp_prop,tmp_array);
			if (tmp_array[0] != archive_rel_change[0] || tmp_array[1] != archive_rel_change[1])
				return true;
		}
	}

//
// archive abs_change
//

	tmp_prop = conf.event_prop.arch_event.abs_change;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (archive_abs_change[0] != INT_MAX || archive_abs_change[1] != INT_MAX)
			return true;
	}
	else
	{
		if (archive_abs_change[0] == INT_MAX && archive_abs_change[1] == INT_MAX)
			return true;
		else
		{
			convert_event_prop(tmp_prop,tmp_array);
			if (tmp_array[0] != archive_abs_change[0] || tmp_array[1] != archive_abs_change[1])
				return true;
		}
	}

//
// event period
//

	tmp_prop = conf.event_prop.per_event.period;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (event_period != DEFAULT_EVENT_PERIOD)
			return true;
	}
	else
	{
		stringstream ss;
		int tmp_per;

		ss << tmp_prop;
		ss >> tmp_per;

		if (tmp_per != event_period)
			return true;
	}

//
// archive event period
//

	tmp_prop = conf.event_prop.arch_event.period;
	if (tmp_prop == AlrmValueNotSpec)
	{
		if (archive_period != INT_MAX)
			return true;
	}
	else
	{
		stringstream ss;
		int tmp_per;

		ss << tmp_prop;
		ss >> tmp_per;

		if  (tmp_per != archive_period)
			return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::upd_att_label
//
// description :
//		Update the forwarded attribute label property. It is the only property which stays local for forwarded
//		attribute
//
// argument :
//		in :
//			- new_label : The new attribute label
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttribute::upd_att_label(const char *new_label)
{
	cout4 << "Entering FwdAttribute::upd_att_label() for att " << name << " with label set to " << new_label << endl;
	string old_label = label;

//
// Change label according to att property changing rule:
//	LabelNotSpec or AlrmvalueNotSpec means return to lib default (att name in this case)
//	"" (empty string) means returns to user def if any otherwise return to lib default
//  NaN means returns to class default (if any), user default (if any), lib default
//

	bool store_db = false;
	bool remove_db = false;

    Tango::DeviceClass *dev_class = get_att_device_class(d_name);

	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);

	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();
	vector<AttrProperty> &def_class_prop = att.get_class_properties();
	size_t nb_class = def_class_prop.size();

	if(TG_strcasecmp(new_label,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(new_label,LabelNotSpec) == 0) ||
			(TG_strcasecmp(new_label,name.c_str()) == 0))
	{
		// force library defaults (even if user defaults defined)
		label = name;
		remove_db = true;
	}
	else if (TG_strcasecmp(new_label,NotANumber) == 0)
	{
		// set class default value if defined, otherwise use the user default or library defaults

		bool found = prop_in_list("label",label,nb_class,def_class_prop);
		if (found == false)
		{
            found = prop_in_list("label",label,nb_user,def_user_prop);
            remove_db = true;
            if (found == false)
            {
                label = name;
			}
		}
	}
	else if (strlen(new_label) == 0)
	{

		// set user default value if defined, otherwise use the library defaults

        bool found = prop_in_list("label",label,nb_user,def_user_prop);
        remove_db = true;
        if (found == false)
        {
            label = name;
		}
	}
	else
	{
		// set property
		label = new_label;
		store_db = true;
	}

//
// Update label in RootAttRegistry
//

	RootAttRegistry &rar = Util::instance()->get_root_att_reg();
	rar.update_label(fwd_dev_name,fwd_att_name,label);

//
// Store new value in DB
//

	if (store_db == true)
	{
		DbDatum db_prop(name);
		DbDatum db_lab("label");

		db_prop << 1;
		db_lab << new_label;

		DbData db_dat;
		db_dat.push_back(db_prop);
		db_dat.push_back(db_lab);

		try
		{
			Util::instance()->get_database()->put_device_attribute_property(d_name,db_dat);
		}
		catch (Tango::DevFailed &e)
		{
			label = old_label;
			rar.update_label(fwd_dev_name,fwd_att_name,label);
			throw;
		}
	}

//
// Remove value from DB because label is now the default one (class, user or lib)
//

	if (remove_db == true)
	{
		DbData db_data;
		db_data.push_back(DbDatum(name));
		db_data.push_back(DbDatum("label"));

		try
		{
			Util::instance()->get_database()->delete_device_attribute_property(d_name, db_data);
		}
		catch (Tango::DevFailed &e)
		{
			label = old_label;
			rar.update_label(fwd_dev_name,fwd_att_name,label);
			throw;
		}
	}

}


} // End of Tango namespace
