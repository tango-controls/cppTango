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
#include <fwdattribute.h>
#include <fwdattribute.tpp>

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

FwdAttributePrivate::FwdAttributePrivate(std::vector<AttrProperty> &prop_list,Attr &tmp_attr,std::string &dev_name,long idx)
:WAttributePrivate(prop_list,tmp_attr,dev_name,idx)
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

FwdAttributePrivate::~FwdAttributePrivate()
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

void FwdAttributePrivate::set_att_config(const Tango::AttributeConfig_5 &conf)
{
	description = conf.description.in();
	unit = conf.unit.in();
	standard_unit = conf.standard_unit.in();
	display_unit = conf.display_unit.in();
	format = conf.format.in();

	min_value_str = conf.min_value.in();
	max_value_str = conf.max_value.in();

	disp_level = conf.level;

//
// Enum labels (in case of)
//

	enum_labels.clear();
	size_t enum_labels_nb = conf.enum_labels.length();
	for (size_t loop = 0;loop < enum_labels_nb;loop++)
		enum_labels.push_back(conf.enum_labels[loop].in());

//
// min alarm
//

	if (TG_strcasecmp(conf.att_alarm.min_alarm,AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(conf.att_alarm.min_alarm,NotANumber) == 0)
	{
		min_alarm_str = AlrmValueNotSpec;
		alarm_conf.reset(min_level);
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			min_alarm_str = conf.att_alarm.min_alarm.in();
			convert_prop_value("min_alarm",min_alarm_str,min_alarm,d_name);
			alarm_conf.set(min_level);
		}
	}

//
// max alarm
//

	if (TG_strcasecmp(conf.att_alarm.max_alarm,AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(conf.att_alarm.max_alarm,NotANumber) == 0)
	{
		max_alarm_str = AlrmValueNotSpec;
		alarm_conf.reset(max_level);
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			max_alarm_str = conf.att_alarm.max_alarm.in();
			convert_prop_value("max_alarm",max_alarm_str,max_alarm,d_name);
			alarm_conf.set(max_level);
		}
	}

//
// min warning
//

	if (TG_strcasecmp(conf.att_alarm.min_warning,AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(conf.att_alarm.min_warning,NotANumber) == 0)
	{
		alarm_conf.reset(min_warn);
		min_warning_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			min_warning_str = conf.att_alarm.min_warning.in();
			convert_prop_value("min_warning",min_warning_str,min_warning,d_name);
			alarm_conf.set(min_warn);
		}
	}

//
// max warning
//

	if (TG_strcasecmp(conf.att_alarm.max_warning,AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(conf.att_alarm.max_warning,NotANumber) == 0)
	{
		alarm_conf.reset(max_warn);
		max_warning_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			max_warning_str = conf.att_alarm.max_warning.in();
			convert_prop_value("max_warning",max_warning_str,max_warning,d_name);
			alarm_conf.set(max_warn);
		}
	}

//
// delta_val
//

	bool delta_val_defined = false;
	if (TG_strcasecmp(conf.att_alarm.delta_val,AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(conf.att_alarm.delta_val,NotANumber) == 0)
			delta_val_str = AlrmValueNotSpec;
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			delta_val_str = conf.att_alarm.delta_val.in();
			convert_prop_value("delta_val",delta_val_str,delta_val,d_name);
			delta_val_defined = true;
		}
	}

//
// delta t
//

	bool delta_t_defined = true;
	std::stringstream ss;
	std::string tmp_prop = conf.att_alarm.delta_t.in();
	if (tmp_prop == AlrmValueNotSpec || tmp_prop == NotANumber)
	{
		delta_t_str = "0";
		delta_t = 0;
	}
	else
	{
		ss << tmp_prop;
		ss >> delta_t;
		delta_t_str = tmp_prop;
		delta_t_defined = true;
	}

	if (delta_t_defined == true && delta_val_defined == true)
		alarm_conf.set(rds);
	else
		alarm_conf.reset(rds);

//
// Set event related properties (not stored as strings)
//

	tmp_prop = conf.event_prop.ch_event.rel_change.in();
	convert_event_prop(tmp_prop,rel_change);

	tmp_prop = conf.event_prop.ch_event.abs_change.in();
	convert_event_prop(tmp_prop,abs_change);

	ss.str("");
	ss.clear();

	tmp_prop = conf.event_prop.per_event.period.in();
	if (tmp_prop == AlrmValueNotSpec)
		event_period = DEFAULT_EVENT_PERIOD;
	else
	{
		ss << tmp_prop;
		ss >> event_period;
	}

	tmp_prop = conf.event_prop.arch_event.rel_change.in();
	convert_event_prop(tmp_prop,archive_rel_change);

	tmp_prop = conf.event_prop.arch_event.abs_change.in();
	convert_event_prop(tmp_prop,archive_abs_change);

	ss.str("");
	ss.clear();

	tmp_prop = conf.event_prop.arch_event.period.in();
	if (tmp_prop == AlrmValueNotSpec)
		archive_period = INT_MAX;
	else
	{
		ss << tmp_prop;
		ss >> archive_period;
	}
}

//
// Same method than previous but the argument is not the same and this one is called when a forwarded
// attribute configuration is written via a call in the Forwarded attribute callback in the case of
// root attribute not available when the DS was started
//

void FwdAttributePrivate::set_att_config(AttributeInfoEx *aie_ptr)
{
	data_format = aie_ptr->data_format;
	writable = aie_ptr->writable;
	data_type = aie_ptr->data_type;
	disp_level = aie_ptr->disp_level;
	enum_labels = aie_ptr->enum_labels;

	switch (aie_ptr->memorized)
	{
	case NOT_KNOWN:
	case NONE:
		set_memorized(false);
		set_memorized_init(false);
		break;

	case MEMORIZED:
		set_memorized(true);
		set_memorized_init(false);
		break;

	case MEMORIZED_WRITE_INIT:
		set_memorized(true);
		set_memorized_init(true);
		break;

	default:
		break;
	}

	writable_attr_name = aie_ptr->writable_attr_name;
	if (writable == READ_WRITE)
		writable_attr_name = name;

	max_x = aie_ptr->max_dim_x;
	max_y = aie_ptr->max_dim_y;

	description = aie_ptr->description;
	unit = aie_ptr->unit;
	standard_unit = aie_ptr->standard_unit;
	display_unit = aie_ptr->display_unit;
	format = aie_ptr->format;

	min_value_str = aie_ptr->min_value;
	max_value_str = aie_ptr->max_value;

	disp_level = aie_ptr->disp_level;

//
// min alarm
//

	if (TG_strcasecmp(aie_ptr->alarms.min_alarm.c_str(),AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(aie_ptr->alarms.min_alarm.c_str(),NotANumber) == 0)
	{
		alarm_conf.reset(min_level);
		min_alarm_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			min_alarm_str = aie_ptr->alarms.min_alarm;
			convert_prop_value("min_alarm",min_alarm_str,min_alarm,d_name);
			alarm_conf.set(min_level);
		}
	}

//
// max alarm
//

	if (TG_strcasecmp(aie_ptr->alarms.max_alarm.c_str(),AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(aie_ptr->alarms.max_alarm.c_str(),NotANumber) == 0)
	{
		alarm_conf.reset(max_level);
		max_alarm_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			max_alarm_str = aie_ptr->alarms.max_alarm;
			convert_prop_value("max_alarm",max_alarm_str,max_alarm,d_name);
			alarm_conf.set(max_level);
		}
	}

//
// min warning
//

	if (TG_strcasecmp(aie_ptr->alarms.min_warning.c_str(),AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(aie_ptr->alarms.min_warning.c_str(),NotANumber) == 0)
	{
		alarm_conf.reset(min_warn);
		min_warning_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			min_warning_str = aie_ptr->alarms.min_warning;
			convert_prop_value("min_warning",min_warning_str,min_warning,d_name);
			alarm_conf.set(min_warn);
		}
	}

//
// max warning
//

	if (TG_strcasecmp(aie_ptr->alarms.max_warning.c_str(),AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(aie_ptr->alarms.max_warning.c_str(),NotANumber) == 0)
	{
		alarm_conf.reset(max_warn);
		max_warning_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			max_warning_str = aie_ptr->alarms.max_warning;
			convert_prop_value("max_warning",max_warning_str,max_warning,d_name);
			alarm_conf.set(max_warn);
		}
	}

//
// delta val
//

	bool delta_val_defined = false;

	if (TG_strcasecmp(aie_ptr->alarms.delta_val.c_str(),AlrmValueNotSpec) == 0 ||
		TG_strcasecmp(aie_ptr->alarms.delta_val.c_str(),NotANumber) == 0)
	{
		delta_val_str = AlrmValueNotSpec;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			delta_val_str = aie_ptr->alarms.delta_val;
			convert_prop_value("delta_val",delta_val_str,delta_val,d_name);
			delta_val_defined = true;
		}
	}

//
// delta t
//

	bool delta_t_defined = false;
	std::string tmp_prop = aie_ptr->alarms.delta_t;
	if (tmp_prop == AlrmValueNotSpec)
	{
		delta_t_str = "0";
		delta_t = 0;
	}
	else
	{
		std::stringstream ss1;
		ss1 << tmp_prop;
		ss1 >> delta_t;
		delta_t_str = tmp_prop;
		delta_t_defined = true;
	}

	if (delta_t_defined == true && delta_val_defined == true)
		alarm_conf.set(rds);
	else
		alarm_conf.reset(rds);

//
// Set event related properties (not stored as strings)
//

	std::stringstream ss;

	tmp_prop = aie_ptr->events.ch_event.rel_change;
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

void FwdAttributePrivate::convert_event_prop(std::string &prop_str,double *ptr)
{
	std::stringstream ss;

	if (prop_str != AlrmValueNotSpec)
	{
		std::string::size_type pos = prop_str.find(',');
		if (pos == std::string::npos)
		{
			double tmp_db;
			ss << prop_str;
			ss >> tmp_db;
			ptr[0] = tmp_db;
			ptr[1] = tmp_db;
		}
		else
		{
			std::string first = prop_str.substr(0,pos);
			std::string second = prop_str.substr(pos + 1);

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
//		FwdAttribute::upd_att_config_base
//
// description :
//
//
// argument :
//		in :
//			- conf : The attribute new configuration
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttributePrivate::upd_att_config_base(const char *new_label)
{
	cout4 << "Entering FwdAttribute::upd_att_config_base" << std::endl;

//
// Throw exception in case of fwd att wrongly configured or if the root device is not yet accessible
//

	if (get_data_type() == DATA_TYPE_UNKNOWN)
	{
		std::string desc("Attribute ");
		desc = desc + name + " is a forwarded attribute and its root device (";
		desc = desc + fwd_dev_name;
		desc = desc + ") is not yet available";
		Tango::Except::throw_exception(API_AttrConfig,desc,"FwdAttribute::upd_att_config_base");
	}

//
// A new label (the only att property stored locally)
//

	if (std::string(new_label) != label)
		upd_att_label(new_label);

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

void FwdAttributePrivate::upd_att_config(const Tango::AttributeConfig_5 &conf)
{

//
// Send new config to root attribute if received configuration if different than the one we already have
//

	if (new_att_conf(nullptr,&conf) == true)
	{
		AttributeInfoListEx aile;
		AttributeInfoEx aie;
		aie = &conf;
		aie.label = get_fwd_att_name();

		RootAttRegistry &rar = Util::instance()->get_root_att_reg();
		DeviceProxy *dev = rar.get_root_att_dp(fwd_dev_name);

		aie.name = fwd_att_name;
		if (aie.writable_attr_name != AssocWritNotSpec)
			aie.writable_attr_name = fwd_att_name;
		aile.push_back(aie);

		cout4 << "Sending att conf to root device " << fwd_dev_name << std::endl;
		dev->set_attribute_config(aile);
	}
}

void FwdAttributePrivate::upd_att_config(const Tango::AttributeConfig_3 &conf)
{

//
// Send new config to root attribute if received configuration if different than the one we already have
//

	if (new_att_conf(&conf,nullptr) == true)
	{
		AttributeInfoListEx aile;
		AttributeInfoEx aie;
		aie = &conf;
		aie.label = get_fwd_att_name();

		RootAttRegistry &rar = Util::instance()->get_root_att_reg();
		DeviceProxy *dev = rar.get_root_att_dp(fwd_dev_name);

		aie.name = fwd_att_name;
		aile.push_back(aie);

		cout4 << "Sending att conf to root device " << fwd_dev_name << std::endl;
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

bool FwdAttributePrivate::new_att_conf(const Tango::AttributeConfig_3 *conf3,const Tango::AttributeConfig_5 *conf5)
{
	bool ret = false;

	if (conf3 != nullptr)
	{
		ret = new_att_conf_base(*conf3);
	}
	else
	{
		ret = new_att_conf_base(*conf5);
		if (ret == false)
		{
			if (conf5->memorized != is_memorized())
				ret = true;
			else if (conf5->memorized == true)
			{
				if (conf5->mem_init != is_memorized_init())
					ret = true;
			}
		}
	}

	return ret;
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

void FwdAttributePrivate::upd_att_label(const char *new_label)
{
	cout4 << "Entering FwdAttribute::upd_att_label() for att " << name << " with label set to " << new_label << std::endl;
	std::string old_label = label;

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

	std::vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();
	std::vector<AttrProperty> &def_class_prop = att.get_class_properties();
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

		db_prop << 1L;
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

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::read_root_att_history
//
// description :
//		Get attribute polling history from the root attribute.
//		Note that we do not use the classical API DeviceProxy class instance. This is because for this call, the
//		network transfer uses RLE algorithm and the API is doing the decoding. In our case, we want data from
//		root attribute in RLE and directly transfer them in RLE to our client.
//
// argument :
//		in :
//			- n : Required history depth
//
//--------------------------------------------------------------------------------------------------------------------

DevAttrHistory_5 *FwdAttributePrivate::read_root_att_history(long n)
{

//
// Get CORBA object reference
//

	RootAttRegistry &rar = Util::instance()->get_root_att_reg();
	DeviceProxy *dp = rar.get_root_att_dp(get_fwd_dev_name());
	Device_5_var dev = dp->get_device_5();

//
// Get data from root device (Reminder: we don't use the classical API. See above)
//

	DevAttrHistory_5 *hist_5 = nullptr;
	int ctr = 0;

	while (ctr < 2)
	{
		try
		{
			dp->check_and_reconnect();

			hist_5 = dev->read_attribute_history_5(get_fwd_att_name().c_str(),n);

			ctr = 2;
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"FwdAttribute","read_root_att_history",dp);
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch || one.minor() == 0)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"FwdAttribute","read_root_att_history",dp);
			}
			else
			{
				dp->set_connection_state(CONNECTION_NOTOK);
				TangoSys_OMemStream desc;
				desc << "Attribute_history failed on device " << get_fwd_dev_name() << std::ends;
				ApiCommExcept::re_throw_exception(one,"API_CommunicationFailed",
								         desc.str(),"FwdAttribute::read_root_att_history()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"FwdAttribute","read_root_att_history",dp);
			}
			else
			{
				dp->set_connection_state(CONNECTION_NOTOK);
				TangoSys_OMemStream desc;
				desc << "Attribute_history failed on device " << get_fwd_dev_name() << std::ends;
				ApiCommExcept::re_throw_exception(comm,"API_CommunicationFailed",
											desc.str(),"FwdAttribute::read_root_att_history()");
			}
		}
        catch (CORBA::SystemException &ce)
        {
			dp->set_connection_state(CONNECTION_NOTOK);
			TangoSys_OMemStream desc;
			desc << "Attribute_history failed on device " << get_fwd_dev_name() << std::ends;
			ApiCommExcept::re_throw_exception(ce,"API_CommunicationFailed",
                        			      desc.str(),"FwdAttribute::read_root_att_history()");
		}
	}

//
// Update attribute name in returned data
//

	hist_5->name = get_name().c_str();

	return hist_5;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::write_read_root_att
//
// description :
//		Get attribute polling history from the root attribute.
//		Note that we do not use the classical API DeviceProxy class instance. This is because for this call, the
//		network transfer uses RLE algorithm and the API is doing the decoding. In our case, we want data from
//		root attribute in RLE and directly transfer them in RLE to our client.
//
// argument :
//		in :
//			- n : Required history depth
//
//--------------------------------------------------------------------------------------------------------------------

AttributeValueList_5 *FwdAttributePrivate::write_read_root_att(Tango::AttributeValueList_4& values)
{
//
// Get CORBA object reference
//

	RootAttRegistry &rar = Util::instance()->get_root_att_reg();
	DeviceProxy *dp = rar.get_root_att_dp(get_fwd_dev_name());
	Device_5_var dev5 = dp->get_device_5();

//
// Update attribute name
//

	values[0].name = get_fwd_att_name().c_str();

	Tango::DevVarStringArray dvsa;
	dvsa.length(1);
	dvsa[0] = Tango::string_dup(get_fwd_att_name().c_str());

	int ctr = 0;
	AttributeValueList_5 *attr_value_list_5 = nullptr;
	Tango::AccessControlType local_act;

	while (ctr < 2)
	{
		try
		{
			dp->check_and_reconnect(local_act);

//
// Throw exception if caller not allowed to write_attribute
//

			if (local_act == ACCESS_READ)
			{
				try
				{
					Device_var dev = Device::_duplicate(dp->device);
					dev->ping();
				}
				catch(...)
				{
					dp->set_connection_state(CONNECTION_NOTOK);
					throw;
				}

				TangoSys_OMemStream desc;
				desc << "Writing attribute(s) on device " << get_fwd_dev_name() << " is not authorized" << std::ends;

				NotAllowedExcept::throw_exception((const char *)API_ReadOnlyMode,desc.str(),
											  	  (const char *)"FwdAttribute::write_read_root_att()");
			}

//
// Now, call the root device server
//

			ClntIdent ci;
			ApiUtil *au = ApiUtil::instance();
			ci.cpp_clnt(au->get_client_pid());

			attr_value_list_5 = dev5->write_read_attributes_5(values,dvsa,ci);

			ctr = 2;

		}
		catch (Tango::MultiDevFailed &e)
		{

//
// Transfer this exception into a DevFailed exception
//

			Tango::DevFailed ex(e.errors[0].err_list);
			TangoSys_OMemStream desc;
			desc << "Failed to write_read_attribute on device " << get_fwd_dev_name();
			desc << ", attribute ";
			desc << values[0].name.in();
			desc << std::ends;
			Except::re_throw_exception(ex,(const char*)API_AttributeFailed,
                        	desc.str(), (const char*)"FwdAttribute::write_read_root_att()");

		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to write_read_attribute on device " << get_fwd_dev_name();
			desc << ", attribute ";
			desc << values[0].name.in();
			desc << std::ends;

			if (::strcmp(e.errors[0].reason,DEVICE_UNLOCKED_REASON) == 0)
				DeviceUnlockedExcept::re_throw_exception(e,(const char*)DEVICE_UNLOCKED_REASON,
							desc.str(), (const char*)"FwdAttribute::write_read_root_att()");
			else
                Except::re_throw_exception(e,(const char*)API_AttributeFailed,
                        	desc.str(), (const char*)"FwdAttribute::write_read_root_att()");
		}
		catch (CORBA::TRANSIENT &trans)
		{
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"FwdAttribute","write_read_root_att()",dp);
		}
		catch (CORBA::OBJECT_NOT_EXIST &one)
		{
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch || one.minor() == 0)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(one,"FwdAttribute","write_read_root_att",dp);
			}
			else
			{
				dp->set_connection_state(CONNECTION_NOTOK);
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_read_attribute on device " << get_fwd_dev_name() << std::ends;
				ApiCommExcept::re_throw_exception(one,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"FwdAttribute::write_read_root_att()");
			}
		}
		catch (CORBA::COMM_FAILURE &comm)
		{
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply)
			{
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"FwdAttribute","write_read_root_att",dp);
			}
			else
			{
				dp->set_connection_state(CONNECTION_NOTOK);
				TangoSys_OMemStream desc;
				desc << "Failed to execute write_attribute on device " << get_fwd_dev_name() << std::ends;
				ApiCommExcept::re_throw_exception(comm,
							      (const char*)"API_CommunicationFailed",
                        				      desc.str(),
							      (const char*)"FwdAttribute::write_read_root_att");
			}
		}
        catch (CORBA::SystemException &ce)
        {
			dp->set_connection_state(CONNECTION_NOTOK);

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes on device " << get_fwd_dev_name() << std::ends;
			ApiCommExcept::re_throw_exception(ce,
						      (const char*)"API_CommunicationFailed",
                        			      desc.str(),
						      (const char*)"FwdAttribute::write_read_root_att()");
		}
	}

//
// Init the returned DeviceAttribute instance
//

	(*attr_value_list_5)[0].name = get_name().c_str();
	return attr_value_list_5;
}

} // End of Tango namespace
