static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               Attribute.cpp
//
// description :        C++ source code for the Attribute and WAttribute classes.
//			These classes are used to manage attribute.
//			A Tango Device object instance has one
//			MultiAttribute object which is an aggregate of
//			Attribute or WAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//-============================================================================

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

RANGES_TYPE2CONST(Tango::DevDouble,DEV_DOUBLE)
RANGES_TYPE2CONST(Tango::DevFloat,DEV_FLOAT)
RANGES_TYPE2CONST(Tango::DevLong,DEV_LONG)
RANGES_TYPE2CONST(Tango::DevLong64,DEV_LONG64)
RANGES_TYPE2CONST(Tango::DevShort,DEV_SHORT)
RANGES_TYPE2CONST(Tango::DevUChar,DEV_UCHAR)
RANGES_TYPE2CONST(Tango::DevULong,DEV_ULONG)
RANGES_TYPE2CONST(Tango::DevULong64,DEV_ULONG64)
RANGES_TYPE2CONST(Tango::DevUShort,DEV_USHORT)

//
// A classical function which will be used as unary predicate for the find_if
// algo. It must be used with the bind2nd adapter to be transform as unary and
// with the ptr_fun adapter to be transform as a function object
//

static bool WantedProp_f(AttrProperty a,const char *n)
{
	return (a.get_name() == n);
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::Attribute
//
// description : 	constructor for the Attribute class from the
//			attribute property vector, its type and the device
//			name
//
// argument : in : 	- prop_list : The attribute property list
//			- type : The attribute data type
//			- dev_name : The device name
//
//--------------------------------------------------------------------------

Attribute::Attribute(vector<AttrProperty> &prop_list,
		     Attr &tmp_attr,string &dev_name,long idx)
:date(true),quality(Tango::ATTR_VALID),check_min_value(false),check_max_value(false)
{

//
// Create the extension class and init some of its members
//

	ext = new Attribute::AttributeExt();
	ext->idx_in_attr = idx;
	ext->d_name = dev_name;
	ext->dev = NULL;

	ext->change_event_implmented = false;
	ext->check_change_event_criteria = true;
	ext->archive_event_implmented = false;
	ext->check_archive_event_criteria = true;
	ext->dr_event_implmented = false;

	ext->event_periodic_client_3 = false;
	ext->event_change_client_3 = false;
	ext->event_archive_client_3 = false;
	ext->event_user_client_3 = false;

	ext->attr_serial_model = ATTR_BY_KERNEL;
	ext->user_attr_mutex = NULL;

	ext->scalar_str_attr_release = false;

//
// Init the attribute name
//

	name = tmp_attr.get_name();
	name_size = name.size();
	name_lower = name;
	transform(name_lower.begin(),name_lower.end(),name_lower.begin(),::tolower);

//
// Clear alarm data
//

	alarm_conf.reset();
	alarm.reset();

//
// Init the remaining attribute main characteristic
//

	data_type = tmp_attr.get_type();
	writable = tmp_attr.get_writable();
	data_format = tmp_attr.get_format();
	ext->disp_level = tmp_attr.get_disp_level();
	ext->poll_period = tmp_attr.get_polling_period();
	writable_attr_name = tmp_attr.get_assoc();

//
// Init the event characteristics
//

	ext->change_event_implmented  		= tmp_attr.is_change_event();
	ext->check_change_event_criteria 	= tmp_attr.is_check_change_criteria();
	ext->archive_event_implmented 		= tmp_attr.is_archive_event();
	ext->check_archive_event_criteria 	= tmp_attr.is_check_archive_criteria();
	ext->dr_event_implmented			= tmp_attr.is_data_ready_event();

	switch(data_format)
	{
	case Tango::SPECTRUM:
		max_x = static_cast<SpectrumAttr &>(tmp_attr).get_max_x();
		max_y = 0;
		dim_y = 0;
		break;

	case Tango::IMAGE:
		max_x = static_cast<ImageAttr &>(tmp_attr).get_max_x();
		max_y = static_cast<ImageAttr &>(tmp_attr).get_max_y();
		break;

	default :
		max_x = 1;
		max_y = 0;
		dim_x = 1;
		dim_y = 0;
	}

//
// Initialise optional properties
//

	init_opt_prop(prop_list,dev_name);

//
// Initialise event related fields
//

	init_event_prop(prop_list,dev_name,tmp_attr);

}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::init_event_prop
//
// description : 	Init the event related properties
//
// in :			prop_list : The property vector
//
//--------------------------------------------------------------------------

void Attribute::init_event_prop(vector<AttrProperty> &prop_list,const string &dev_name,Attr &att)
{
	ext->rel_change[0] = INT_MAX;			// default for relative change is none
	ext->rel_change[1] = INT_MAX;			// default for relative change is none
	ext->abs_change[0] = INT_MAX;			// default for absolute change is none
	ext->abs_change[1] = INT_MAX;			// default for absolute change is none
	ext->archive_rel_change[0] = INT_MAX;		// default for archive change is none
	ext->archive_rel_change[1] = INT_MAX;		// default for archive change is none
	ext->archive_abs_change[0] = INT_MAX;		// default for archive change is none
	ext->archive_abs_change[1] = INT_MAX;		// default for archive change is none

    ext->notifd_event = false;
	ext->zmq_event = false;

	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

//
// Init min and max relative change for change event
//

	string rel_change_str;
	bool rel_change_defined = false;
	try
	{
        rel_change_str = get_attr_value(prop_list,"rel_change");
        rel_change_defined = true;
	}
	catch (...) {}

	if(rel_change_defined)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			vector<double> rel_change;
			vector<bool> rel_change_set_usr_def;
			validate_change_properties(dev_name,"rel_change",rel_change_str,rel_change,rel_change_set_usr_def);
			ext->rel_change[0] = rel_change[0];
			ext->rel_change[1] = rel_change[1];

			if(rel_change_set_usr_def[0] || rel_change_set_usr_def[1])
			{
				if (nb_user != 0)
				{
					size_t i;
					for (i = 0;i < nb_user;i++)
					{
						if (def_user_prop[i].get_name() == "rel_change")
							break;
					}
					if (i != nb_user)
					{
						vector<double> rel_change_usr_def;
						validate_change_properties(dev_name,"rel_change",def_user_prop[i].get_value(),rel_change_usr_def);
						if(rel_change_set_usr_def[0])
							ext->rel_change[0] = rel_change_usr_def[0];
						if(rel_change_set_usr_def[1])
							ext->rel_change[1] = rel_change_usr_def[1];
					}
				}
			}
			cout1 << "Attribute::Attribute(): rel_change = " << ext->rel_change[0] << " " << ext->rel_change[1] << endl;
		}
		else
			throw_err_data_type("rel_change",dev_name,"Attribute::init_event_prop");
	}

//
// Init min and max absolute change for change event
//

	string abs_change_str;
	bool abs_change_defined = false;
	try
	{
		abs_change_str = get_attr_value(prop_list,"abs_change");
		abs_change_defined = true;
	}
	catch (...) {}

	if(abs_change_defined)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			vector<double> abs_change;
			vector<bool> abs_change_set_usr_def;
			validate_change_properties(dev_name,"abs_change",abs_change_str,abs_change,abs_change_set_usr_def);
			ext->abs_change[0] = abs_change[0];
			ext->abs_change[1] = abs_change[1];

			if(abs_change_set_usr_def[0] || abs_change_set_usr_def[1])
			{
				if (nb_user != 0)
				{
					size_t i;
					for (i = 0;i < nb_user;i++)
					{
						if (def_user_prop[i].get_name() == "abs_change")
							break;
					}
					if (i != nb_user)
					{
						vector<double> abs_change_usr_def;
						validate_change_properties(dev_name,"abs_change",def_user_prop[i].get_value(),abs_change_usr_def);
						if(abs_change_set_usr_def[0])
							ext->abs_change[0] = abs_change_usr_def[0];
						if(abs_change_set_usr_def[1])
							ext->abs_change[1] = abs_change_usr_def[1];
					}
				}
			}
			cout1 << "Attribute::Attribute(): abs_change = " << ext->abs_change[0] << " " << ext->abs_change[1] << endl;
		}
		else
			throw_err_data_type("abs_change",dev_name,"Attribute::init_event_prop");
	}

//
// Init min and max relative change for archive event
//

	string archive_rel_change_str;
	bool archive_rel_change_defined = false;
	try
	{
		archive_rel_change_str = get_attr_value(prop_list,"archive_rel_change");
		archive_rel_change_defined = true;
	}
	catch (...) {}

	if(archive_rel_change_defined)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			vector<double> archive_rel_change;
			vector<bool> archive_rel_change_set_usr_def;
			validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_str,archive_rel_change,archive_rel_change_set_usr_def);
			ext->archive_rel_change[0] = archive_rel_change[0];
			ext->archive_rel_change[1] = archive_rel_change[1];

			if(archive_rel_change_set_usr_def[0] || archive_rel_change_set_usr_def[1])
			{
				if (nb_user != 0)
				{
					size_t i;
					for (i = 0;i < nb_user;i++)
					{
						if (def_user_prop[i].get_name() == "archive_rel_change")
							break;
					}
					if (i != nb_user)
					{
						vector<double> archive_rel_change_usr_def;
						validate_change_properties(dev_name,"archive_rel_change",def_user_prop[i].get_value(),archive_rel_change_usr_def);
						if(archive_rel_change_set_usr_def[0])
							ext->archive_rel_change[0] = archive_rel_change_usr_def[0];
						if(archive_rel_change_set_usr_def[1])
							ext->archive_rel_change[1] = archive_rel_change_usr_def[1];
					}
				}
			}
			cout1 << "Attribute::Attribute(): archive_rel_change = " << ext->archive_rel_change[0] << " " << ext->archive_rel_change[1] << endl;
		}
		else
			throw_err_data_type("archive_rel_change",dev_name,"Attribute::init_event_prop");
	}

//
// Init min and max absolute change for archive event
//

	string archive_abs_change_str;
	bool archive_abs_change_defined = false;
	try
	{
		archive_abs_change_str = get_attr_value(prop_list,"archive_abs_change");
		archive_abs_change_defined = true;
	}
	catch (...) {}

	if(archive_abs_change_defined)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			vector<double> archive_abs_change;
			vector<bool> archive_abs_change_set_usr_def;
			validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_str,archive_abs_change,archive_abs_change_set_usr_def);
			ext->archive_abs_change[0] = archive_abs_change[0];
			ext->archive_abs_change[1] = archive_abs_change[1];

			if(archive_abs_change_set_usr_def[0] || archive_abs_change_set_usr_def[1])
			{
				if (nb_user != 0)
				{
					size_t i;
					for (i = 0;i < nb_user;i++)
					{
						if (def_user_prop[i].get_name() == "archive_abs_change")
							break;
					}
					if (i != nb_user)
					{
						vector<double> archive_abs_change_usr_def;
						validate_change_properties(dev_name,"archive_abs_change",def_user_prop[i].get_value(),archive_abs_change_usr_def);
						if(archive_abs_change_set_usr_def[0])
							ext->archive_abs_change[0] = archive_abs_change_usr_def[0];
						if(archive_abs_change_set_usr_def[1])
							ext->archive_abs_change[1] = archive_abs_change_usr_def[1];
					}
				}
			}
			cout1 << "Attribute::Attribute(): archive_abs_change = " << ext->archive_abs_change[0] << " " << ext->archive_abs_change[1] << endl;
		}
		else
			throw_err_data_type("archive_abs_change",dev_name,"Attribute::init_event_prop");
	}


//
// Init period for periodic event
//

	ext->event_period = (int)(DEFAULT_EVENT_PERIOD);  	// default for event period is 1 second

	string event_period_str;
	bool event_period_defined = false;
	try
	{
    	event_period_str = get_attr_value(prop_list,"event_period");
    	event_period_defined = true;
	}
	catch (...) { }

	if(event_period_defined)
	{
		TangoSys_MemStream str;
		int event_period = 0;
		str << event_period_str;
		if(str >> event_period && str.eof())
		{
			if (event_period > 0)
				ext->event_period = event_period;
			cout1 << "Attribute::Attribute(): event_period_str " << event_period_str << " event_period = " << ext->event_period << endl;
		}
		else
			throw_err_format("event_period",dev_name,"Attribute::init_event_prop");
	}

//
// Init period for archive event
//

	ext->archive_period = (int)(INT_MAX);

	string archive_period_str;
	bool archive_period_defined = false;
	try
	{
		archive_period_str = get_attr_value(prop_list,"archive_period");
		archive_period_defined = true;
	}
	catch (...) { }

	if(archive_period_defined)
	{
		TangoSys_MemStream str;
		int archive_period = 0;
		str << archive_period_str;
		if(str >> archive_period && str.eof())
		{
			if (archive_period > 0)
			{
				ext->archive_period = archive_period;
			}
			cout1 << "Attribute::Attribute(): archive_period_str " << archive_period_str << " archive_period = " << ext->archive_period << endl;
		}
		else
			throw_err_format("archive_period",dev_name,"Attribute::init_event_prop");
	}

//
// Init remaining parameters
//

	ext->periodic_counter = 0;
	ext->archive_periodic_counter = 0;
	ext->last_periodic = 0.;
	ext->archive_last_periodic = 0.;
	ext->prev_change_event.inited = false;
	ext->prev_quality_event.inited = false;
	ext->prev_archive_event.inited = false;

//
// do not start sending events automatically, wait for the first
// client to subscribe. Sending events automatically will put an
// unnecessary load on the server because all attributes will be polled
//

	ext->event_change_subscription = 0;
	ext->event_archive_subscription = 0;
	ext->event_quality_subscription = 0;
	ext->event_periodic_subscription = 0;
	ext->event_user_subscription = 0;
	ext->event_attr_conf_subscription = 0;
	ext->event_data_ready_subscription = 0;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::init_opt_prop
//
// description : 	Init the optional properties
//
// in :			prop_list : The property vector
//			dev_name : The device name (usefull for error)
//
//--------------------------------------------------------------------------

void Attribute::init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name)
{

//
// Init the label property
//

	label = get_attr_value(prop_list,"label");

//
// Init the description property
//

	description = get_attr_value(prop_list,"description");

//
// Init the unit property
//

	unit = get_attr_value(prop_list,"unit");

//
// Init the standard unit property
//

	standard_unit = get_attr_value(prop_list,"standard_unit");

//
// Init the display unit property
//

	display_unit = get_attr_value(prop_list,"display_unit");

//
// Init the format property
//

	format = get_attr_value(prop_list,"format");


	TangoSys_MemStream str;
	bool empty = true;

//
// Init the min alarm property
//

	min_alarm_str = get_attr_value(prop_list,"min_alarm");
	if (min_alarm_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			str << min_alarm_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_alarm.sh && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_level) && min_alarm.sh >= max_alarm.sh)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				min_alarm.lg = (DevLong)min_alarm.db;
				if(alarm_conf.test(max_level) && min_alarm.lg >= max_alarm.lg)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG64:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				min_alarm.lg64 = (DevLong64)min_alarm.db;
				if(alarm_conf.test(max_level) && min_alarm.lg64 >= max_alarm.lg64)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_level) && min_alarm.db >= max_alarm.db)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;
			case Tango::DEV_FLOAT:
				if (!(str >> min_alarm.fl && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_level) && min_alarm.fl >= max_alarm.fl)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> min_alarm.ush && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_level) && min_alarm.ush >= max_alarm.ush)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> min_alarm.sh && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				min_alarm.uch = (unsigned char)min_alarm.sh;
				if(alarm_conf.test(max_level) && min_alarm.uch >= max_alarm.uch)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				min_alarm.ulg = (DevULong)min_alarm.db;
				if(alarm_conf.test(max_level) && min_alarm.ulg >= max_alarm.ulg)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				min_alarm.ulg64 = (DevULong64)min_alarm.db;
				if(alarm_conf.test(max_level) && min_alarm.ulg64 >= max_alarm.ulg64)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> min_alarm.sh && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop");
				min_alarm.uch = (unsigned char)min_alarm.sh;
				if(alarm_conf.test(max_level) && min_alarm.uch >= max_alarm.uch)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;
			}
			alarm_conf.set(min_level);
		}
		else
			throw_err_data_type("min_alarm",dev_name,"Attribute::init_opt_prop");
	}

//
// Init the max alarm property
//

	max_alarm_str = get_attr_value(prop_list,"max_alarm");
	if (max_alarm_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.str("");
				str.clear();
			}

			str << max_alarm_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_alarm.sh && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_level) && min_alarm.sh >= max_alarm.sh)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				max_alarm.lg = (DevLong)max_alarm.db;
				if(alarm_conf.test(min_level) && min_alarm.lg >= max_alarm.lg)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				max_alarm.lg64 = (DevLong64)max_alarm.db;
				if(alarm_conf.test(min_level) && min_alarm.lg64 >= max_alarm.lg64)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_level) && min_alarm.db >= max_alarm.db)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> max_alarm.fl && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_level) && min_alarm.fl >= max_alarm.fl)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> max_alarm.ush && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_level) && min_alarm.ush >= max_alarm.ush)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> max_alarm.sh && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				max_alarm.uch = (unsigned char)max_alarm.sh;
				if(alarm_conf.test(min_level) && min_alarm.uch >= max_alarm.uch)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				max_alarm.ulg = (DevULong)max_alarm.db;
				if(alarm_conf.test(min_level) && min_alarm.ulg >= max_alarm.ulg)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				max_alarm.ulg64 = (DevULong64)max_alarm.db;
				if(alarm_conf.test(min_level) && min_alarm.ulg64 >= max_alarm.ulg64)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> max_alarm.sh && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop");
				max_alarm.uch = (unsigned char)max_alarm.sh;
				if(alarm_conf.test(min_level) && min_alarm.uch >= max_alarm.uch)
					throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop");
				break;
			}
			alarm_conf.set(max_level);
		}
		else
			throw_err_data_type("max_alarm",dev_name,"Attribute::init_opt_prop");
	}

//
// Init the min value property
//

	min_value_str = get_attr_value(prop_list,"min_value");
	if (min_value_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.str("");
				str.clear();
			}

			str << min_value_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_value.sh && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				if(check_max_value && min_value.sh >= max_value.sh)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				min_value.lg = (DevLong)min_value.db;
				if(check_max_value && min_value.lg >= max_value.lg)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG64:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				min_value.lg64 = (DevLong64)min_value.db;
				if(check_max_value && min_value.lg64 >= max_value.lg64)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				if(check_max_value && min_value.db >= max_value.db)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> min_value.fl && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				if(check_max_value && min_value.fl >= max_value.fl)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> min_value.ush && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				if(check_max_value && min_value.ush >= max_value.ush)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> min_value.sh && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				min_value.uch = (unsigned char)min_value.sh;
				if(check_max_value && min_value.uch >= max_value.uch)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				min_value.ulg = (DevULong)min_value.db;
				if(check_max_value && min_value.ulg >= max_value.ulg)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				min_value.ulg64 = (DevULong64)min_value.db;
				if(check_max_value && min_value.ulg64 >= max_value.ulg64)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> min_value.sh && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::init_opt_prop");
				min_value.uch = (unsigned char)min_value.sh;
				if(check_max_value && min_value.uch >= max_value.uch)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;
			}
			check_min_value = true;
		}
		else
			throw_err_data_type("min_value",dev_name,"Attribute::init_opt_prop");
	}

//
// Init the max value property
//

	max_value_str = get_attr_value(prop_list,"max_value");
	if (max_value_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.str("");
				str.clear();
			}

			str << max_value_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_value.sh && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				if(check_min_value && min_value.sh >= max_value.sh)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				max_value.lg = (DevLong)max_value.db;
				if(check_min_value && min_value.lg >= max_value.lg)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				max_value.lg64 = (DevLong64)max_value.db;
				if(check_min_value && min_value.lg64 >= max_value.lg64)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				if(check_min_value && min_value.db >= max_value.db)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> max_value.fl && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				if(check_min_value && min_value.fl >= max_value.fl)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> max_value.ush && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				if(check_min_value && min_value.ush >= max_value.ush)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> max_value.sh && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				max_value.uch = (unsigned char)max_value.sh;
				if(check_min_value && min_value.uch >= max_value.uch)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				max_value.ulg = (DevULong)max_value.db;
				if(check_min_value && min_value.ulg >= max_value.ulg)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				max_value.ulg64 = (DevULong64)max_value.db;
				if(check_min_value && min_value.ulg64 >= max_value.ulg64)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> max_value.sh && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::init_opt_prop");
				max_value.uch = (unsigned char)max_value.sh;
				if(check_min_value && min_value.uch >= max_value.uch)
					throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop");
				break;
			}
			check_max_value = true;
		}
		else
			throw_err_data_type("max_value",dev_name,"Attribute::init_opt_prop");
	}

//
// Init the min warning property
//

	min_warning_str = get_attr_value(prop_list,"min_warning");
	if (min_warning_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.str("");
				str.clear();
			}

			str << min_warning_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_warning.sh && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_warn) && min_warning.sh >= max_warning.sh)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				min_warning.lg = (DevLong)min_warning.db;
				if(alarm_conf.test(max_warn) && min_warning.lg >= max_warning.lg)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG64:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				min_warning.lg64 = (DevLong64)min_warning.db;
				if(alarm_conf.test(max_warn) && min_warning.lg64 >= max_warning.lg64)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_warn) && min_warning.db >= max_warning.db)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;
			case Tango::DEV_FLOAT:
				if (!(str >> min_warning.fl && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_warn) && min_warning.fl >= max_warning.fl)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> min_warning.ush && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(max_warn) && min_warning.ush >= max_warning.ush)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> min_warning.sh && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				min_warning.uch = (unsigned char)min_warning.sh;
				if(alarm_conf.test(max_warn) && min_warning.uch >= max_warning.uch)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				min_warning.ulg = (DevULong)min_warning.db;
				if(alarm_conf.test(max_warn) && min_warning.ulg >= max_warning.ulg)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				min_warning.ulg64 = (DevULong64)min_warning.db;
				if(alarm_conf.test(max_warn) && min_warning.ulg64 >= max_warning.ulg64)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> min_warning.sh && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop");
				min_warning.uch = (unsigned char)min_warning.sh;
				if(alarm_conf.test(max_warn) && min_warning.uch >= max_warning.uch)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;
			}
			alarm_conf.set(min_warn);
		}
		else
			throw_err_data_type("min_warning",dev_name,"Attribute::init_opt_prop");
	}

//
// Init the max warning property
//

	max_warning_str = get_attr_value(prop_list,"max_warning");
	if (max_warning_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.str("");
				str.clear();
			}

			str << max_warning_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_warning.sh && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_warn) && min_warning.sh >= max_warning.sh)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				max_warning.lg = (DevLong)max_warning.db;
				if(alarm_conf.test(min_warn) && min_warning.lg >= max_warning.lg)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				max_warning.lg64 = (DevLong64)max_warning.db;
				if(alarm_conf.test(min_warn) && min_warning.lg64 >= max_warning.lg64)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_warn) && min_warning.db >= max_warning.db)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> max_warning.fl && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_warn) && min_warning.fl >= max_warning.fl)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> max_warning.ush && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				if(alarm_conf.test(min_warn) && min_warning.ush >= max_warning.ush)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> max_warning.sh && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				max_warning.uch = (unsigned char)max_warning.sh;
				if(alarm_conf.test(min_warn) && min_warning.uch >= max_warning.uch)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				max_warning.ulg = (DevULong)max_warning.db;
				if(alarm_conf.test(min_warn) && min_warning.ulg >= max_warning.ulg)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				max_warning.ulg64 = (DevULong64)max_warning.db;
				if(alarm_conf.test(min_warn) && min_warning.ulg64 >= max_warning.ulg64)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> max_warning.sh && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop");
				max_warning.uch = (unsigned char)max_warning.sh;
				if(alarm_conf.test(min_warn) && min_warning.uch >= max_warning.uch)
					throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop");
				break;
			}
			alarm_conf.set(max_warn);
		}
		else
			throw_err_data_type("max_warning",dev_name,"Attribute::init_opt_prop");
	}

//
// Get delta_t property
//

	delta_t_str = get_attr_value(prop_list,"delta_t");
	bool delta_t_defined = false;
	if (delta_t_str != "0")
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			delta_t = get_lg_attr_value(prop_list,"delta_t");
			if (delta_t != 0)
				delta_t_defined = true;
		}
		else
			throw_err_data_type("delta_t",dev_name,"Attribute::init_opt_prop");
	}
	else
		delta_t =  0;

//
// Get delta_val property
//

	delta_val_str = get_attr_value(prop_list,"delta_val");
	bool delta_val_defined = false;
	if (delta_val_str != AlrmValueNotSpec)
	{
		if((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.str("");
				str.clear();
			}

			str << delta_val_str;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> delta_val.sh && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_LONG:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				delta_val.lg = (DevLong)delta_val.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				delta_val.lg64 = (DevLong64)delta_val.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> delta_val.fl && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> delta_val.ush && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> delta_val.sh && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				delta_val.uch = (unsigned char)delta_val.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				delta_val.ulg = (DevULong)delta_val.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				delta_val.ulg64 = (DevULong64)delta_val.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> delta_val.sh && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop");
				delta_val.uch = (unsigned char)delta_val.sh;
				break;
			}
			if(delta_t_defined)
				alarm_conf.set(rds); // set RDS flag only if both delta_t and delta_val are set
			delta_val_defined = true;
		}
		else
			throw_err_data_type("delta_val",dev_name,"Attribute::init_opt_prop");
	}

//
// Throw exception if only one the RDS property is defined
//

	if (((delta_t_defined == true) && (delta_val_defined == false)) ||
	    ((delta_t_defined == false) && (delta_val_defined == true)))
	{
		TangoSys_OMemStream o;

		o << "RDS alarm properties (delta_t and delta_val) are not correctly defined for attribute " << name;
		o << " in device " << dev_name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::init_opt_prop");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::throw_err_format
//
// description : 	Throw a Tango DevFailed exception when an error format
//			is detected in the string which should be converted
//			to a number
//
// in :			prop_name : The property name
//			dev_name : The device name
//			origin : The origin of the exception
//
//--------------------------------------------------------------------------

void Attribute::throw_err_format(const char *prop_name,const string &dev_name,const char *origin)
{
	TangoSys_OMemStream o;

	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nThe property " << prop_name << " is defined in an unsupported format" << ends;
	Except::throw_exception((const char *)"API_AttrOptProp",
			      o.str(),
			      (const char *)origin);
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::throw_incoherent_val_err
//
// description : 	Throw a Tango DevFailed exception when the min or max
//				property is incoherent with its counterpart
//
// in :			min_prop : The min property name
//			max_prop : The max property name
//			dev_name : The device name
//			origin : The origin of the exception
//
//--------------------------------------------------------------------------

void Attribute::throw_incoherent_val_err(const char *min_prop,const char *max_prop,const string &dev_name,const char *origin)
{
	TangoSys_OMemStream o;

	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nValue of " << min_prop << " is grater than or equal to " << max_prop << ends;
	Except::throw_exception((const char *)"API_IncoherentValues",
			      o.str(),
			      (const char *)origin);
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::throw_err_data_type
//
// description : 	Throw a Tango DevFailed exception when an error on
//					data type is detected
//
// in :		prop_name : The property name
//			dev_name : The device name
//			origin : The origin of the exception
//
//--------------------------------------------------------------------------

void Attribute::throw_err_data_type(const char *prop_name,const string &dev_name,const char *origin)
{
	TangoSys_OMemStream o;

	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nThe property " << prop_name << " is not settable for the attribute data type" << ends;
	Except::throw_exception((const char *)"API_AttrOptProp",
			      o.str(),
			      (const char *)origin);
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::throw_min_max_value
//
// description : 	Throw a Tango DevFailed exception when an error on
//					min/max value is detected
//
// in :	dev_name : The device name
//      memorized_value : The attribute memorized value
//      check_type : The type of check which was done (min_value or max_value)
//
//--------------------------------------------------------------------------

void Attribute::throw_min_max_value(string &dev_name,string &memorized_value,MinMaxValueCheck check_type)
{
	TangoSys_OMemStream o;

	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nThis attribute is memorized and the memorized value (" << memorized_value << ") is ";
	if (check_type == MIN)
        o << "below";
    else
        o << "above";
    o << " the new limit!!" << ends;
	Except::throw_exception((const char *)"API_AttrOptProp",
			      o.str(),
			      (const char *)"Attribute::throw_min_max_value");
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::is_polled
//
// description : 	Check if the attribute polled
//
// This method returns a boolean set to true if the attribute is polled
//
//--------------------------------------------------------------------------

bool Attribute::is_polled()
{
    Tango::Util *tg = Util::instance();
    if ( ext->dev == NULL )
    {
        ext->dev = tg->get_device_by_name(ext->d_name);
    }

	string &att_name = get_name_lower();

	vector<string> &attr_list = ext->dev->get_polled_attr();

	for (unsigned int i = 0;i < attr_list.size();i = i+2)
	{

//
//	Convert to lower case before comparison
//

		string	name_lowercase(attr_list[i]);
		transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
		if ( att_name == name_lowercase )
		{

//
// when the polling buffer is externally filled (polling period == 0)
// mark the attribute as not polled! No events can be send by the polling thread!
//

			if ( attr_list[i+1] == "0" )
			{
				return false;
			}
			else
				return true;
		}
	}

//
// now check wether a polling period is set ( for example by pogo)
//

	if ( get_polling_period() > 0 )
	{

//
// check the list of non_auto_polled attributes to verify wether
// the polling was disabled
//

		vector<string> &napa = ext->dev->get_non_auto_polled_attr();
		for (unsigned int j = 0;j < napa.size();j++)
		{
#ifdef _TG_WINDOWS_
			if (_stricmp(napa[j].c_str(), att_name.c_str()) == 0)
#else
			if (strcasecmp(napa[j].c_str(), att_name.c_str()) == 0)
#endif
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool Attribute::is_polled(DeviceImpl *the_dev)
{
    if ((the_dev != NULL) && (ext->dev == NULL))
    {
        ext->dev = the_dev;
    }

    return is_polled();
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::is_writ_associated
//
// description : 	Check if the attribute has an associated writable
//			attribute
//
// This method returns a boolean set to true if the atribute has an associatied
// writable attribute
//
//--------------------------------------------------------------------------

bool Attribute::is_writ_associated()
{
	if (writable_attr_name != AssocWritNotSpec)
		return true;
	else
		return false;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_attr_value
//
// description : 	Retrieve a property value as a string from the vector
//			of properties
//
// in :			prop_list : The property vector
//			prop_name : the property name
//
//--------------------------------------------------------------------------


string &Attribute::get_attr_value(vector <AttrProperty> &prop_list,const char *prop_name)
{
	vector<AttrProperty>::iterator pos;

//
// Get the property value
//

	pos = find_if(prop_list.begin(),prop_list.end(),bind2nd(ptr_fun(WantedProp_f),prop_name));
	if (pos == prop_list.end())
	{
		TangoSys_OMemStream o;

		o << "Property " << prop_name << " is missing for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::get_attr_value");
	}

	return pos->get_value();
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_lg_attr_value
//
// description : 	Retrieve a property value as a long from the vector
//			of properties
//
// in :			prop_list : The property vector
//			prop_name : the property name
//
//--------------------------------------------------------------------------


long Attribute::get_lg_attr_value(vector <AttrProperty> &prop_list,const char *prop_name)
{
	vector<AttrProperty>::iterator pos;

//
// Get the property value
//

	pos = find_if(prop_list.begin(),prop_list.end(),bind2nd(ptr_fun(WantedProp_f),prop_name));
	if (pos == prop_list.end())
	{
		TangoSys_OMemStream o;

		o << "Property " << prop_name << " is missing for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::get_attr_value");
	}

	pos->convert();
	return pos->get_lg_value();
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_properties
//
// description : 	Init the Tango::AttributeConfig with all the attribute
//			properties value
//
//--------------------------------------------------------------------------

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

	conf.level = ext->disp_level;
}

void Attribute::get_properties_3(Tango::AttributeConfig_3 &conf)
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
	conf.min_value = CORBA::string_dup(min_value_str.c_str());
	conf.max_value = CORBA::string_dup(max_value_str.c_str());
	conf.level = ext->disp_level;

//
// Copy alarm properties
//

	conf.att_alarm.min_alarm = CORBA::string_dup(min_alarm_str.c_str());
	conf.att_alarm.max_alarm = CORBA::string_dup(max_alarm_str.c_str());
	conf.att_alarm.min_warning = CORBA::string_dup(min_warning_str.c_str());;
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

	if (ext->event_period == INT_MAX)
		conf.event_prop.per_event.period = CORBA::string_dup((const char *)(DEFAULT_EVENT_PERIOD));
	else
	{
		int per = (int)((double)ext->event_period);
		str << per;
		MEM_STREAM_2_CORBA(conf.event_prop.per_event.period,str);
	}

//
// Copy change event properties
//

	if (fabs(ext->rel_change[0]) == fabs(ext->rel_change[1]))
	{
		if (ext->rel_change[0] == INT_MAX)
			conf.event_prop.ch_event.rel_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(ext->rel_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.rel_change,str);
		}
	}
	else
	{
		if (ext->rel_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(ext->rel_change[0]) << ",";

		if (ext->rel_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(ext->rel_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.ch_event.rel_change,str);
	}

	if (fabs(ext->abs_change[0]) == fabs(ext->abs_change[1]))
	{
		if (ext->abs_change[0] == INT_MAX)
			conf.event_prop.ch_event.abs_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(ext->abs_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.abs_change,str);
		}
	}
	else
	{
		if (ext->abs_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(ext->abs_change[0]) << ",";

		if (ext->abs_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(ext->abs_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.ch_event.abs_change,str);
	}

//
// Copy archive event properties
//

	if (ext->archive_period == INT_MAX)
		conf.event_prop.arch_event.period = CORBA::string_dup(AlrmValueNotSpec);
	else
	{
		int per = (int)((double)ext->archive_period);
		str << per;
		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.period,str);
	}

	if (fabs(ext->archive_rel_change[0]) == fabs(ext->archive_rel_change[1]))
	{
		if (ext->archive_rel_change[0] == INT_MAX)
			conf.event_prop.arch_event.rel_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(ext->archive_rel_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.rel_change,str);
		}
	}
	else
	{
		if (ext->archive_rel_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(ext->archive_rel_change[0]) << ",";

		if (ext->archive_rel_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(ext->archive_rel_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.rel_change,str);
	}

	if (fabs(ext->archive_abs_change[0]) == fabs(ext->archive_abs_change[1]))
	{
		if (ext->archive_abs_change[0] == INT_MAX)
			conf.event_prop.arch_event.abs_change = CORBA::string_dup(AlrmValueNotSpec);
		else
		{
			str << fabs(ext->archive_abs_change[1]);
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.abs_change,str);
		}
	}
	else
	{
		if (ext->archive_abs_change[0] == INT_MAX)
			str << AlrmValueNotSpec << ",";
		else
			str << fabs(ext->archive_abs_change[0]) << ",";

		if (ext->archive_abs_change[1] == INT_MAX)
			str << AlrmValueNotSpec;
		else
			str << fabs(ext->archive_abs_change[1]);

		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.abs_change,str);
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_properties
//
// description : 	Init the Tango::AttributeConfig with all the attribute
//			properties value
//
//--------------------------------------------------------------------------

void Attribute::set_properties(const Tango::AttributeConfig &conf,Tango::DeviceImpl *d)
{
	if (ext->dev == NULL)
		ext->dev = d;

	set_properties(conf,d->get_name());
}

void Attribute::set_properties(const Tango::AttributeConfig &conf,string &dev_name)
{

//
// Check if the caller try to change "hard coded" properties
// Throw exception in case of
//

    check_hard_coded_properties(conf);

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string means returns to its
// default value which could be the library default value or the
// user defined default value
//

	Tango::DeviceImpl *dev = get_att_device();
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

//
// First the description
//

	if(TG_strcasecmp(conf.description,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.description,DescNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		description = DescNotSpec;
	}
	else if((TG_strcasecmp(conf.description,NotANumber) == 0) ||
			(strlen(conf.description) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "description")
					break;
			}
			if (i == nb_user)
				description = DescNotSpec;
			else
				description = def_user_prop[i].get_value();
		}
		else
			description = DescNotSpec;
	}
	else
	{
		// set property
		description = conf.description;
	}

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
	else if((TG_strcasecmp(conf.label,NotANumber) == 0) ||
			(strlen(conf.label) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "label")
					break;
			}
			if (i == nb_user)
				label = name.c_str();
			else
				label = def_user_prop[i].get_value();
		}
		else
			label = name.c_str();
	}
	else
	{
		// set property
		label = conf.label;
	}

//
// The unit
//

	if(TG_strcasecmp(conf.unit,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.unit,UnitNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		unit = UnitNotSpec;
	}
	else if((TG_strcasecmp(conf.unit,NotANumber) == 0) ||
			(strlen(conf.unit) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "unit")
					break;
			}
			if (i == nb_user)
				unit = UnitNotSpec;
			else
				unit = def_user_prop[i].get_value();
		}
		else
			unit = UnitNotSpec;
	}
	else
	{
		// set property
		unit = conf.unit;
	}

//
// The standard unit
//

	if(TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		standard_unit = StdUnitNotSpec;
	}
	else if((TG_strcasecmp(conf.standard_unit,NotANumber) == 0) ||
			(strlen(conf.standard_unit) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "standard_unit")
					break;
			}
			if (i == nb_user)
				standard_unit = StdUnitNotSpec;
			else
				standard_unit = def_user_prop[i].get_value();
		}
		else
			standard_unit = StdUnitNotSpec;
	}
	else
	{
		// set property
		standard_unit = conf.standard_unit;
	}

//
// The display unit
//

	if(TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.display_unit,DispUnitNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		display_unit = DispUnitNotSpec;
	}
	else if((TG_strcasecmp(conf.display_unit,NotANumber) == 0) ||
			(strlen(conf.display_unit) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "display_unit")
					break;
			}
			if (i == nb_user)
				display_unit = DispUnitNotSpec;
			else
				display_unit = def_user_prop[i].get_value();
		}
		else
			display_unit = DispUnitNotSpec;
	}
	else
	{
		// set property
		display_unit = conf.display_unit;
	}

//
// The format
//

	if(TG_strcasecmp(conf.format,AlrmValueNotSpec) == 0 ||
			(TG_strcasecmp(conf.format,FormatNotSpec) == 0))
	{
		// force library defaults (even if user defaults defined)
		format = FormatNotSpec;
	}
	else if((TG_strcasecmp(conf.format,NotANumber) == 0) ||
			(strlen(conf.format) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "format")
					break;
			}
			if (i == nb_user)
				format = FormatNotSpec;
			else
				format = def_user_prop[i].get_value();
		}
		else
			format = FormatNotSpec;
	}
	else
	{
		// set property
		format = conf.format;
	}

//
// For the last four properties, convert their value to the right type
//

	TangoSys_MemStream str;

	if(TG_strcasecmp(conf.min_value,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		min_value_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.min_value,NotANumber) == 0) ||
			(strlen(conf.min_value) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "min_value")
					break;
			}
			if (i == nb_user)
				min_value_str = AlrmValueNotSpec;
			else
				min_value_str = def_user_prop[i].get_value();
		}
		else
			min_value_str = AlrmValueNotSpec;
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
			str << min_value_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_value.sh && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				min_value.lg = (DevLong)min_value.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				min_value.lg64 = (DevLong64)min_value.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> min_value.fl && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> min_value.ush && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> min_value.sh && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				min_value.uch = (unsigned char)min_value.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				min_value.ulg = (DevULong)min_value.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> min_value.db && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				min_value.ulg64 = (DevULong64)min_value.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> min_value.sh && str.eof()))
					throw_err_format("min_value",dev_name,"Attribute::set_properties");
				min_value.uch = (unsigned char)min_value.sh;
				break;
			}
			check_min_value = true;

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new
// min_value is not above the already memorized value
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
			throw_err_data_type("min_value",dev_name,"Attribute::set_properties");
	}


	if(TG_strcasecmp(conf.max_value,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		max_value_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.max_value,NotANumber) == 0) ||
			(strlen(conf.max_value) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "max_value")
					break;
			}
			if (i == nb_user)
				max_value_str = AlrmValueNotSpec;
			else
				max_value_str = def_user_prop[i].get_value();
		}
		else
			max_value_str = AlrmValueNotSpec;
	}
	else
	{
		// set property
		max_value_str = conf.max_value;
	}

	if (max_value_str == AlrmValueNotSpec)
	{
		check_max_value = false;
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.str("");
			str.clear();

			str << max_value_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_value.sh && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				max_value.lg = (DevLong)max_value.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				max_value.lg64 = (DevLong64)max_value.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> max_value.fl && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> max_value.ush && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> max_value.sh && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				max_value.uch = (unsigned char)max_value.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				max_value.ulg = (DevULong)max_value.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> max_value.db && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				max_value.ulg64 = (DevULong64)max_value.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> max_value.sh && str.eof()))
					throw_err_format("max_value",dev_name,"Attribute::set_properties");
				max_value.uch = (unsigned char)max_value.sh;
				break;
			}
			check_max_value = true;

//
// If the attribute is READ_WRITE or WRITE and memorized, check that the new
// max_value is not below the already memorized value
//

            Tango::AttrWriteType w_type = att.get_writable();
            if ((w_type == Tango::READ_WRITE) || (w_type == Tango::WRITE))
            {
                WAttribute *w_att = static_cast<WAttribute *>(this);
                string m_value;
                if ((w_att->is_memorized() == true) && (w_att->mem_value_below_above(MAX,m_value) == true))
                    throw_min_max_value(dev_name,m_value,MAX);
            }
		}
		else
			throw_err_data_type("max_value",dev_name,"Attribute::set_properties");
	}


	if(TG_strcasecmp(conf.min_alarm,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		min_alarm_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.min_alarm,NotANumber) == 0) ||
			(strlen(conf.min_alarm) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "min_alarm")
					break;
			}
			if (i == nb_user)
				min_alarm_str = AlrmValueNotSpec;
			else
				min_alarm_str = def_user_prop[i].get_value();
		}
		else
			min_alarm_str = AlrmValueNotSpec;
	}
	else
	{
		// set property
		min_alarm_str = conf.min_alarm;
	}

	if (min_alarm_str == AlrmValueNotSpec)
	{
		alarm_conf.reset(min_level);
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.str("");
			str.clear();

			str << min_alarm_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_alarm.sh && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				min_alarm.lg = (DevLong)min_alarm.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				min_alarm.lg64 = (DevLong64)min_alarm.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> min_alarm.fl && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> min_alarm.ush && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> min_alarm.sh && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				min_alarm.uch = (unsigned char)min_alarm.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				min_alarm.ulg = (DevULong)min_alarm.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> min_alarm.db && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				min_alarm.ulg64 = (DevULong64)min_alarm.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> min_alarm.sh && str.eof()))
					throw_err_format("min_alarm",dev_name,"Attribute::set_properties");
				min_alarm.uch = (unsigned char)min_alarm.sh;
				break;
			}
			alarm_conf.set(min_level);
		}
		else
			throw_err_data_type("min_alarm",dev_name,"Attribute::set_properties");
	}


	if(TG_strcasecmp(conf.max_alarm,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		max_alarm_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.max_alarm,NotANumber) == 0) ||
			(strlen(conf.max_alarm) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "max_alarm")
					break;
			}
			if (i == nb_user)
				max_alarm_str = AlrmValueNotSpec;
			else
				max_alarm_str = def_user_prop[i].get_value();
		}
		else
			max_alarm_str = AlrmValueNotSpec;
	}
	else
	{
		// set property
		max_alarm_str = conf.max_alarm;
	}

	if (max_alarm_str == AlrmValueNotSpec)
	{
		alarm_conf.reset(max_level);
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.str("");
			str.clear();

			str << max_alarm_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_alarm.sh && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				max_alarm.lg = (DevLong)max_alarm.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				max_alarm.lg64 = (DevLong64)max_alarm.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> max_alarm.fl && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> max_alarm.ush && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> max_alarm.sh && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				max_alarm.uch = (unsigned char)max_alarm.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				max_alarm.ulg = (DevULong)max_alarm.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> max_alarm.db && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				max_alarm.ulg64 = (DevULong64)max_alarm.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> max_alarm.sh && str.eof()))
					throw_err_format("max_alarm",dev_name,"Attribute::set_properties");
				max_alarm.uch = (unsigned char)max_alarm.sh;
				break;
			}
			alarm_conf.set(max_level);
		}
		else
			throw_err_data_type("max_alarm",dev_name,"Attribute::set_properties");
	}

}


void Attribute::set_properties(const Tango::AttributeConfig_3 &conf)
{
	set_properties(conf,ext->d_name);
}

void Attribute::set_properties(const Tango::AttributeConfig_3 &conf,Tango::DeviceImpl *d)
{
	if (ext->dev == NULL)
		ext->dev = d;

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
// Add a check of the display level property because it is not
// checked by the check_hard_coded_properties() template method
// called by the set_properties() method.
// Display level is available only in AttributeConfig_3
//

    check_hard_coded_properties(conf);

    if (conf.level != get_disp_level())
    {
        throw_hard_coded_prop("level");
    }

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string or NotANumber mean return to its
// default value which could be the library default value or the
// user defined default value if present. AlrmValueNotSpec brings the library
// default values regardless of the user defined defaults.
//

	Tango::DeviceImpl *dev = get_att_device();
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

//
// The min_warning case
//

	TangoSys_MemStream str;

	if(TG_strcasecmp(conf.att_alarm.min_warning,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		min_warning_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.att_alarm.min_warning,NotANumber) == 0) ||
			(strlen(conf.att_alarm.min_warning) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "min_warning")
					break;
			}
			if (i == nb_user)
				min_warning_str = AlrmValueNotSpec;
			else
				min_warning_str = def_user_prop[i].get_value();
		}
		else
			min_warning_str = AlrmValueNotSpec;
	}
	else
	{
		// set property
		min_warning_str = conf.att_alarm.min_warning;
	}

	if (min_warning_str == AlrmValueNotSpec)
	{
		alarm_conf.reset(min_warn);
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str << min_warning_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_warning.sh && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				min_warning.lg = (DevLong)min_warning.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				min_warning.lg64 = (DevLong64)min_warning.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> min_warning.fl && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> min_warning.ush && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> min_warning.sh && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				min_warning.uch = (unsigned char)min_warning.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				min_warning.ulg = (DevULong)min_warning.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> min_warning.db && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				min_warning.ulg64 = (DevULong64)min_warning.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> min_warning.sh && str.eof()))
					throw_err_format("min_warning",dev_name,"Attribute::set_properties");
				min_warning.uch = (unsigned char)min_warning.sh;
				break;
			}
			alarm_conf.set(min_warn);
		}
		else
			throw_err_data_type("min_warning",dev_name,"Attribute::set_properties");
	}

//
// Now, ladies and gentleman, the max warning case
//

	if(TG_strcasecmp(conf.att_alarm.max_warning,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		max_warning_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.att_alarm.max_warning,NotANumber) == 0) ||
			(strlen(conf.att_alarm.max_warning) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "max_warning")
					break;
			}
			if (i == nb_user)
				max_warning_str = AlrmValueNotSpec;
			else
				max_warning_str = def_user_prop[i].get_value();
		}
		else
			max_warning_str = AlrmValueNotSpec;
	}
	else
	{
		// set property
		max_warning_str = conf.att_alarm.max_warning;
	}

	if (max_warning_str == AlrmValueNotSpec)
	{
		alarm_conf.reset(max_warn);
	}
	else
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.str("");
			str.clear();

			str << max_warning_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_warning.sh && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				max_warning.lg = (DevLong)max_warning.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				max_warning.lg64 = (DevLong64)max_warning.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> max_warning.fl && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> max_warning.ush && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> max_warning.sh && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				max_warning.uch = (unsigned char)max_warning.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				max_warning.ulg = (DevULong)max_warning.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> max_warning.db && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				max_warning.ulg64 = (DevULong64)max_warning.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> max_warning.sh && str.eof()))
					throw_err_format("max_warning",dev_name,"Attribute::set_properties");
				max_warning.uch = (unsigned char)max_warning.sh;
				break;
			}
			alarm_conf.set(max_warn);
		}
		else
			throw_err_data_type("max_warning",dev_name,"Attribute::set_properties");
	}

//
// Then, the delta_val
//

	bool delta_val_defined = false;
	if(TG_strcasecmp(conf.att_alarm.delta_val,AlrmValueNotSpec) == 0)
	{
		// force library defaults (even if user defaults defined)
		delta_val_str = AlrmValueNotSpec;
	}
	else if((TG_strcasecmp(conf.att_alarm.delta_val,NotANumber) == 0) ||
			(strlen(conf.att_alarm.delta_val) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "delta_val")
					break;
			}
			if (i == nb_user)
				delta_val_str = AlrmValueNotSpec;
			else
				delta_val_str = def_user_prop[i].get_value();
		}
		else
			delta_val_str = AlrmValueNotSpec;
	}
	else
	{
		// set property
		delta_val_str = conf.att_alarm.delta_val;
	}

	if(delta_val_str != AlrmValueNotSpec)
	{
		if ((data_type != Tango::DEV_STRING) &&
			(data_type != Tango::DEV_BOOLEAN) &&
			(data_type != Tango::DEV_STATE))
		{
			str.str("");
			str.clear();

			str << delta_val_str;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> delta_val.sh && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_LONG:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				delta_val.lg = (DevLong)delta_val.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				delta_val.lg64 = (DevLong64)delta_val.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> delta_val.fl && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_USHORT:
				if (!(str >> delta_val.ush && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> delta_val.sh && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				delta_val.uch = (unsigned char)delta_val.sh;
				break;

			case Tango::DEV_ULONG:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				delta_val.ulg = (DevULong)delta_val.db;
				break;

			case Tango::DEV_ULONG64:
				if (!(str >> delta_val.db && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				delta_val.ulg64 = (DevULong64)delta_val.db;
				break;

			case Tango::DEV_ENCODED:
				if (!(str >> delta_val.sh && str.eof()))
					throw_err_format("delta_val",dev_name,"Attribute::set_properties");
				delta_val.uch = (unsigned char)delta_val.sh;
				break;
			}
			delta_val_defined = true;
		}
		else
			throw_err_data_type("delta_val",dev_name,"Attribute::set_properties");
	}

//
// And the delta_t
//

	bool delta_t_defined = false;
	if(TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(conf.att_alarm.delta_t,"0") == 0)
	{
		// force library defaults (even if user defaults defined)
		delta_t_str = "0";
	}
	else if((TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0) ||
			(strlen(conf.att_alarm.delta_t) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "delta_t")
					break;
			}
			if (i == nb_user)
				delta_t_str = "0";
			else
			{
				delta_t_str = def_user_prop[i].get_value();
			}
		}
		else
			delta_t_str = "0";
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
			if (!(str >> delta_t && str.eof()))
				throw_err_format("delta_t",dev_name,"Attribute::set_properties");
			for (unsigned int loop = 0;loop < delta_t_str.size();loop++)
			{
				if (isdigit(delta_t_str[loop]) == 0)
					throw_err_format("delta_t",dev_name,"Attribute::set_properties");
			}
			delta_t_defined = true;
		}
		else
			throw_err_data_type("delta_t",dev_name,"Attribute::set_properties");
	}
	else
		delta_t = 0;

//
// Set RDS alarm flag only if both delta_t and delta_val are defined
//

	if(delta_t_defined && delta_val_defined)
		alarm_conf.set(rds);
	else if(!delta_t_defined && !delta_val_defined)
		alarm_conf.reset(rds);
	else {

//
// Set device if not already done
//

		if (ext->dev == NULL)
		{
			Tango::Util *tg = Tango::Util::instance();
			ext->dev = tg->get_device_by_name(ext->d_name);
		}

		if (ext->dev->get_logger()->is_warn_enabled())
				ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) incoherent (only one of the properties delta_t or delta_val is set) in attribute " << name << endl;
	}


//
// Now, the 4 changes parameters (for change and archive events)
//

//
// Relative change
//

	string rel_change_str(conf.event_prop.ch_event.rel_change); // provided, comma separated min and/or max values for the property
	string rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> rel_change; // vector containing min and max values of the property
	vector<double> rel_change_usr; // vector containing user default min and max values of the property
	bool rel_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "rel_change")
				break;
		}
		if (i != nb_user)
		{
			rel_change_usr_str = def_user_prop[i].get_value();
			rel_change_usr_def = true;
		}
	}

	if(rel_change_usr_def)
		validate_change_properties(dev_name,"rel_change",rel_change_usr_str,rel_change_usr);

	validate_change_properties(dev_name,"rel_change",rel_change_str,rel_change,rel_change_set_usr_def);
	if(rel_change_usr_def)
	{
		(rel_change_set_usr_def[0]) ? ext->rel_change[0] = rel_change_usr[0] : ext->rel_change[0] = rel_change[0];
		(rel_change_set_usr_def[1]) ? ext->rel_change[1] = rel_change_usr[1] : ext->rel_change[1] = rel_change[1];
	}
	else
	{
		ext->rel_change[0] = rel_change[0];
		ext->rel_change[1] = rel_change[1];
	}

//
// Absolute change
//

	string abs_change_str(conf.event_prop.ch_event.abs_change); // provided, comma separated min and/or max values for the property
	string abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> abs_change; // vector containing min and max values of the property
	vector<double> abs_change_usr; // vector containing user default min and max values of the property
	bool abs_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "abs_change")
				break;
		}
		if (i != nb_user)
		{
			abs_change_usr_str = def_user_prop[i].get_value();
			abs_change_usr_def = true;
		}
	}

	if(abs_change_usr_def)
		validate_change_properties(dev_name,"abs_change",abs_change_usr_str,abs_change_usr);

	validate_change_properties(dev_name,"abs_change",abs_change_str,abs_change,abs_change_set_usr_def);
	if(abs_change_usr_def)
	{
		(abs_change_set_usr_def[0]) ? ext->abs_change[0] = abs_change_usr[0] : ext->abs_change[0] = abs_change[0];
		(abs_change_set_usr_def[1]) ? ext->abs_change[1] = abs_change_usr[1] : ext->abs_change[1] = abs_change[1];
	}
	else
	{
		ext->abs_change[0] = abs_change[0];
		ext->abs_change[1] = abs_change[1];
	}

//
// Archive relative change
//

	string archive_rel_change_str(conf.event_prop.arch_event.rel_change); // provided, comma separated min and/or max values for the property
	string archive_rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> archive_rel_change; // vector containing min and max values of the property
	vector<double> archive_rel_change_usr; // vector containing user default min and max values of the property
	bool archive_rel_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> archive_rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "archive_rel_change")
				break;
		}
		if (i != nb_user)
		{
			archive_rel_change_usr_str = def_user_prop[i].get_value();
			archive_rel_change_usr_def = true;
		}
	}

	if(archive_rel_change_usr_def)
		validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_usr_str,archive_rel_change_usr);

	validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_str,archive_rel_change,archive_rel_change_set_usr_def);
	if(rel_change_usr_def)
	{
		(archive_rel_change_set_usr_def[0]) ? ext->archive_rel_change[0] = archive_rel_change_usr[0] : ext->archive_rel_change[0] = archive_rel_change[0];
		(archive_rel_change_set_usr_def[1]) ? ext->archive_rel_change[1] = archive_rel_change_usr[1] : ext->archive_rel_change[1] = archive_rel_change[1];
	}
	else
	{
		ext->archive_rel_change[0] = archive_rel_change[0];
		ext->archive_rel_change[1] = archive_rel_change[1];
	}

//
// Archive absolute change
//

	string archive_abs_change_str(conf.event_prop.arch_event.abs_change); // provided, comma separated min and/or max values for the property
	string archive_abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> archive_abs_change; // vector containing min and max values of the property
	vector<double> archive_abs_change_usr; // vector containing user default min and max values of the property
	bool archive_abs_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> archive_abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "archive_abs_change")
				break;
		}
		if (i != nb_user)
		{
			archive_abs_change_usr_str = def_user_prop[i].get_value();
			archive_abs_change_usr_def = true;
		}
	}

	if(archive_abs_change_usr_def)
		validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_usr_str,archive_abs_change_usr);

	validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_str,archive_abs_change,archive_abs_change_set_usr_def);
	if(archive_abs_change_usr_def)
	{
		(archive_abs_change_set_usr_def[0]) ? ext->archive_abs_change[0] = archive_abs_change_usr[0] : ext->archive_abs_change[0] = archive_abs_change[0];
		(archive_abs_change_set_usr_def[1]) ? ext->archive_abs_change[1] = archive_abs_change_usr[1] : ext->archive_abs_change[1] = archive_abs_change[1];
	}
	else
	{
		ext->archive_abs_change[0] = archive_abs_change[0];
		ext->archive_abs_change[1] = archive_abs_change[1];
	}

//
// And finally, the last two event periods
//

//
// Event period
//

	TangoSys_MemStream def_event_period;
	def_event_period << (int)(DEFAULT_EVENT_PERIOD);
	if(TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(conf.event_prop.per_event.period,def_event_period.str().c_str()) == 0)
	{
		// force library defaults (even if user defaults defined)
		ext->event_period = DEFAULT_EVENT_PERIOD;
	}
	else if((TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0) ||
			(strlen(conf.event_prop.per_event.period) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "event_period")
					break;
			}
			if (i == nb_user)
				ext->event_period = DEFAULT_EVENT_PERIOD;
			else
			{
				str.str("");
				str.clear();
				str << def_user_prop[i].get_value();
				int tmp_period;
				if (!(str >> tmp_period && str.eof()))
					throw_err_format("event_period",dev_name,"Attribute::set_properties");
				ext->event_period = tmp_period;
			}
		}
		else
			ext->event_period = DEFAULT_EVENT_PERIOD;
	}
	else
	{
		// set property
		str.str("");
		str.clear();
		str << conf.event_prop.per_event.period;
		int tmp_period;
		if (!(str >> tmp_period && str.eof()))
			throw_err_format("event_period",dev_name,"Attribute::set_properties");
		ext->event_period = tmp_period;
	}

//
// Archive event period
//

	TangoSys_MemStream def_archive_period;
	def_archive_period << (int)(INT_MAX);
	if(TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0 ||
			TG_strcasecmp(conf.event_prop.arch_event.period,def_archive_period.str().c_str()) == 0)
	{
		// force library defaults (even if user defaults defined)
		ext->archive_period = INT_MAX;
	}
	else if((TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0) ||
			(strlen(conf.event_prop.arch_event.period) == 0))
	{
		// set user default value if defined, otherwise use the library defaults
		if (nb_user != 0)
		{
			size_t i;
			for (i = 0;i < nb_user;i++)
			{
				if (def_user_prop[i].get_name() == "archive_period")
					break;
			}
			if (i == nb_user)
				ext->archive_period = INT_MAX;
			else
			{
				str.str("");
				str.clear();
				str << def_user_prop[i].get_value();
				int tmp_period;
				if (!(str >> tmp_period && str.eof()))
					throw_err_format("archive_period",dev_name,"Attribute::set_properties");
				ext->archive_period = tmp_period;
			}
		}
		else
			ext->archive_period = INT_MAX;
	}
	else
	{
		// set property
		str.str("");
		str.clear();
		str << conf.event_prop.arch_event.period;
		int tmp_period;
		if (!(str >> tmp_period && str.eof()))
			throw_err_format("archive_period",dev_name,"Attribute::set_properties");
		ext->archive_period = tmp_period;
	}
}


void Attribute::set_upd_properties(const AttributeConfig_3 &conf)
{
	set_upd_properties(conf,ext->d_name);
}

void Attribute::set_upd_properties(const AttributeConfig_3 &conf,string &dev_name)
{

//
// Backup current configuration
//

	AttributeConfig_3 old_conf;
	get_properties_3(old_conf);

//
// Set properties locally. In case of exception bring the backed-up values
//

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
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_LONG:
					if(min_value.lg >= max_value.lg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_LONG64:
					if(min_value.lg64 >= max_value.lg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_DOUBLE:
					if(min_value.db >= max_value.db)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_FLOAT:
					if(min_value.fl >= max_value.fl)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_USHORT:
					if(min_value.ush >= max_value.ush)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_UCHAR:
					if(min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ULONG:
					if(min_value.ulg >= max_value.ulg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ULONG64:
					if(min_value.ulg64 >= max_value.ulg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ENCODED:
					if(min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::set_upd_properties");
					break;
				}
			}
			else
				throw_err_data_type("min_value",dev_name,"Attribute::set_upd_properties");
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
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_LONG:
					if(min_alarm.lg >= max_alarm.lg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_LONG64:
					if(min_alarm.lg64 >= max_alarm.lg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_DOUBLE:
					if(min_alarm.db >= max_alarm.db)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_FLOAT:
					if(min_alarm.fl >= max_alarm.fl)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_USHORT:
					if(min_alarm.ush >= max_alarm.ush)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_UCHAR:
					if(min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ULONG:
					if(min_alarm.ulg >= max_alarm.ulg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ULONG64:
					if(min_alarm.ulg64 >= max_alarm.ulg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ENCODED:
					if(min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::set_upd_properties");
					break;
				}
			}
			else
				throw_err_data_type("min_alarm",dev_name,"Attribute::set_upd_properties");
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
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_LONG:
					if(min_warning.lg >= max_warning.lg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_LONG64:
					if(min_warning.lg64 >= max_warning.lg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_DOUBLE:
					if(min_warning.db >= max_warning.db)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_FLOAT:
					if(min_warning.fl >= max_warning.fl)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_USHORT:
					if(min_warning.ush >= max_warning.ush)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_UCHAR:
					if(min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ULONG:
					if(min_warning.ulg >= max_warning.ulg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ULONG64:
					if(min_warning.ulg64 >= max_warning.ulg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;

				case Tango::DEV_ENCODED:
					if(min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::set_upd_properties");
					break;
				}
			}
			else
				throw_err_data_type("min_warning",dev_name,"Attribute::set_upd_properties");
		}

//
// Update database
//

		try
		{
			upd_database(conf,dev_name);
		}
		catch(DevFailed &db_except)
		{

//
// In case of exception, try to store old properties in the database and
// inform the user about the error
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
				Except::throw_exception((const char *)"API_CorruptedDatabase",
							  o.str(),
							  (const char *)"Attribute::set_upd_properties");
			}

			throw db_except;
		}
	}
	catch(DevFailed &except)
	{
		set_properties(old_conf,dev_name);
		throw except;
	}
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

//+-------------------------------------------------------------------------
//
// method : 		Attribute::upd_database
//
// description : 	Update the tango database with the new attribute
//			values
//
//--------------------------------------------------------------------------

void Attribute::upd_database(const Tango::AttributeConfig &conf,string &dev_name)
{
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
// Get the vector of user default properties to store the library defaults in
// the data base in case there are user defaults declared for the property
//

	Tango::DeviceImpl *dev = get_att_device();
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();

//
// Check if "string" properties must be updated in db and if
// needed insert a corresponding DbDatum object in the DbData
// vector
//

	check_str_prop(conf,db_d,prop_to_update,db_del,prop_to_delete,def_user_prop);

//
// For the last four, if the data type is not string, checks that the input
// strings are really number
//

	TangoSys_MemStream str;

	CHECK_PROP(conf.min_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_value",def_user_prop);

	CHECK_PROP(conf.max_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_value",def_user_prop);

	CHECK_PROP(conf.min_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_alarm",def_user_prop);

	CHECK_PROP(conf.max_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_alarm",def_user_prop);

//
// Update db only if needed
//

	if (prop_to_update != 0)
	{
		cout4 << prop_to_update << " properties to update in db" << endl;
		db_d[0] << prop_to_update;

		Tango::Util *tg = Tango::Util::instance();

//
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
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
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
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

void Attribute::upd_database(const Tango::AttributeConfig_3 &conf,string &dev_name)
{
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
// Get the vector of user default properties to store the library defaults in
// the data base in case there are user defaults declared for the property
//

	Tango::DeviceImpl *dev = get_att_device();
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();
	string usr_def_val;

//
// Check if "string" properties must be updated in db and if
// needed insert a corresponding DbDatum object in the DbData
// vector
//

	AttributeConfig tmp_conf;
	tmp_conf.description = conf.description;
	tmp_conf.label = conf.label;
	tmp_conf.unit = conf.unit;
	tmp_conf.standard_unit = conf.standard_unit;
	tmp_conf.display_unit = conf.display_unit;
	tmp_conf.format = conf.format;

	check_str_prop(tmp_conf,db_d,prop_to_update,db_del,prop_to_delete,def_user_prop);

//
// For the last two, if the data type is not string, checks that the input
// strings are really number
//

	TangoSys_MemStream str;

	CHECK_PROP(conf.min_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_value",def_user_prop);

	CHECK_PROP(conf.max_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_value",def_user_prop);

//
// Check for alarm related data
//

	CHECK_PROP(conf.att_alarm.min_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_alarm",def_user_prop);

	CHECK_PROP(conf.att_alarm.max_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_alarm",def_user_prop);

	CHECK_PROP(conf.att_alarm.min_warning,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_warning",def_user_prop);

	CHECK_PROP(conf.att_alarm.max_warning,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_warning",def_user_prop);

	CHECK_PROP(conf.att_alarm.delta_val,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"delta_val",def_user_prop);

	bool user_defaults = false;
	bool store_in_db = true;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "delta_t")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	if(user_defaults)
	{
		if ((TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0) ||
				(strcmp(conf.att_alarm.delta_t,usr_def_val.c_str()) == 0) ||
				(strlen(conf.att_alarm.delta_t) == 0))
			store_in_db = false;
	}
	else
	{
		if ((TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.att_alarm.delta_t,"0") == 0) ||
				(TG_strcasecmp(conf.att_alarm.delta_t,NotANumber) == 0) ||
				(strlen(conf.att_alarm.delta_t) == 0))
			store_in_db = false;
	}

	if(store_in_db)
	{
		const char *tmp = conf.att_alarm.delta_t.in();
		if(TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0)
			tmp = "0";
		else
		{
			if ((data_type != Tango::DEV_STRING) &&
			    (data_type != Tango::DEV_BOOLEAN) &&
				(data_type != Tango::DEV_STATE))
			{
				str.str("");
				str.clear();
				str << conf.att_alarm.delta_t;
				int lg;
				if (!(str >> lg && str.eof()))
					throw_err_format("delta_t",dev_name,"Attribute::upd_database");
				for (unsigned int loop = 0;loop < strlen(tmp);loop++)
				{
					if (isdigit(tmp[loop]) == 0)
						throw_err_format("delta_t",dev_name,"Attribute::upd_database");
				}
			}
			else
			{
				throw_err_data_type("delta_t",dev_name,"Attribute::set_upd_properties");
			}
		}
		DbDatum deltat("delta_t");
		deltat << tmp;
		db_d.push_back(deltat);
		prop_to_update++;
	}
	else
	{
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
	vector<double> rel_change; // vector containing min and max values of the property
	vector<double> rel_change_usr; // vector containing user default min and max values of the property
	bool rel_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "rel_change")
				break;
		}
		if (i != nb_user)
		{
			rel_change_usr_str = def_user_prop[i].get_value();
			rel_change_usr_def = true;
		}
	}

	if(rel_change_usr_def)
		validate_change_properties(dev_name,"rel_change",rel_change_usr_str,rel_change_usr);

	validate_change_properties(dev_name,"rel_change",rel_change_str,rel_change,rel_change_set_usr_def);
	if(rel_change_usr_def)
	{
		if(rel_change_set_usr_def[0])
			rel_change[0] = rel_change_usr[0];
		if(rel_change_set_usr_def[1])
			rel_change[1] = rel_change_usr[1];
	}

	rel_change[0] = fabs(rel_change[0]);
	rel_change[1] = fabs(rel_change[1]);

	if(rel_change[0] == rel_change[1])
	{
		if(rel_change[0] == INT_MAX && !rel_change_usr_def)
		{
			DbDatum del_relchange("rel_change");
			db_del.push_back(del_relchange);
			prop_to_delete++;
		}
		else
		{
			if(rel_change_usr_def && rel_change[0] == fabs(rel_change_usr[0]))
			{
				DbDatum del_relchange("rel_change");
				db_del.push_back(del_relchange);
				prop_to_delete++;
			}
			else
			{
				DbDatum relchange("rel_change");
				relchange << rel_change[0];
				db_d.push_back(relchange);
				prop_to_update++;
			}
		}
	}
	else
	{
		if(rel_change_usr_def)
		{
			vector<string> rel_change_str_tmp(2);
			vector<bool> rel_change_usr_def_tmp(2);
			rel_change_usr_def_tmp[0] = rel_change_usr_def_tmp[1] = false;

			if(rel_change[0] == fabs(rel_change_usr[0]))
			{
				rel_change_str_tmp[0] = NotANumber;
				rel_change_usr_def_tmp[0] = true;
			}
			if(rel_change[1] == fabs(rel_change_usr[1]))
			{
				rel_change_str_tmp[1] = NotANumber;
				rel_change_usr_def_tmp[1] = true;
			}

			if(rel_change_usr_def_tmp[0] && rel_change_usr_def_tmp[1])
			{
				DbDatum del_relchange("rel_change");
				db_del.push_back(del_relchange);
				prop_to_delete++;
			}
			else if(!rel_change_usr_def_tmp[0] && !rel_change_usr_def_tmp[1])
			{
				DbDatum relchange("rel_change");
				relchange << rel_change;
				db_d.push_back(relchange);
				prop_to_update++;
			}
			else
			{
				TangoSys_MemStream str;
				str.precision(TANGO_FLOAT_PRECISION);
				if(rel_change_usr_def_tmp[0] && !rel_change_usr_def_tmp[1])
				{
					str << rel_change[1];
					rel_change_str_tmp[1] = str.str();
				}
				else
				{
					str << rel_change[0];
					rel_change_str_tmp[0] = str.str();
				}
				DbDatum relchange("rel_change");
				relchange << rel_change_str_tmp;
				db_d.push_back(relchange);
				prop_to_update++;
			}
		}
		else
		{
			DbDatum relchange("rel_change");
			relchange << rel_change;
			db_d.push_back(relchange);
			prop_to_update++;
		}
	}

//
// Absolute change
//

	string abs_change_str(conf.event_prop.ch_event.abs_change); // provided, comma separated min and/or max values for the property
	string abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> abs_change; // vector containing min and max values of the property
	vector<double> abs_change_usr; // vector containing user default min and max values of the property
	bool abs_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "abs_change")
				break;
		}
		if (i != nb_user)
		{
			abs_change_usr_str = def_user_prop[i].get_value();
			abs_change_usr_def = true;
		}
	}

	if(abs_change_usr_def)
		validate_change_properties(dev_name,"abs_change",abs_change_usr_str,abs_change_usr);

	validate_change_properties(dev_name,"abs_change",abs_change_str,abs_change,abs_change_set_usr_def);
	if(abs_change_usr_def)
	{
		if(abs_change_set_usr_def[0])
			abs_change[0] = abs_change_usr[0];
		if(abs_change_set_usr_def[1])
			abs_change[1] = abs_change_usr[1];
	}

	abs_change[0] = fabs(abs_change[0]);
	abs_change[1] = fabs(abs_change[1]);

	if(abs_change[0] == abs_change[1])
	{
		if(abs_change[0] == INT_MAX && !abs_change_usr_def)
		{
			DbDatum del_abschange("abs_change");
			db_del.push_back(del_abschange);
			prop_to_delete++;
		}
		else
		{
			if(abs_change_usr_def && abs_change[0] == fabs(abs_change_usr[0]))
			{
				DbDatum del_abschange("abs_change");
				db_del.push_back(del_abschange);
				prop_to_delete++;
			}
			else
			{
				DbDatum abschange("abs_change");
				abschange << abs_change[0];
				db_d.push_back(abschange);
				prop_to_update++;
			}
		}
	}
	else
	{
		if(abs_change_usr_def)
		{
			vector<string> abs_change_str_tmp(2);
			vector<bool> abs_change_usr_def_tmp(2);
			abs_change_usr_def_tmp[0] = abs_change_usr_def_tmp[1] = false;

			if(abs_change[0] == fabs(abs_change_usr[0]))
			{
				abs_change_str_tmp[0] = NotANumber;
				abs_change_usr_def_tmp[0] = true;
			}
			if(abs_change[1] == fabs(abs_change_usr[1]))
			{
				abs_change_str_tmp[1] = NotANumber;
				abs_change_usr_def_tmp[1] = true;
			}

			if(abs_change_usr_def_tmp[0] && abs_change_usr_def_tmp[1])
			{
				DbDatum del_abschange("abs_change");
				db_del.push_back(del_abschange);
				prop_to_delete++;
			}
			else if(!abs_change_usr_def_tmp[0] && !abs_change_usr_def_tmp[1])
			{
				DbDatum abschange("abs_change");
				abschange << abs_change;
				db_d.push_back(abschange);
				prop_to_update++;
			}
			else
			{
				TangoSys_MemStream str;
				str.precision(TANGO_FLOAT_PRECISION);
				if(abs_change_usr_def_tmp[0] && !abs_change_usr_def_tmp[1])
				{
					str << abs_change[1];
					abs_change_str_tmp[1] = str.str();
				}
				else
				{
					str << abs_change[0];
					abs_change_str_tmp[0] = str.str();
				}
				DbDatum abschange("abs_change");
				abschange << abs_change_str_tmp;
				db_d.push_back(abschange);
				prop_to_update++;
			}
		}
		else
		{
			DbDatum abschange("abs_change");
			abschange << abs_change;
			db_d.push_back(abschange);
			prop_to_update++;
		}
	}

//
// Archive relative change
//

	string archive_rel_change_str(conf.event_prop.arch_event.rel_change); // provided, comma separated min and/or max values for the property
	string archive_rel_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> archive_rel_change; // vector containing min and max values of the property
	vector<double> archive_rel_change_usr; // vector containing user default min and max values of the property
	bool archive_rel_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> archive_rel_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "archive_rel_change")
				break;
		}
		if (i != nb_user)
		{
			archive_rel_change_usr_str = def_user_prop[i].get_value();
			archive_rel_change_usr_def = true;
		}
	}

	if(archive_rel_change_usr_def)
		validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_usr_str,archive_rel_change_usr);

	validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_str,archive_rel_change,archive_rel_change_set_usr_def);
	if(archive_rel_change_usr_def)
	{
		if(archive_rel_change_set_usr_def[0])
			archive_rel_change[0] = archive_rel_change_usr[0];
		if(archive_rel_change_set_usr_def[1])
			archive_rel_change[1] = archive_rel_change_usr[1];
	}

	archive_rel_change[0] = fabs(archive_rel_change[0]);
	archive_rel_change[1] = fabs(archive_rel_change[1]);

	if(archive_rel_change[0] == archive_rel_change[1])
	{
		if(archive_rel_change[0] == INT_MAX && !archive_rel_change_usr_def)
		{
			DbDatum del_archiverelchange("archive_rel_change");
			db_del.push_back(del_archiverelchange);
			prop_to_delete++;
		}
		else
		{
			if(archive_rel_change_usr_def && archive_rel_change[0] == fabs(archive_rel_change_usr[0]))
			{
				DbDatum del_archiverelchange("archive_rel_change");
				db_del.push_back(del_archiverelchange);
				prop_to_delete++;
			}
			else
			{
				DbDatum archiverelchange("archive_rel_change");
				archiverelchange << archive_rel_change[0];
				db_d.push_back(archiverelchange);
				prop_to_update++;
			}
		}
	}
	else
	{
		if(archive_rel_change_usr_def)
		{
			vector<string> archive_rel_change_str_tmp(2);
			vector<bool> archive_rel_change_usr_def_tmp(2);
			archive_rel_change_usr_def_tmp[0] = archive_rel_change_usr_def_tmp[1] = false;

			if(archive_rel_change[0] == fabs(archive_rel_change_usr[0]))
			{
				archive_rel_change_str_tmp[0] = NotANumber;
				archive_rel_change_usr_def_tmp[0] = true;
			}
			if(archive_rel_change[1] == fabs(archive_rel_change_usr[1]))
			{
				archive_rel_change_str_tmp[1] = NotANumber;
				archive_rel_change_usr_def_tmp[1] = true;
			}

			if(archive_rel_change_usr_def_tmp[0] && archive_rel_change_usr_def_tmp[1])
			{
				DbDatum del_archiverelchange("archive_rel_change");
				db_del.push_back(del_archiverelchange);
				prop_to_delete++;
			}
			else if(!archive_rel_change_usr_def_tmp[0] && !archive_rel_change_usr_def_tmp[1])
			{
				DbDatum archiverelchange("archive_rel_change");
				archiverelchange << archive_rel_change;
				db_d.push_back(archiverelchange);
				prop_to_update++;
			}
			else
			{
				TangoSys_MemStream str;
				str.precision(TANGO_FLOAT_PRECISION);
				if(archive_rel_change_usr_def_tmp[0] && !archive_rel_change_usr_def_tmp[1])
				{
					str << archive_rel_change[1];
					archive_rel_change_str_tmp[1] = str.str();
				}
				else
				{
					str << archive_rel_change[0];
					archive_rel_change_str_tmp[0] = str.str();
				}
				DbDatum archiverelchange("archive_rel_change");
				archiverelchange << archive_rel_change_str_tmp;
				db_d.push_back(archiverelchange);
				prop_to_update++;
			}
		}
		else
		{
			DbDatum archiverelchange("archive_rel_change");
			archiverelchange << archive_rel_change;
			db_d.push_back(archiverelchange);
			prop_to_update++;
		}
	}

//
// Archive absolute change
//

	string archive_abs_change_str(conf.event_prop.arch_event.abs_change); // provided, comma separated min and/or max values for the property
	string archive_abs_change_usr_str; // user default, comma separated min and/or max values for the property, if defined
	vector<double> archive_abs_change; // vector containing min and max values of the property
	vector<double> archive_abs_change_usr; // vector containing user default min and max values of the property
	bool archive_abs_change_usr_def = false; // true if there are user defaults defined for the property
	vector<bool> archive_abs_change_set_usr_def; // vector indicating if to use provided values for the property or the user defaults if defined

	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "archive_abs_change")
				break;
		}
		if (i != nb_user)
		{
			archive_abs_change_usr_str = def_user_prop[i].get_value();
			archive_abs_change_usr_def = true;
		}
	}

	if(archive_abs_change_usr_def)
		validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_usr_str,archive_abs_change_usr);

	validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_str,archive_abs_change,archive_abs_change_set_usr_def);
	if(archive_abs_change_usr_def)
	{
		if(archive_abs_change_set_usr_def[0])
			archive_abs_change[0] = archive_abs_change_usr[0];
		if(archive_abs_change_set_usr_def[1])
			archive_abs_change[1] = archive_abs_change_usr[1];
	}

	archive_abs_change[0] = fabs(archive_abs_change[0]);
	archive_abs_change[1] = fabs(archive_abs_change[1]);

	if(archive_abs_change[0] == archive_abs_change[1])
	{
		if(archive_abs_change[0] == INT_MAX && !archive_abs_change_usr_def)
		{
			DbDatum del_archiveabschange("archive_abs_change");
			db_del.push_back(del_archiveabschange);
			prop_to_delete++;
		}
		else
		{
			if(archive_abs_change_usr_def && archive_abs_change[0] == fabs(archive_abs_change_usr[0]))
			{
				DbDatum del_archiveabschange("archive_abs_change");
				db_del.push_back(del_archiveabschange);
				prop_to_delete++;
			}
			else
			{
				DbDatum archiveabschange("archive_abs_change");
				archiveabschange << archive_abs_change[0];
				db_d.push_back(archiveabschange);
				prop_to_update++;
			}
		}
	}
	else
	{
		if(archive_abs_change_usr_def)
		{
			vector<string> archive_abs_change_str_tmp(2);
			vector<bool> archive_abs_change_usr_def_tmp(2);
			archive_abs_change_usr_def_tmp[0] = archive_abs_change_usr_def_tmp[1] = false;

			if(archive_abs_change[0] == fabs(archive_abs_change_usr[0]))
			{
				archive_abs_change_str_tmp[0] = NotANumber;
				archive_abs_change_usr_def_tmp[0] = true;
			}
			if(archive_abs_change[1] == fabs(archive_abs_change_usr[1]))
			{
				archive_abs_change_str_tmp[1] = NotANumber;
				archive_abs_change_usr_def_tmp[1] = true;
			}

			if(archive_abs_change_usr_def_tmp[0] && archive_abs_change_usr_def_tmp[1])
			{
				DbDatum del_archiveabschange("archive_abs_change");
				db_del.push_back(del_archiveabschange);
				prop_to_delete++;
			}
			else if(!archive_abs_change_usr_def_tmp[0] && !archive_abs_change_usr_def_tmp[1])
			{
				DbDatum archiveabschange("archive_abs_change");
				archiveabschange << archive_abs_change;
				db_d.push_back(archiveabschange);
				prop_to_update++;
			}
			else
			{
				TangoSys_MemStream str;
				str.precision(TANGO_FLOAT_PRECISION);
				if(archive_abs_change_usr_def_tmp[0] && !archive_abs_change_usr_def_tmp[1])
				{
					str << archive_abs_change[1];
					archive_abs_change_str_tmp[1] = str.str();
				}
				else
				{
					str << archive_abs_change[0];
					archive_abs_change_str_tmp[0] = str.str();
				}
				DbDatum archiveabschange("archive_abs_change");
				archiveabschange << archive_abs_change_str_tmp;
				db_d.push_back(archiveabschange);
				prop_to_update++;
			}
		}
		else
		{
			DbDatum archiveabschange("archive_abs_change");
			archiveabschange << archive_abs_change;
			db_d.push_back(archiveabschange);
			prop_to_update++;
		}
	}

//
// Event period
//

	TangoSys_MemStream def_event_period_str;
	string def_event_period;
	def_event_period_str << (int)(DEFAULT_EVENT_PERIOD);
	def_event_period_str >> def_event_period;
	user_defaults = false;
	store_in_db = true;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "event_period")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
			if((TG_strcasecmp(usr_def_val.c_str(),AlrmValueNotSpec) == 0) ||
					(TG_strcasecmp(usr_def_val.c_str(),NotANumber) == 0) ||
					(strlen(usr_def_val.c_str()) == 0))
			{
				TangoSys_MemStream str;
				str << (int)(DEFAULT_EVENT_PERIOD);
				usr_def_val = str.str();
			}
		}
	}

	if(user_defaults)
	{
		if ((TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0) ||
				(strcmp(conf.event_prop.per_event.period,usr_def_val.c_str()) == 0) ||
				(strlen(conf.event_prop.per_event.period) == 0))
			store_in_db = false;
	}
	else
	{
		if ((TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.event_prop.per_event.period,def_event_period.c_str()) == 0) ||
				(TG_strcasecmp(conf.event_prop.per_event.period,NotANumber) == 0) ||
				(strlen(conf.event_prop.per_event.period) == 0))
			store_in_db = false;
	}

	if(store_in_db)
	{
		const char *tmp = conf.event_prop.per_event.period.in();
		if(TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0)
			tmp = def_event_period.c_str();
		else
		{
			str.str("");
			str.clear();
			str << conf.event_prop.per_event.period;
			int tmp_period;
			if (!(str >> tmp_period && str.eof()))
				throw_err_format("event_period",dev_name,"Attribute::upd_database");
		}
		DbDatum eventperiod("event_period");
		eventperiod << tmp;
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
	user_defaults = false;
	store_in_db = true;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "archive_period")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
			if((TG_strcasecmp(usr_def_val.c_str(),AlrmValueNotSpec) == 0) ||
					(TG_strcasecmp(usr_def_val.c_str(),NotANumber) == 0) ||
					(strlen(usr_def_val.c_str()) == 0))
			{
				TangoSys_MemStream str;
				str << (int)(INT_MAX);
				usr_def_val = str.str();
			}
		}
	}

	if(user_defaults)
	{
		if ((TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0) ||
				(strcmp(conf.event_prop.arch_event.period,usr_def_val.c_str()) == 0) ||
				(strlen(conf.event_prop.arch_event.period) == 0))
			store_in_db = false;
	}
	else
	{
		if ((TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.event_prop.arch_event.period,def_arch_event_period.c_str()) == 0) ||
				(TG_strcasecmp(conf.event_prop.arch_event.period,NotANumber) == 0) ||
				(strlen(conf.event_prop.arch_event.period) == 0))
			store_in_db = false;
	}

	if(store_in_db)
	{
		const char *tmp = conf.event_prop.arch_event.period.in();
		if(TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0)
			tmp = def_arch_event_period.c_str();
		else
		{
			str.str("");
			str.clear();
			str << conf.event_prop.arch_event.period;
			int tmp_period;
			if (!(str >> tmp_period && str.eof()))
				throw_err_format("archive_period",dev_name,"Attribute::upd_database");
		}
		DbDatum archeventperiod("archive_period");
		archeventperiod << tmp;
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
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
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
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
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

//+--------------------------------------------------------------------------
//
// method : 		Attribute::check_str_prop
//
// description : 	Check if attribute properties which are real string
//			needs to be updated in db
//
// in :			conf : The attribute configuration structure
//			db_d : The DbData which will be used to update database
//			prop_to_update : The number of property(ies) to update
//					 in database
//
//--------------------------------------------------------------------------

void Attribute::check_str_prop(const Tango::AttributeConfig &conf,
			       Tango::DbData &db_d,long &prop_to_update,
			       Tango::DbData &db_del,long &prop_to_delete,
			       vector<AttrProperty> &def_user_prop)
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

	bool user_defaults;
	string usr_def_val;
	size_t nb_user = def_user_prop.size();

//
// Description
//

	user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "description")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	if(user_defaults) // user defaults defined
	{
		if ((TG_strcasecmp(conf.description,NotANumber) == 0) ||
				(strcmp(conf.description,usr_def_val.c_str()) == 0) ||
				(strlen(conf.description) == 0))
		{
			DbDatum del_desc("description");
			db_del.push_back(del_desc);
			prop_to_delete++;
		}
		else
		{
			DbDatum desc("description");
			const char *tmp = conf.description.in();
			if(TG_strcasecmp(conf.description,AlrmValueNotSpec) == 0) // force the library defaults
				tmp = DescNotSpec;
			else if(TG_strcasecmp(conf.description,DescNotSpec) == 0)
				tmp = DescNotSpec;
			desc << tmp;
			db_d.push_back(desc);
			prop_to_update++;
		}
	}
	else // no user defaults defined
	{
		if ((TG_strcasecmp(conf.description,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.description,DescNotSpec) == 0) ||
				(TG_strcasecmp(conf.description,NotANumber) == 0) ||
				(strlen(conf.description) == 0))
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
	}

//
// Label
//

	user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "label")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	if(user_defaults) // user defaults defined
	{
		if ((TG_strcasecmp(conf.label,NotANumber) == 0) ||
				(strcmp(conf.label,usr_def_val.c_str()) == 0) ||
				(strlen(conf.label) == 0))
		{
			DbDatum del_lab("label");
			db_del.push_back(del_lab);
			prop_to_delete++;
		}
		else
		{
			DbDatum lab("label");
			const char *tmp = conf.label.in();
			if(TG_strcasecmp(conf.label,AlrmValueNotSpec) == 0) // force the library defaults
				tmp = LabelNotSpec;
			else if(TG_strcasecmp(conf.label,LabelNotSpec) == 0)
				tmp = LabelNotSpec;
			lab << tmp;
			db_d.push_back(lab);
			prop_to_update++;
		}
	}
	else // no user defaults defined
	{
		if ((TG_strcasecmp(conf.label,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.label,LabelNotSpec) == 0) ||
				(TG_strcasecmp(conf.label,name.c_str()) == 0) ||
				(TG_strcasecmp(conf.label,NotANumber) == 0) ||
				(strlen(conf.label) == 0))
		{
			DbDatum del_lab("label");
			db_del.push_back(del_lab);
			prop_to_delete++;
		}
		else
		{
			DbDatum lab("label");
			const char *tmp = conf.label.in();
			lab << tmp;
			db_d.push_back(lab);
			prop_to_update++;
		}
	}

//
// Unit
//

	user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "unit")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	if(user_defaults) // user defaults defined
	{
		if ((TG_strcasecmp(conf.unit,NotANumber) == 0) ||
				(strcmp(conf.unit,usr_def_val.c_str()) == 0) ||
				(strlen(conf.unit) == 0))
		{
			DbDatum del_unit("unit");
			db_del.push_back(del_unit);
			prop_to_delete++;
		}
		else
		{
			DbDatum uni("unit");
			const char *tmp = conf.unit.in();
			if(TG_strcasecmp(conf.unit,AlrmValueNotSpec) == 0) // force the library defaults
				tmp = UnitNotSpec;
			else if(TG_strcasecmp(conf.unit,UnitNotSpec) == 0)
				tmp = UnitNotSpec;
			uni << tmp;
			db_d.push_back(uni);
			prop_to_update++;
		}
	}
	else // no user defaults defined
	{
		if ((TG_strcasecmp(conf.unit,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.unit,UnitNotSpec) == 0) ||
				(TG_strcasecmp(conf.unit,NotANumber) == 0) ||
				(strlen(conf.unit) == 0))
		{
			DbDatum del_unit("unit");
			db_del.push_back(del_unit);
			prop_to_delete++;
		}
		else
		{
			DbDatum uni("unit");
			const char *tmp = conf.unit.in();
			uni << tmp;
			db_d.push_back(uni);
			prop_to_update++;
		}
	}

//
// Standard unit
//

	user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "standard_unit")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	if(user_defaults) // user defaults defined
	{
		if ((TG_strcasecmp(conf.standard_unit,NotANumber) == 0) ||
				(strcmp(conf.standard_unit,usr_def_val.c_str()) == 0) ||
				(strlen(conf.standard_unit) == 0))
		{
			DbDatum del_std_uni("standard_unit");
			db_del.push_back(del_std_uni);
			prop_to_delete++;
		}
		else
		{
			DbDatum std_uni("standard_unit");
			const char *tmp = conf.standard_unit.in();
			if(TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) == 0) // force the library defaults
				tmp = StdUnitNotSpec;
			else if(TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) == 0)
				tmp = StdUnitNotSpec;
			std_uni << tmp;
			db_d.push_back(std_uni);
			prop_to_update++;
		}
	}
	else // no user defaults defined
	{
		if ((TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) == 0) ||
				(TG_strcasecmp(conf.standard_unit,NotANumber) == 0) ||
				(strlen(conf.standard_unit) == 0))
		{
			DbDatum del_std_unit("standard_unit");
			db_del.push_back(del_std_unit);
			prop_to_delete++;
		}
		else
		{
			DbDatum std_uni("standard_unit");
			const char *tmp = conf.standard_unit.in();
			std_uni << tmp;
			db_d.push_back(std_uni);
			prop_to_update++;
		}
	}

//
// Display unit
//

	user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "display_unit")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}

	if(user_defaults) // user defaults defined
	{
		if ((TG_strcasecmp(conf.display_unit,NotANumber) == 0) ||
				(strcmp(conf.display_unit,usr_def_val.c_str()) == 0) ||
				(strlen(conf.display_unit) == 0))
		{
			DbDatum del_disp_unit("display_unit");
			db_del.push_back(del_disp_unit);
			prop_to_delete++;
		}
		else
		{
			DbDatum disp_uni("display_unit");
			const char *tmp = conf.display_unit.in();
			if(TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) == 0) // force the library defaults
				tmp = DispUnitNotSpec;
			else if(TG_strcasecmp(conf.display_unit,DispUnitNotSpec) == 0)
				tmp = DispUnitNotSpec;
			disp_uni << tmp;
			db_d.push_back(disp_uni);
			prop_to_update++;
		}
	}
	else // no user defaults defined
	{
		if ((TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.display_unit,DispUnitNotSpec) == 0) ||
				(TG_strcasecmp(conf.display_unit,NotANumber) == 0) ||
				(strlen(conf.display_unit) == 0))
		{
			DbDatum del_disp_unit("display_unit");
			db_del.push_back(del_disp_unit);
			prop_to_delete++;
		}
		else
		{
			DbDatum disp_uni("display_unit");
			const char *tmp = conf.display_unit.in();
			disp_uni << tmp;
			db_d.push_back(disp_uni);
			prop_to_update++;
		}
	}

//
// Format
//

	user_defaults = false;
	if (nb_user != 0)
	{
		size_t i;
		for (i = 0;i < nb_user;i++)
		{
			if (def_user_prop[i].get_name() == "format")
				break;
		}
		if (i != nb_user) // user defaults defined
		{
			user_defaults = true;
			usr_def_val = def_user_prop[i].get_value();
		}
	}
	if(user_defaults) // user defaults defined
	{
		if ((TG_strcasecmp(conf.format,NotANumber) == 0) ||
				(strcmp(conf.format,usr_def_val.c_str()) == 0) ||
				(strlen(conf.format) == 0))
		{
			DbDatum del_for("format");
			db_del.push_back(del_for);
			prop_to_delete++;
		}
		else
		{
			DbDatum form("format");
			const char *tmp = conf.format.in();
			if(TG_strcasecmp(conf.format,AlrmValueNotSpec) == 0) // force the library defaults
				tmp = FormatNotSpec;
			else if(TG_strcasecmp(conf.format,FormatNotSpec) == 0)
				tmp = FormatNotSpec;
			form << tmp;
			db_d.push_back(form);
			prop_to_update++;
		}
	}
	else // no user defaults defined
	{
		if ((TG_strcasecmp(conf.format,AlrmValueNotSpec) == 0) ||
				(TG_strcasecmp(conf.format,FormatNotSpec) == 0) ||
				(TG_strcasecmp(conf.format,NotANumber) == 0) ||
				(strlen(conf.format) == 0))
		{
			DbDatum del_for("format");
			db_del.push_back(del_for);
			prop_to_delete++;
		}
		else
		{
			DbDatum form("format");
			const char *tmp = conf.format.in();
			form << tmp;
			db_d.push_back(form);
			prop_to_update++;
		}
	}
}


//+--------------------------------------------------------------------------
//
// method : 		Attribute::validate_change_properties
//
// description : 	Check if attribute change properties are properly defined
//
// in :				dev_name : The name of the device
//				prop_name : The name of the property
//				change_prop_str : A string representing the change property
//					(in the form of either "value" or "value1,value2")
//
// out :			validated_prop : A vector of parsed change properties values
//					in the form of numbers of type double
//
//--------------------------------------------------------------------------

void Attribute::validate_change_properties(const string &dev_name, const char *prop_name, string &change_prop_str, vector<double> &validated_prop)
{
	vector<bool> bring_usr_def;
	validate_change_properties(dev_name, prop_name, change_prop_str, validated_prop, bring_usr_def);
}

//+--------------------------------------------------------------------------
//
// method : 		Attribute::validate_change_properties
//
// description : 	Check if attribute change properties are properly defined
//
// in :				dev_name : The name of the device
//				prop_name : The name of the property
//				change_prop_str : A string representing the change property
//					(in the form of either "value" or "value1,value2")
//
// out :			validated_prop : A vector of parsed change properties values
//					in the form of numbers of type double
//				bring_usr_def : A vector of boolean values indicating if for
//					a corresponding value in validated_prop vector there
//					was a request to restore the user defined value
//
//--------------------------------------------------------------------------

void Attribute::validate_change_properties(const string &dev_name, const char *prop_name, string &change_prop_str, vector<double> &validated_prop, vector<bool> &bring_usr_def)
{
	// by default, values for event change properties are set to INT_MAX
	validated_prop.clear();
	validated_prop.push_back(INT_MAX);
	validated_prop.push_back(INT_MAX);

	// by default, bring user defined properties flags are set to false
	bring_usr_def.clear();
	bring_usr_def.push_back(false);
	bring_usr_def.push_back(false);

	TangoSys_MemStream str;
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
		bring_usr_def[0] = true;
		bring_usr_def[1] = true;
	}
	else if(!one_param && prop_min == "")
		throw_err_format(prop_name,dev_name,"Attribute::validate_change_properties");
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
					throw_err_data_type(prop_name,const_cast<string&>(dev_name),"Attribute::validate_change_properties");
			}
		}
		else
			throw_err_format(prop_name,dev_name,"Attribute::validate_change_properties");
	}

	if(!one_param)
	{
		validated_prop[1] = INT_MAX;
		bring_usr_def[1] = false;

		if(TG_strcasecmp(prop_max.c_str(),AlrmValueNotSpec) == 0)
			validated_prop[1] = INT_MAX;
		else if(TG_strcasecmp(prop_max.c_str(),NotANumber) == 0)
			bring_usr_def[1] = true;
		else if(prop_max == "")
			throw_err_format(prop_name,dev_name,"Attribute::validate_change_properties");
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
						throw_err_data_type(prop_name,const_cast<string&>(dev_name),"Attribute::validate_change_properties");
				}
			}
			else
				throw_err_format(prop_name,dev_name,"Attribute::validate_change_properties");
		}
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_value
//
// description : 	Set the attribute read value and quality. This method
//			automatically set the date when it has been called
//			This method is overloaded several times for all the
//			supported attribute data type
//
// in :			p_data : The attribute read value
//			x : The attribute x dimension (default is 1)
//			y : The atttribute y dimension (default is 0)
//			release : A flag set to true if memory must be
//				  de-allocated (default is false)
//
//--------------------------------------------------------------------------

void Attribute::set_value(Tango::DevShort *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_SHORT)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_sh[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.sh_seq = new Tango::DevVarShortArray(data_size);
				value.sh_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.sh_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevShort *tmp_ptr = new Tango::DevShort[1];
				*tmp_ptr = *p_data;
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevLong *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_LONG)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_lo[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.lg_seq = new Tango::DevVarLongArray(data_size);
				value.lg_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.lg_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevLong *tmp_ptr = new Tango::DevLong[1];
				*tmp_ptr = *p_data;
				value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevLong64 *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_LONG64)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.lg64_seq = new Tango::DevVarLong64Array(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				ext->tmp_lo64[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.lg64_seq = new Tango::DevVarLong64Array(data_size);
				value.lg64_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.lg64_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevLong64 *tmp_ptr = new Tango::DevLong64[1];
				*tmp_ptr = *p_data;
				value.lg64_seq = new Tango::DevVarLong64Array(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.lg64_seq = new Tango::DevVarLong64Array(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevFloat *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_FLOAT)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_fl[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.fl_seq = new Tango::DevVarFloatArray(data_size);
				value.fl_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.fl_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevFloat *tmp_ptr = new Tango::DevFloat[1];
				*tmp_ptr = *p_data;
				value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevDouble *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_DOUBLE)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_db[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.db_seq = new Tango::DevVarDoubleArray(data_size);
				value.db_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.db_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevDouble *tmp_ptr = new Tango::DevDouble[1];
				*tmp_ptr = *p_data;
				value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevString *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_STRING)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		if (release == true)
		{
			char **strvec = Tango::DevVarStringArray::allocbuf(data_size);
			for (int i = 0;i < data_size;i++)
				strvec[i] = p_data[i];
			value.str_seq = new Tango::DevVarStringArray(data_size,data_size,strvec,release);
			delete [] p_data;
		}
		else
			value.str_seq = new Tango::DevVarStringArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_str[0] = *p_data;
				if (release == true)
					delete p_data;
				ext->scalar_str_attr_release = release;
			}
			else
			{
				value.str_seq = new Tango::DevVarStringArray(data_size);
				value.str_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.str_seq)[k] = CORBA::string_dup(p_data[k]);
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if (release == true)
			{
				char **strvec = Tango::DevVarStringArray::allocbuf(data_size);
				for (int i = 0;i < data_size;i++)
					strvec[i] = p_data[i];
				value.str_seq = new Tango::DevVarStringArray(data_size,data_size,strvec,release);
				if (data_format == Tango::SCALAR)
					delete p_data;
				else
					delete [] p_data;
			}
			else
				value.str_seq = new Tango::DevVarStringArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevUShort *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_USHORT)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_ush[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.ush_seq = new Tango::DevVarUShortArray(data_size);
				value.ush_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ush_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevUShort *tmp_ptr = new Tango::DevUShort[1];
				*tmp_ptr = *p_data;
				value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevBoolean *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_BOOLEAN)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_boo[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.boo_seq = new Tango::DevVarBooleanArray(data_size);
				value.boo_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.boo_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevBoolean *tmp_ptr = new Tango::DevBoolean[1];
				*tmp_ptr = *p_data;
				value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevUChar *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_UCHAR)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_cha[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.cha_seq = new Tango::DevVarCharArray(data_size);
				value.cha_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.cha_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevUChar *tmp_ptr = new Tango::DevUChar[1];
				*tmp_ptr = *p_data;
				value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevULong *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_ULONG)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.ulg_seq = new Tango::DevVarULongArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				ext->tmp_ulo[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.ulg_seq = new Tango::DevVarULongArray(data_size);
				value.ulg_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ulg_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevULong *tmp_ptr = new Tango::DevULong[1];
				*tmp_ptr = *p_data;
				value.ulg_seq = new Tango::DevVarULongArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.ulg_seq = new Tango::DevVarULongArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevULong64 *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_ULONG64)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.ulg64_seq = new Tango::DevVarULong64Array(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				ext->tmp_ulo64[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.ulg64_seq = new Tango::DevVarULong64Array(data_size);
				value.ulg64_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ulg64_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevULong64 *tmp_ptr = new Tango::DevULong64[1];
				*tmp_ptr = *p_data;
				value.ulg64_seq = new Tango::DevVarULong64Array(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.ulg64_seq = new Tango::DevVarULong64Array(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevState *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_STATE)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.state_seq = new Tango::DevVarStateArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				ext->tmp_state[0] = *p_data;
				if (release == true)
					delete p_data;
			}
			else
			{
				value.state_seq = new Tango::DevVarStateArray(data_size);
				value.state_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.state_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevState *tmp_ptr = new Tango::DevState[1];
				*tmp_ptr = *p_data;
				value.state_seq = new Tango::DevVarStateArray(data_size,data_size,tmp_ptr,release);
				delete p_data;
			}
			else
				value.state_seq = new Tango::DevVarStateArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevEncoded *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_ENCODED)
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		if (release == true)
			delete p_data;

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.enc_seq = new Tango::DevVarEncodedArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			tmp_enc[0] = *p_data;
			if (release == true)
				delete p_data;
		}
		else
		{
			if (release == true)
			{
				DevEncoded *tmp_ptr = new DevEncoded[1];

				tmp_ptr->encoded_format = p_data->encoded_format;

				unsigned long nb_data = p_data->encoded_data.length();
				tmp_ptr->encoded_data.replace(nb_data,nb_data,p_data->encoded_data.get_buffer(true),true);
				p_data->encoded_data.replace(0,0,NULL,false);

				value.enc_seq = new Tango::DevVarEncodedArray(data_size,data_size,tmp_ptr,true);
				delete p_data;
			}
			else
				value.enc_seq = new Tango::DevVarEncodedArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,bool release)
{
	if (release == false)
	{
		enc_help.encoded_format = CORBA::string_dup(*p_data_str);
		enc_help.encoded_data.replace(size,size,p_data,false);

		set_value(&enc_help);
	}
	else
	{
		DevEncoded *enc_ptr = new DevEncoded;
		enc_ptr->encoded_format = CORBA::string_dup(*p_data_str);
		delete [] *p_data_str;
		enc_ptr->encoded_data.replace(size,size,p_data,true);

		set_value(enc_ptr,1,0,true);
	}
}

void Attribute::set_value(Tango::EncodedAttribute *attr) {

  Tango::DevString *f    = attr->get_format();
  Tango::DevUChar  *d    = attr->get_data();
  long              size = attr->get_size();

  if( *f==NULL ) {

    TangoSys_OMemStream o;
    o << "DevEncoded format for attribute " << name << " not specified" << ends;
    Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
                            (const char *)"Attribute::set_value");
  }

  if( size==0 || !d ) {

    TangoSys_OMemStream o;
    o << "DevEncoded data for attribute " << name << " not specified" << ends;
    Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
                            (const char *)"Attribute::set_value");
  }


  set_value(f,d,size,false);

  if (attr->get_exclusion() == true)
  {
  	set_user_attr_mutex(attr->get_mutex());
  }

}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_value_date_quality
//
// description : 	Set the attribute read value date and quality.
//			This method is overloaded several times for all the
//			supported attribute data type
//
// in :			p_data : The attribute read value
//			t : The attribute date
//			qual : The attribute quality
//			x : The attribute x dimension (default is 1)
//			y : The atttribute y dimension (default is 0)
//			release : A flag set to true if memory must be
//				  de-allocated (default is false)
//
//--------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevShort *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevShort *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevShort *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevLong *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevLong *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevLong *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif


//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevLong64 *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevLong64 *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevLong64 *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevFloat *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevFloat *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevFloat *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevDouble *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevDouble *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevDouble *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevString *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevString *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevString *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevUShort *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevUShort *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevUShort *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevUChar *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevUChar *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevUChar *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevULong *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevULong *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevULong *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevULong64 *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevULong64 *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevULong64 *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevState *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevState *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevState *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevEncoded *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
}

void Attribute::set_value_date_quality(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,time_t t,
				    Tango::AttrQuality qual,
				    bool release)
{
	set_value(p_data_str,p_data,size,release);
	set_quality(qual,false);
	set_date(t);
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevEncoded *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
}

void Attribute::set_value_date_quality(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,struct _timeb &t,
				    Tango::AttrQuality qual,
				    bool release)
{
	set_value(p_data_str,p_data,size,release);
	set_quality(qual,false);
	set_date(t);
}
#else
void Attribute::set_value_date_quality(Tango::DevEncoded *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
}

void Attribute::set_value_date_quality(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,struct timeval &t,
				    Tango::AttrQuality qual,
				    bool release)
{
	set_value(p_data_str,p_data,size,release);
	set_quality(qual,false);
	set_date(t);
}
#endif

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_data_size
//
// description : 	Compute the attribute amount of data
//
//--------------------------------------------------------------------------

void Attribute::set_data_size()
{
	switch(data_format)
	{
	case Tango::SCALAR :
		data_size = 1;
		break;

	case Tango::SPECTRUM :
		data_size = dim_x;
		break;

	case Tango::IMAGE :
		data_size = dim_x * dim_y;
		break;

	case FMT_UNKNOWN :
		data_size = 0;
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_time
//
// description : 	Set the date if the date flag is true
//
//--------------------------------------------------------------------------

void Attribute::set_time()
{
	if (date == true)
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
}

//+-------------------------------------------------------------------------
//
// method :		Attribute::check_alarm
//
// description :	Check if the attribute is in alarm
//
// This method returns a boolean set to true if the atribute is in alarm. In
// this case, it also set the attribute quality factor to ALARM
//
//--------------------------------------------------------------------------

bool Attribute::check_alarm()
{
	bool returned = false;

//
// Throw exception if no alarm is defined for this attribute
//

	if (is_alarmed().none() == true)
	{
		TangoSys_OMemStream o;

		o << "No alarm defined for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrNoAlarm",o.str(),
				      (const char *)"Attribute::check_alarm");
	}

//
// If the attribute quality is different than VALID don`t do any checking to avoid
// to override a user positioned quality value.
// If no alarms levels are specified, just return without alarm.
//

	if ( quality != Tango::ATTR_VALID )
	{
	    log_quality();
		return returned;
	}

//
// Get the monitor protecting device att config
//

	TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
	AutoTangoMonitor sync1(&mon1);

//
// If some alarm are defined on level, check attribute level
//

	bitset<numFlags> &bs = is_alarmed();

	if ((bs.test(Attribute::min_level) == true) ||
	    (bs.test(Attribute::max_level) == true))
	{
		if (check_level_alarm() == true)
			returned = true;
	}

	if (returned == false)
	{
		if ((bs.test(Attribute::min_warn) == true) || (bs.test(Attribute::max_warn) == true))
		{
			if (check_warn_alarm() == true)
				returned = true;
		}
	}

//
// If the RDS alarm is set, check attribute level
//

	if ((bs.test(Attribute::rds) == true))
	{
		if (check_rds_alarm() == true)
			returned = true;
	}

    log_quality();

	return returned;
}


//+-------------------------------------------------------------------------
//
// method :		Attribute::check_level_alarm
//
// description :	Check if the attribute is in alarm level
//
// This method returns a boolean set to true if the atribute is in alarm. In
// this case, it also set the attribute quality factor to ALARM
//
//--------------------------------------------------------------------------

bool Attribute::check_level_alarm()
{
	bool returned = false;
	bool real_returned = false;

//
// Check the min alarm if defined
//

	long i;
	if (alarm_conf.test(min_level) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (check_scalar_wattribute() == true)
			{
				short tmp_val;
				if (date == false)
					tmp_val = (*value.sh_seq)[0];
				else
					tmp_val = tmp_sh[0];

				if (tmp_val <= min_alarm.sh)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.sh_seq)[i] <= min_alarm.sh)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevLong tmp_val;
				if (date == false)
					tmp_val = (*value.lg_seq)[0];
				else
					tmp_val = tmp_lo[0];

				if (tmp_val <= min_alarm.lg)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg_seq)[i] <= min_alarm.lg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG64:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevLong64 tmp_val;
				if (date == false)
					tmp_val = (*value.lg64_seq)[0];
				else
					tmp_val = ext->tmp_lo64[0];

				if (tmp_val <= min_alarm.lg64)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg64_seq)[i] <= min_alarm.lg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_DOUBLE:
			if (check_scalar_wattribute() == true)
			{
                Tango::DevDouble tmp_val;
                if (date == false)
                    tmp_val = (*value.db_seq)[0];
                else
                    tmp_val = tmp_db[0];

                if (tmp_val <= min_alarm.db)
                    returned = true;
			}
			else
			{
                for (i = 0;i < data_size;i++)
                {
                    if ((*value.db_seq)[i] <= min_alarm.db)
                    {
                        returned = true;
                        break;
                    }
			    }
			}
			break;

		case Tango::DEV_FLOAT:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevFloat tmp_val;
				if (date == false)
					tmp_val = (*value.fl_seq)[0];
				else
					tmp_val = tmp_fl[0];

				if (tmp_val <= min_alarm.fl)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.fl_seq)[i] <= min_alarm.fl)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_USHORT:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevUShort tmp_val;
				if (date == false)
					tmp_val = (*value.ush_seq)[0];
				else
					tmp_val = tmp_ush[0];

				if (tmp_val <= min_alarm.ush)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ush_seq)[i] <= min_alarm.ush)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_UCHAR:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevUChar tmp_val;
				if (date == false)
					tmp_val = (*value.cha_seq)[0];
				else
					tmp_val = tmp_cha[0];

				if (tmp_val <= min_alarm.uch)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.cha_seq)[i] <= min_alarm.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG64:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevULong64 tmp_val;
				if (date == false)
					tmp_val = (*value.ulg64_seq)[0];
				else
					tmp_val = ext->tmp_ulo64[0];

				if (tmp_val <= min_alarm.ulg64)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg64_seq)[i] <= min_alarm.ulg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevULong tmp_val;
				if (date == false)
					tmp_val = (*value.ulg_seq)[0];
				else
					tmp_val = ext->tmp_ulo[0];

				if (tmp_val <= min_alarm.ulg)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg_seq)[i] <= min_alarm.ulg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ENCODED:
			if (check_scalar_wattribute() == true)
			{
				for (unsigned int i = 0;i < tmp_enc[0].encoded_data.length();i++)
				{
					if (tmp_enc[0].encoded_data[i] <= min_alarm.uch)
					{
						returned = true;
						break;
					}
				}
			}
			else
			{
				for (unsigned int i = 0;i < (*value.enc_seq)[0].encoded_data.length();i++)
				{
					if ((*value.enc_seq)[0].encoded_data[i] <= min_alarm.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;
		}
		if (returned == true)
		{
			quality = Tango::ATTR_ALARM;
			alarm.set(min_level);
			real_returned = true;
		}
	}

//
// Check the max alarm if defined
//

	returned = false;
	if (alarm_conf.test(max_level) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevShort tmp_val;
				if (date == false)
					tmp_val = (*value.sh_seq)[0];
				else
					tmp_val = tmp_sh[0];

				if (tmp_val >= max_alarm.sh)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.sh_seq)[i] >= max_alarm.sh)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevLong tmp_val;
				if (date == false)
					tmp_val = (*value.lg_seq)[0];
				else
					tmp_val = tmp_lo[0];

				if (tmp_val >= max_alarm.lg)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg_seq)[i] >= max_alarm.lg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG64:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevLong64 tmp_val;
				if (date == false)
					tmp_val = (*value.lg64_seq)[0];
				else
					tmp_val = ext->tmp_lo64[0];

				if (tmp_val >= max_alarm.lg64)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg64_seq)[i] >= max_alarm.lg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_DOUBLE:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevDouble tmp_val;
				if (date == false)
					tmp_val = (*value.db_seq)[0];
				else
					tmp_val = tmp_db[0];

				if (tmp_val >= max_alarm.db)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.db_seq)[i] >= max_alarm.db)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_FLOAT:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevFloat tmp_val;
				if (date == false)
					tmp_val = (*value.fl_seq)[0];
				else
					tmp_val = tmp_fl[0];

				if (tmp_val >= max_alarm.fl)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.fl_seq)[i] >= max_alarm.fl)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_USHORT:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevUShort tmp_val;
				if (date == false)
					tmp_val = (*value.ush_seq)[0];
				else
					tmp_val = tmp_ush[0];

				if (tmp_val >= max_alarm.ush)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ush_seq)[i] >= max_alarm.ush)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_UCHAR:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevUChar tmp_val;
				if (date == false)
					tmp_val = (*value.cha_seq)[0];
				else
					tmp_val = tmp_cha[0];

				if (tmp_val >= max_alarm.uch)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.cha_seq)[i] >= max_alarm.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevULong tmp_val;
				if (date == false)
					tmp_val = (*value.ulg_seq)[0];
				else
					tmp_val = ext->tmp_ulo[0];

				if (tmp_val >= max_alarm.ulg)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg_seq)[i] >= max_alarm.ulg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG64:
			if (check_scalar_wattribute() == true)
			{
				Tango::DevULong64 tmp_val;
				if (date == false)
					tmp_val = (*value.ulg64_seq)[0];
				else
					tmp_val = ext->tmp_ulo64[0];

				if (tmp_val >= max_alarm.ulg64)
					returned = true;
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg64_seq)[i] >= max_alarm.ulg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ENCODED:
			if (check_scalar_wattribute() == true)
			{
				for (unsigned int i = 0;i < tmp_enc[0].encoded_data.length();i++)
				{
					if (tmp_enc[0].encoded_data[i] >= max_alarm.uch)
					{
						returned = true;
						break;
					}
				}
			}
			else
			{
				for (unsigned int i = 0;i < (*value.enc_seq)[0].encoded_data.length();i++)
				{
					if ((*value.enc_seq)[0].encoded_data[i] >= max_alarm.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;
		}
		if (returned == true)
		{
			quality = Tango::ATTR_ALARM;
			alarm.set(max_level);
			real_returned = true;
		}
	}

	return real_returned;
}


//+-------------------------------------------------------------------------
//
// method :		Attribute::check_warn_alarm
//
// description :	Check if the attribute is in warning alarm
//
// This method returns a boolean set to true if the atribute is in alarm. In
// this case, it also set the attribute quality factor to ALARM
//
//--------------------------------------------------------------------------

bool Attribute::check_warn_alarm()
{
	bool returned = false;
	bool real_returned = false;

//
// Check the min warning if defined
//

	long i;
	if (alarm_conf.test(min_warn) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_sh[0] <= min_warning.sh)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.sh_seq)[i] <= min_warning.sh)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_lo[0] <= min_warning.lg)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg_seq)[i] <= min_warning.lg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG64:
			if (check_scalar_wattribute() == true)
			{
				if (ext->tmp_lo64[0] <= min_warning.lg64)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg64_seq)[i] <= min_warning.lg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_DOUBLE:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_db[0] <= min_warning.db)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.db_seq)[i] <= min_warning.db)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_FLOAT:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_fl[0] <= min_warning.fl)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.fl_seq)[i] <= min_warning.fl)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_USHORT:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_ush[0] <= min_warning.ush)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ush_seq)[i] <= min_warning.ush)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_UCHAR:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_cha[0] <= min_warning.uch)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.cha_seq)[i] <= min_warning.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG:
			if (check_scalar_wattribute() == true)
			{
				if (ext->tmp_ulo[0] <= min_warning.ulg)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg_seq)[i] <= min_warning.ulg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG64:
			if (check_scalar_wattribute() == true)
			{
				if (ext->tmp_ulo64[0] <= min_warning.ulg64)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg64_seq)[i] <= min_warning.ulg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ENCODED:
			if (check_scalar_wattribute() == true)
			{
				for (unsigned int i = 0;i < tmp_enc[0].encoded_data.length();i++)
				{
					if (tmp_enc[0].encoded_data[i] <= min_warning.uch)
					{
						returned = true;
						break;
					}
				}
			}
			else
			{
				for (unsigned int i = 0;i < (*value.enc_seq)[0].encoded_data.length();i++)
				{
					if ((*value.enc_seq)[0].encoded_data[i] <= min_warning.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;
		}
		if (returned == true)
		{
			quality = Tango::ATTR_WARNING;
			alarm.set(min_warn);
			real_returned = true;
		}
	}

//
// Check the max warning if defined
//

	returned = false;
	if (alarm_conf.test(max_warn) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_sh[0] >= max_warning.sh)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.sh_seq)[i] >= max_warning.sh)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_lo[0] >= max_warning.lg)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg_seq)[i] >= max_warning.lg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_LONG64:
			if (check_scalar_wattribute() == true)
			{
				if (ext->tmp_lo64[0] >= max_warning.lg64)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.lg64_seq)[i] >= max_warning.lg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_DOUBLE:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_db[0] >= max_warning.db)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.db_seq)[i] >= max_warning.db)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_FLOAT:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_fl[0] >= max_warning.fl)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.fl_seq)[i] >= max_warning.fl)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_USHORT:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_ush[0] >= max_warning.ush)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ush_seq)[i] >= max_warning.ush)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_UCHAR:
			if (check_scalar_wattribute() == true)
			{
				if (tmp_cha[0] >= max_warning.uch)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.cha_seq)[i] >= max_warning.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG:
			if (check_scalar_wattribute() == true)
			{
				if (ext->tmp_ulo[0] >= max_warning.ulg)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg_seq)[i] >= max_warning.ulg)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ULONG64:
			if (check_scalar_wattribute() == true)
			{
				if (ext->tmp_ulo64[0] >= max_warning.ulg64)
				{
					returned = true;
				}
			}
			else
			{
				for (i = 0;i < data_size;i++)
				{
					if ((*value.ulg64_seq)[i] >= max_warning.ulg64)
					{
						returned = true;
						break;
					}
				}
			}
			break;

		case Tango::DEV_ENCODED:
			if (check_scalar_wattribute() == true)
			{
				for (unsigned int i = 0;i < tmp_enc[0].encoded_data.length();i++)
				{
					if (tmp_enc[0].encoded_data[i] >= max_warning.uch)
					{
						returned = true;
						break;
					}
				}
			}
			else
			{
				for (unsigned int i = 0;i < (*value.enc_seq)[0].encoded_data.length();i++)
				{
					if ((*value.enc_seq)[0].encoded_data[i] >= max_warning.uch)
					{
						returned = true;
						break;
					}
				}
			}
			break;
		}
		if (returned == true)
		{
			quality = Tango::ATTR_WARNING;
			alarm.set(max_warn);
			real_returned = true;
		}
	}

	return real_returned;
}


//+-------------------------------------------------------------------------
//
// method :		Attribute::check_scalar_wattribute
//
// description :	Check whether the attribute is a READ_WRITE or
//						READ_WITH_WRITE scalar attribute.
//
//--------------------------------------------------------------------------

bool Attribute::check_scalar_wattribute()
{
	if ((writable == Tango::READ_WRITE) ||
	    (writable == Tango::READ_WITH_WRITE))
	{
		if (data_format == Tango::SCALAR)
			return true;
	}
	return false;
}

//+-------------------------------------------------------------------------
//
// method :		Attribute::delete_seq
//
// description :	Delete the sequence created to store attribute
//			value.
//			In case of a read_attributes CORBA operation,
//			this delete will be done automatically because the
//			sequence is inserted in an CORBA::Any object which will
//			delete the sequence when it is destroyed.
//			This method is usefull only to delete the sequence
//			created in case of the attribute is read during a
//			DevState command to evaluate  device state
//
//--------------------------------------------------------------------------

void Attribute::delete_seq()
{
	switch (data_type)
	{
	case Tango::DEV_SHORT:
		delete value.sh_seq;
		break;

	case Tango::DEV_LONG:
		delete value.lg_seq;
		break;

	case Tango::DEV_LONG64:
		delete value.lg64_seq;
		break;

	case Tango::DEV_DOUBLE:
		delete value.db_seq;
		break;

	case Tango::DEV_STRING:
		delete value.str_seq;
		break;

	case Tango::DEV_FLOAT:
		delete value.fl_seq;
		break;

	case Tango::DEV_USHORT:
		delete value.ush_seq;
		break;

	case Tango::DEV_UCHAR:
		delete value.cha_seq;
		break;

	case Tango::DEV_BOOLEAN:
		delete value.boo_seq;
		break;

	case Tango::DEV_ULONG:
		delete value.ulg_seq;
		break;

	case Tango::DEV_ULONG64:
		delete value.ulg64_seq;
		break;

	case Tango::DEV_STATE:
		delete value.state_seq;
		break;

	case Tango::DEV_ENCODED:
		delete value.enc_seq;
		break;
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::add_write_value
//
// description : 	These methods add the associated writable attribute
//					value to the read attribute buffer and create a
//					sequence from the attribute internal buffer
//
// in :	val : The associated write attribute value
//
//--------------------------------------------------------------------------

void Attribute::add_write_value(Tango::DevVarShortArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_sh[1] = (*val_ptr)[0];
		value.sh_seq = new Tango::DevVarShortArray(data_size + 1,data_size + 1,tmp_sh,false);
	}
	else
	{
		long nb_read = value.sh_seq->length();
		value.sh_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.sh_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarLongArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_lo[1] = (*val_ptr)[0];
		value.lg_seq = new Tango::DevVarLongArray(data_size + 1,data_size + 1,tmp_lo,false);
	}
	else
	{
		long nb_read = value.lg_seq->length();
		value.lg_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.lg_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarLong64Array *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		ext->tmp_lo64[1] = (*val_ptr)[0];
		value.lg64_seq = new Tango::DevVarLong64Array(data_size + 1,data_size + 1,ext->tmp_lo64,false);
	}
	else
	{
		long nb_read = value.lg64_seq->length();
		value.lg64_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.lg64_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarDoubleArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_db[1] = (*val_ptr)[0];
		value.db_seq = new Tango::DevVarDoubleArray(data_size + 1,data_size + 1,tmp_db,false);
	}
	else
	{
		long nb_read = value.db_seq->length();
		value.db_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.db_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarStringArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		if (ext->scalar_str_attr_release == true)
		{
			char **strvec = Tango::DevVarStringArray::allocbuf(2);
			strvec[0] = tmp_str[0];
			strvec[1] = CORBA::string_dup((*val_ptr)[0]);
			value.str_seq = new Tango::DevVarStringArray(2,2,strvec,true);
		}
		else
		{
			tmp_str[1] = (*val_ptr)[0];
			value.str_seq = new Tango::DevVarStringArray(data_size + 1,data_size + 1,tmp_str,false);
		}
	}
	else
	{
		long nb_read = value.str_seq->length();
		value.str_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.str_seq)[nb_read + k] = CORBA::string_dup((*val_ptr)[k]);
	}
}

void Attribute::add_write_value(Tango::DevVarFloatArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_fl[1] = (*val_ptr)[0];
		value.fl_seq = new Tango::DevVarFloatArray(data_size + 1,data_size + 1,tmp_fl,false);
	}
	else
	{
		long nb_read = value.fl_seq->length();
		value.fl_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.fl_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarBooleanArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_boo[1] = (*val_ptr)[0];
		value.boo_seq = new Tango::DevVarBooleanArray(data_size + 1,data_size + 1,tmp_boo,false);
	}
	else
	{
		long nb_read = value.boo_seq->length();
		value.boo_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.boo_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarUShortArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_ush[1] = (*val_ptr)[0];
		value.ush_seq = new Tango::DevVarUShortArray(data_size + 1,data_size + 1,tmp_ush,false);
	}
	else
	{
		long nb_read = value.ush_seq->length();
		value.ush_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.ush_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarCharArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		tmp_cha[1] = (*val_ptr)[0];
		value.cha_seq = new Tango::DevVarCharArray(data_size + 1,data_size + 1,tmp_cha,false);
	}
	else
	{
		long nb_read = value.cha_seq->length();
		value.cha_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.cha_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarULongArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		ext->tmp_ulo[1] = (*val_ptr)[0];
		value.ulg_seq = new Tango::DevVarULongArray(data_size + 1,data_size + 1,ext->tmp_ulo,false);
	}
	else
	{
		long nb_read = value.ulg_seq->length();
		value.ulg_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.ulg_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarULong64Array *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		ext->tmp_ulo64[1] = (*val_ptr)[0];
		value.ulg64_seq = new Tango::DevVarULong64Array(data_size + 1,data_size + 1,ext->tmp_ulo64,false);
	}
	else
	{
		long nb_read = value.ulg64_seq->length();
		value.ulg64_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.ulg64_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevVarStateArray *val_ptr)
{
	if (data_format == Tango::SCALAR)
	{
		ext->tmp_state[1] = (*val_ptr)[0];
		value.state_seq = new Tango::DevVarStateArray(data_size + 1,data_size + 1,ext->tmp_state,false);
	}
	else
	{
		long nb_read = value.state_seq->length();
		value.state_seq->length(nb_read + val_ptr->length());
		for (unsigned int k = 0;k < val_ptr->length();k++)
			(*value.state_seq)[nb_read + k] = (*val_ptr)[k];
	}
}

void Attribute::add_write_value(Tango::DevEncoded &val_ref)
{
	tmp_enc[1] = val_ref;
	value.enc_seq = new Tango::DevVarEncodedArray(data_size + 1,data_size + 1,tmp_enc,false);
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::Attribute_2_AttributeValue
//
// description : 	Build an AttributeValue_3 object from the Attribute
//			object content
//
// in :		ptr : Pointer to the AttributeValue_3 object to
//			      be filled in.
//			ptr_4 : Pointer to the AttributeValue_4 object to be filled
//					in.
//			dev : The device to which the attribute belongs to
//
//--------------------------------------------------------------------------


void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_3 *ptr,Tango::DeviceImpl *dev)
{
	if ((name_lower == "state") || (name_lower == "status"))
	{
		ptr->quality = Tango::ATTR_VALID;
		CORBA::Any &a = ptr->value;

		if (name_lower == "state")
		{
			a <<= dev->get_state();
		}
		else
		{
			Tango::DevVarStringArray str_seq(1);
			str_seq.length(1);
			str_seq[0] = CORBA::string_dup(dev->get_status().c_str());

			a <<= str_seq;
		}

#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		ptr->time.tv_sec = (long)t.time;
		ptr->time.tv_usec = (long)(t.millitm * 1000);
		ptr->time.tv_nsec = 0;
#else
		struct timeval after;

		gettimeofday(&after,NULL);
		ptr->time.tv_sec = after.tv_sec;
		ptr->time.tv_usec = after.tv_usec;
		ptr->time.tv_nsec = 0;
#endif
		ptr->r_dim.dim_x = 1;
		ptr->r_dim.dim_y = 0;
		ptr->w_dim.dim_x = 0;
		ptr->w_dim.dim_y = 0;

		ptr->name = CORBA::string_dup(name.c_str());
	}

	else
	{
		if (quality != Tango::ATTR_INVALID)
		{
			MultiAttribute *m_attr = dev->get_device_attr();

			// Add the attribute setpoint to the value sequence

			if ((writable == Tango::READ_WRITE) ||
			    (writable == Tango::READ_WITH_WRITE))
			{
				m_attr->add_write_value(*this);
			}

			// check for alarms to position the data quality value.
			if ( is_alarmed().any() == true )
			{
				check_alarm();
			}

			Tango::DevVarShortArray *sh_seq;
			Tango::DevShort *sh_tmp_ptr;
			Tango::DevVarLongArray *lo_seq;
			Tango::DevLong *lo_tmp_ptr;
			Tango::DevVarDoubleArray *db_seq;
			Tango::DevDouble *db_tmp_ptr;
			Tango::DevVarStringArray *str_seq;
			Tango::DevString *str_tmp_ptr;
			Tango::DevVarFloatArray *fl_seq;
			Tango::DevFloat *fl_tmp_ptr;
			Tango::DevVarBooleanArray *bo_seq;
			Tango::DevBoolean *bo_tmp_ptr;
			Tango::DevVarUShortArray *ush_seq;
			Tango::DevUShort *ush_tmp_ptr;
			Tango::DevVarUCharArray *uch_seq;
			Tango::DevUChar *uch_tmp_ptr;
			Tango::DevVarLong64Array *lo64_seq;
			Tango::DevLong64 *lo64_tmp_ptr;
			Tango::DevVarULongArray *ulo_seq;
			Tango::DevULong *ulo_tmp_ptr;
			Tango::DevVarULong64Array *ulo64_seq;
			Tango::DevULong64 *ulo64_tmp_ptr;
			Tango::DevVarStateArray *state_seq;
			Tango::DevState *state_tmp_ptr;

			CORBA::Any &a = ptr->value;
			long	seq_length = value.sh_seq->length();

			switch (data_type)
			{
			case Tango::DEV_SHORT :
				sh_tmp_ptr = get_short_value()->get_buffer();
				sh_seq = new Tango::DevVarShortArray(seq_length,seq_length,sh_tmp_ptr,false);
				a <<= *sh_seq;
				delete sh_seq;
				break;

			case Tango::DEV_LONG :
				lo_tmp_ptr = get_long_value()->get_buffer();
				lo_seq = new Tango::DevVarLongArray(seq_length,seq_length,lo_tmp_ptr,false);
				a <<= *lo_seq;
				delete lo_seq;
				break;

			case Tango::DEV_LONG64 :
				lo64_tmp_ptr = get_long64_value()->get_buffer();
				lo64_seq = new Tango::DevVarLong64Array(seq_length,seq_length,lo64_tmp_ptr,false);
				a <<= *lo64_seq;
				delete lo64_seq;
				break;

			case Tango::DEV_DOUBLE :
				db_tmp_ptr = get_double_value()->get_buffer();
				db_seq = new Tango::DevVarDoubleArray(seq_length,seq_length,db_tmp_ptr,false);
				a <<= *db_seq;
				delete db_seq;
				break;

			case Tango::DEV_STRING :
				str_tmp_ptr = get_string_value()->get_buffer();
				str_seq = new Tango::DevVarStringArray(seq_length,seq_length,str_tmp_ptr,false);
				a <<= *str_seq;
				delete str_seq;
				break;

			case Tango::DEV_FLOAT :
				fl_tmp_ptr = get_float_value()->get_buffer();
				fl_seq = new Tango::DevVarFloatArray(seq_length,seq_length,fl_tmp_ptr,false);
				a <<= *fl_seq;
				delete fl_seq;
				break;

			case Tango::DEV_BOOLEAN :
				bo_tmp_ptr = get_boolean_value()->get_buffer();
				bo_seq = new Tango::DevVarBooleanArray(seq_length,seq_length,bo_tmp_ptr,false);
				a <<= *bo_seq;
				delete bo_seq;
				break;

			case Tango::DEV_USHORT :
				ush_tmp_ptr = get_ushort_value()->get_buffer();
				ush_seq = new Tango::DevVarUShortArray(seq_length,seq_length,ush_tmp_ptr,false);
				a <<= *ush_seq;
				delete ush_seq;
				break;

			case Tango::DEV_UCHAR :
				uch_tmp_ptr = get_uchar_value()->get_buffer();
				uch_seq = new Tango::DevVarUCharArray(seq_length,seq_length,uch_tmp_ptr,false);
				a <<= *uch_seq;
				delete uch_seq;
				break;

			case Tango::DEV_ULONG :
				ulo_tmp_ptr = get_ulong_value()->get_buffer();
				ulo_seq = new Tango::DevVarULongArray(seq_length,seq_length,ulo_tmp_ptr,false);
				a <<= *ulo_seq;
				delete ulo_seq;
				break;

			case Tango::DEV_ULONG64 :
				ulo64_tmp_ptr = get_ulong64_value()->get_buffer();
				ulo64_seq = new Tango::DevVarULong64Array(seq_length,seq_length,ulo64_tmp_ptr,false);
				a <<= *ulo64_seq;
				delete ulo64_seq;
				break;

			case Tango::DEV_STATE :
				state_tmp_ptr = get_state_value()->get_buffer();
				state_seq = new Tango::DevVarStateArray(seq_length,seq_length,state_tmp_ptr,false);
				a <<= *state_seq;
				delete state_seq;
				break;
			}

			ptr->r_dim.dim_x = dim_x;
			ptr->r_dim.dim_y = dim_y;
			if ((writable == Tango::READ_WRITE) ||
			     (writable == Tango::READ_WITH_WRITE))
			{
				WAttribute &assoc_att = m_attr->get_w_attr_by_ind(get_assoc_ind());
				ptr->w_dim.dim_x = assoc_att.get_w_dim_x();
				ptr->w_dim.dim_y = assoc_att.get_w_dim_y();
			}
			else
			{
				ptr->w_dim.dim_x = 0;
				ptr->w_dim.dim_y = 0;
			}
		}
		else
		{
			ptr->r_dim.dim_x = 0;
			ptr->r_dim.dim_y = 0;
			ptr->w_dim.dim_x = 0;
			ptr->w_dim.dim_y = 0;
		}

		ptr->time = when;
		ptr->quality = quality;
		ptr->name = CORBA::string_dup(name.c_str());
	}
}


void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_4 *ptr_4,Tango::DeviceImpl *dev)
{
	if ((name_lower == "state") || (name_lower == "status"))
	{
		ptr_4->quality = Tango::ATTR_VALID;

		if (name_lower == "state")
		{
			ptr_4->value.dev_state_att(dev->get_state());
		}
		else
		{
			Tango::DevVarStringArray str_seq(1);
			str_seq.length(1);
			str_seq[0] = CORBA::string_dup(dev->get_status().c_str());

			ptr_4->value.string_att_value(str_seq);
		}

#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		ptr_4->time.tv_sec = (long)t.time;
		ptr_4->time.tv_usec = (long)(t.millitm * 1000);
		ptr_4->time.tv_nsec = 0;
#else
		struct timeval after;

		gettimeofday(&after,NULL);

		ptr_4->time.tv_sec = after.tv_sec;
		ptr_4->time.tv_usec = after.tv_usec;
		ptr_4->time.tv_nsec = 0;
#endif
		ptr_4->r_dim.dim_x = 1;
		ptr_4->r_dim.dim_y = 0;
		ptr_4->w_dim.dim_x = 0;
		ptr_4->w_dim.dim_y = 0;

		ptr_4->name = CORBA::string_dup(name.c_str());
		ptr_4->data_format = data_format;
	}
	else
	{
		if (quality != Tango::ATTR_INVALID)
		{
			MultiAttribute *m_attr = dev->get_device_attr();

			// Add the attribute setpoint to the value sequence

			if ((writable == Tango::READ_WRITE) ||
			    (writable == Tango::READ_WITH_WRITE))
			{
				m_attr->add_write_value(*this);
			}

			// check for alarms to position the data quality value.
			if ( is_alarmed().any() == true )
			{
				check_alarm();
			}

			Tango::AttributeValueList_4 dummy_list(1,1,ptr_4,false);
			dev->data_into_net_object(*this,(Tango::AttributeValueList_3 *)NULL,&dummy_list,0,writable,false);

			ptr_4->r_dim.dim_x = dim_x;
			ptr_4->r_dim.dim_y = dim_y;
			if ((writable == Tango::READ_WRITE) ||
			     (writable == Tango::READ_WITH_WRITE))
			{
				WAttribute &assoc_att = m_attr->get_w_attr_by_ind(get_assoc_ind());
				ptr_4->w_dim.dim_x = assoc_att.get_w_dim_x();
				ptr_4->w_dim.dim_y = assoc_att.get_w_dim_y();
			}
			else
			{
				ptr_4->w_dim.dim_x = 0;
				ptr_4->w_dim.dim_y = 0;
			}
		}
		else
		{
			ptr_4->r_dim.dim_x = 0;
			ptr_4->r_dim.dim_y = 0;
			ptr_4->w_dim.dim_x = 0;
			ptr_4->w_dim.dim_y = 0;
			ptr_4->value.union_no_data(true);
		}

		ptr_4->time = when;
		ptr_4->quality = quality;
		ptr_4->data_format = data_format;
		ptr_4->name = CORBA::string_dup(name.c_str());
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::AttributeValue_4_2_AttributeValue_3
//
// description : 	Build an AttributeValue_3 object from the AttributeValue_4
//					object. This method is used in case an event is requested
//					by a client knowing only IDL release 3
//
// in :		ptr_4 : Pointer to the AttributeValue_4 object
//			ptr_3 : Pointer to the AttributeValue_3 object to be filled in
//
//--------------------------------------------------------------------------


void Attribute::AttributeValue_4_2_AttributeValue_3(const Tango::AttributeValue_4 *ptr_4,Tango::AttributeValue_3 *ptr_3)
{

//
// First copy the data
//

	if (ptr_4->quality != Tango::ATTR_INVALID)
	{
		switch (ptr_4->value._d())
		{
			case ATT_BOOL:
			{
				const DevVarBooleanArray &tmp_seq = ptr_4->value.bool_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_SHORT:
			{
				const DevVarShortArray &tmp_seq = ptr_4->value.short_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_LONG:
			{
				const DevVarLongArray &tmp_seq = ptr_4->value.long_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_LONG64:
			{
				const DevVarLong64Array &tmp_seq = ptr_4->value.long64_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_FLOAT:
			{
				const DevVarFloatArray &tmp_seq = ptr_4->value.float_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_DOUBLE:
			{
				const DevVarDoubleArray &tmp_seq = ptr_4->value.double_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_UCHAR:
			{
				const DevVarCharArray &tmp_seq = ptr_4->value.uchar_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_USHORT:
			{
				const DevVarUShortArray &tmp_seq = ptr_4->value.ushort_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_ULONG:
			{
				const DevVarULongArray &tmp_seq = ptr_4->value.ulong_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_ULONG64:
			{
				const DevVarULong64Array &tmp_seq = ptr_4->value.ulong64_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_STRING:
			{
				const DevVarStringArray &tmp_seq = ptr_4->value.string_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case ATT_STATE:
			{
				const DevVarStateArray &tmp_seq = ptr_4->value.state_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case DEVICE_STATE:
			{
				const DevState &sta = ptr_4->value.dev_state_att();
				ptr_3->value <<= sta;
			}
			break;

			case ATT_ENCODED:
			{
				const DevVarEncodedArray &tmp_seq = ptr_4->value.encoded_att_value();
				ptr_3->value <<= tmp_seq;
			}
			break;

			case NO_DATA:
			break;
		}
	}

//
// The remaining fields
//

	ptr_3->time = ptr_4->time;
	ptr_3->quality = ptr_4->quality;
	ptr_3->name = ptr_4->name;

	ptr_3->r_dim = ptr_4->r_dim;
	ptr_3->w_dim = ptr_4->w_dim;

	ptr_3->err_list = ptr_4->err_list;
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::fire_change_event
//
// description : 	Fire a change change event for the attribute value.
// in :			   ptr : Pointer to a DevFailed exception to fire in case of
// 							an error to indicate.
//
//--------------------------------------------------------------------------


void Attribute::fire_change_event(DevFailed *except)
{
	cout4 << "Attribute::fire_change_event() entring ..." << endl;

	if ( except != NULL )
	{
		set_value_flag(false);
	}

//
// Check if it is needed to send an event
//

	Tango::AttributeValue_3 *send_attr = NULL;
	Tango::AttributeValue_4 *send_attr_4 = NULL;

	try
	{
		time_t now;
		int change_subscription;

		now = time(NULL);
		change_subscription = (int)now - ext->event_change_subscription;

		if (change_subscription > EVENT_RESUBSCRIBE_PERIOD)
		{
			if ( (name_lower != "state") && (name_lower != "status"))
			{
				// delete the data values allocated in the attribute
				bool data_flag = get_value_flag();
				if ( data_flag == true )
				{
					// For writable scalar attributes the sequence for the
					// attribute data is not yet allocated. This will happen
					// only when adding the set point!
					if ( !check_scalar_wattribute() )
					{
						if (quality != Tango::ATTR_INVALID)
							delete_seq();
						set_value_flag (false);
					}
				}
			}
			return;
		}

//
// Get the event supplier, and simply return if not created
//

		EventSupplier *event_supplier_nd = NULL;
		EventSupplier *event_supplier_zmq = NULL;

		Tango::Util *tg = Util::instance();
		if (use_notifd_event() == true)
            event_supplier_nd = tg->get_notifd_event_supplier();
        if (use_zmq_event() == true)
            event_supplier_zmq = tg->get_zmq_event_supplier();

		if ((event_supplier_nd == NULL) && (event_supplier_zmq == NULL))
		{
			if ( name_lower != "state" )
			{
				// delete the data values allocated in the attribute
				bool data_flag = get_value_flag();
				if ( data_flag == true )
				{
					// For writable scalar attributes the sequence for the
					// attribute data is not yet allcoated. This will happen
					// only when adding the set point!
					if ( !check_scalar_wattribute() )
					{
						if (quality != Tango::ATTR_INVALID)
							delete_seq();
						set_value_flag (false);
					}
				}
			}
			return;
		}

//
// Retrieve device object if not already done
//

		if (ext->dev == NULL)
			ext->dev = tg->get_device_by_name(ext->d_name);

		if ( except == NULL )
		{
//
// Check that the attribute value has been set
//

			if ((name_lower != "state") && (name_lower != "status"))
			{
				if (quality != Tango::ATTR_INVALID)
				{
					if (value_flag == false)
					{
						TangoSys_OMemStream o;

						o << "Value for attribute ";
						o << name;
						o << " has not been updated. Can't send change event\n";
						o << "Set the attribute value (using set_value(...) method) before!" << ends;

						Except::throw_exception((const char *)"API_AttrValueNotSet",o.str(),
				        		(const char *)"Attribute::fire_change_event");
					}
				}
			}
		}

//
// Build one AttributeValue_3 or AttributeValue_4 object
//

		try
		{
			if (ext->dev->get_dev_idl_version() >= 4)
			{
				if (ext->event_change_client_3 == true)
					send_attr = new Tango::AttributeValue_3;
				else
					send_attr_4 = new Tango::AttributeValue_4;
			}
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)"API_MemoryAllocation",
				      	  (const char *)"Can't allocate memory in server",
				      	  (const char *)"Attribute::fire_change_event");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr != NULL)
				Attribute_2_AttributeValue(send_attr,ext->dev);
			else
				Attribute_2_AttributeValue(send_attr_4,ext->dev);
		}

//
// Create the structure used to send data to event system
//

        EventSupplier::AttributeData ad;
        ::memset(&ad,0,sizeof(ad));

//
// Fire event
//

		if ( is_check_change_criteria() == true )
		{

            if (send_attr != NULL)
                ad.attr_val_3 = send_attr;
            else
                ad.attr_val_4 = send_attr_4;

//
// Eventually push the event (if detected)
// When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event
// is detected.
//

            bool send_event = false;
            if (event_supplier_nd != NULL)
                send_event = event_supplier_nd->detect_and_push_change_event(ext->dev,ad,*this,name,except,true);
            if (event_supplier_zmq != NULL)
            {
                if (event_supplier_nd != NULL)
                {
                    if (send_event == true)
                    {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        event_supplier_zmq->push_event(ext->dev,"change",f_names,f_data,f_names_lg,f_data_lg,ad,name,except);
                    }
                }
                else
                    event_supplier_zmq->detect_and_push_change_event(ext->dev,ad,*this,name,except,true);
            }
		}
		else
		{

//
// Send event, if the read_attribute failed or if it is the first time
// that the read_attribute succeed after a failure.
// Same thing if the attribute quality factor changes to INVALID
//
// This is done only to be able to set-up the same filters with events
// comming with the standard mechanism or coming from a manual fire event call.
//

			bool force_change   = false;
			bool quality_change = false;

			if ((except != NULL) ||
				(quality == Tango::ATTR_INVALID) ||
				((except == NULL) && (ext->prev_change_event.err == true)) ||
				((quality != Tango::ATTR_INVALID) &&
				(ext->prev_change_event.quality == Tango::ATTR_INVALID)))
			{
				force_change = true;
			}

			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
			{
				ext->prev_change_event.err    = true;
				ext->prev_change_event.except = *except;
			}
			else
			{
				Tango::AttrQuality the_quality;

				if (send_attr_4 != NULL)
				{
					the_quality = send_attr_4->quality;
					ext->prev_change_event.value_4 = send_attr_4->value;
				}
				else
				{
					the_quality = send_attr->quality;
					ext->prev_change_event.value = send_attr->value;
				}

				if (ext->prev_change_event.quality !=  the_quality)
				{
					quality_change = true;
				}

				ext->prev_change_event.quality = the_quality;
				ext->prev_change_event.err = false;
			}
			ext->prev_change_event.inited = true;

			filterable_names.push_back("forced_event");
			if (force_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);

			filterable_names.push_back("quality");
			if (quality_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);

            if (send_attr != NULL)
                ad.attr_val_3 = send_attr;
            else
                ad.attr_val_4 = send_attr_4;

//
// Finally push the event(s)
//

            if (event_supplier_nd != NULL)
                event_supplier_nd->push_event(ext->dev,
                                        "change",
                                        filterable_names,
                                        filterable_data,
                                        filterable_names_lg,
                                        filterable_data_lg,
                                        ad,
                                        name,
                                        except);
            if (event_supplier_zmq != NULL)
                event_supplier_zmq->push_event(ext->dev,
                                        "change",
                                        filterable_names,
                                        filterable_data,
                                        filterable_names_lg,
                                        filterable_data_lg,
                                        ad,
                                        name,
                                        except);
		}

//
// Return allocated memory
//

		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;

//
// Delete the data values allocated in the attribute
//

		if ( (name_lower != "state") && (name_lower != "status") )
		{
			bool data_flag = get_value_flag();
			if ( data_flag == true )
			{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
				set_value_flag (false);
			}
		}
	}
	catch (...)
	{
		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;

		if ( (name_lower != "state") && (name_lower != "status"))
		{

// delete the data values allocated in the attribute

			bool data_flag = get_value_flag();
			if ( data_flag == true )
			{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
				set_value_flag (false);
			}
		}

		throw;
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::fire_archive_event
//
// description : 	Fire a archive change event for the attribute value.
// in :			   ptr : Pointer to a DevFailed exception to fire in case of
// 							an error to indicate.
//
//--------------------------------------------------------------------------


void Attribute::fire_archive_event(DevFailed *except)
{
	cout4 << "Attribute::fire_archive_event() entring ..." << endl;

	if ( except != NULL )
	{
		set_value_flag (false);
	}

//
// Check if it is needed to send an event
//

	Tango::AttributeValue_3 *send_attr = NULL;
	Tango::AttributeValue_4 *send_attr_4 = NULL;

	try
	{
		time_t now;
		int archive_subscription;

		now = time(NULL);
		archive_subscription = (int)now - ext->event_archive_subscription;

		if (archive_subscription > EVENT_RESUBSCRIBE_PERIOD)
        {
			if ( (name_lower != "state") && (name_lower != "status") )
            {

//
// Delete the data values allocated in the attribute
//

				bool data_flag = get_value_flag();
				if ( data_flag == true )
                {

//
// For writable scalar attributes the sequence for the
// attribute data is not yet allcoated. This will happen
// only when adding the set point!
//

					if ( !check_scalar_wattribute() )
					{
						if (quality != Tango::ATTR_INVALID)
							delete_seq();
						set_value_flag (false);
					}
                }
            }
			return;
        }

//
// Get the event supplier, and simply return if not created
//

		EventSupplier *event_supplier_nd = NULL;
		EventSupplier *event_supplier_zmq = NULL;

		Tango::Util *tg = Util::instance();
		if (use_notifd_event() == true)
            event_supplier_nd = tg->get_notifd_event_supplier();
        if (use_zmq_event() == true)
            event_supplier_zmq = tg->get_zmq_event_supplier();

		if ((event_supplier_nd == NULL) && (event_supplier_zmq == NULL))
        {
			if ( name_lower != "state" )
            {

//
// Delete the data values allocated in the attribute
//

                bool data_flag = get_value_flag();
				if ( data_flag == true )
                {

//
// For writable scalar attributes the sequence for the
// attribute data is not yet allcoated. This will happen
// only when adding the set point!
//

					if ( !check_scalar_wattribute() )
					{
						if (quality != Tango::ATTR_INVALID)
							delete_seq();
						set_value_flag (false);
					}
                }
            }
			return;
        }

//
// Retrieve device object if not already done
//

		if (ext->dev == NULL)
			ext->dev = tg->get_device_by_name(ext->d_name);

		if ( except == NULL )
		{

//
// Check that the attribute value has been set
//

			if ((name_lower != "state") && (name_lower != "status"))
			{
				if (quality != Tango::ATTR_INVALID)
				{
					if (value_flag == false)
					{
						TangoSys_OMemStream o;

						o << "Value for attribute ";
						o << name;
						o << " has not been updated. Can't send archive event\n";
						o << "Set the attribute value (using set_value(...) method) before!" << ends;

						Except::throw_exception((const char *)"API_AttrValueNotSet",o.str(),
				        		(const char *)"Attribute::fire_archive_event");
					}
				}
			}
		}

//
// Build one AttributeValue_3 or AttributeValue_4 object
//

		try
		{
			if (ext->dev->get_dev_idl_version() >= 4)
			{
				if (ext->event_archive_client_3 == true)
					send_attr = new Tango::AttributeValue_3;
				else
					send_attr_4 = new Tango::AttributeValue_4;
			}
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)"API_MemoryAllocation",
				      	  (const char *)"Can't allocate memory in server",
				      	  (const char *)"Attribute::fire_archive_event");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr != NULL)
				Attribute_2_AttributeValue(send_attr,ext->dev);
			else
				Attribute_2_AttributeValue(send_attr_4,ext->dev);
		}

//
// Create the structure used to send data to event system
//

        EventSupplier::AttributeData ad;
        ::memset(&ad,0,sizeof(ad));

        if (send_attr != NULL)
            ad.attr_val_3 = send_attr;
        else
            ad.attr_val_4 = send_attr_4;

//
// Fire event
//

		if ( is_check_archive_criteria() == true )
		{
#ifdef _TG_WINDOWS_
        	struct _timeb           now_win;
#endif
        	struct timeval          now_timeval;

#ifdef _TG_WINDOWS_
			_ftime(&now_win);
			now_timeval.tv_sec = (unsigned long)now_win.time;
			now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
			gettimeofday(&now_timeval,NULL);
#endif

//
// Eventually push the event (if detected)
// When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event
// is detected.
//

            bool send_event = false;
            if (event_supplier_nd != NULL)
                send_event = event_supplier_nd->detect_and_push_archive_event(ext->dev,ad,*this,name,except,&now_timeval,true);
            if (event_supplier_zmq != NULL)
            {
                if (event_supplier_nd != NULL)
                {
                    if (send_event == true)
                    {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        event_supplier_zmq->push_event(ext->dev,"archive",f_names,f_data,f_names_lg,f_data_lg,ad,name,except);
                    }
                }
                else
                    event_supplier_zmq->detect_and_push_archive_event(ext->dev,ad,*this,name,except,&now_timeval,true);
            }
		}
		else
		{

//
// Execute detect_change only to calculate the delta_change_rel and
// delta_change_abs and force_change !
//

			bool force_change   = false;
			bool quality_change = false;
			double delta_change_rel = 0.0;
			double delta_change_abs = 0.0;

            if (event_supplier_nd != NULL)
                event_supplier_nd->detect_change(*this, ad,true,
							delta_change_rel,
							delta_change_abs,
							except,
							force_change,
							ext->dev);
            else if (event_supplier_zmq != NULL)
                event_supplier_zmq->detect_change(*this, ad,true,
							delta_change_rel,
							delta_change_abs,
							except,
							force_change,
							ext->dev);


			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
			{
				ext->prev_archive_event.err    = true;
				ext->prev_archive_event.except = *except;
			}
			else
			{
				Tango::AttrQuality the_quality;

				if (send_attr_4 != NULL)
				{
					ext->prev_archive_event.value_4 = send_attr_4->value;
					the_quality = send_attr_4->quality;
				}
				else
				{
					ext->prev_archive_event.value = send_attr->value;
					the_quality = send_attr->quality;
				}

				if (ext->prev_archive_event.quality !=  the_quality)
				{
					quality_change = true;
				}

				ext->prev_archive_event.quality = the_quality;
				ext->prev_archive_event.err = false;
			}
			ext->prev_archive_event.inited = true;

			filterable_names.push_back("forced_event");
			if (force_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);

			filterable_names.push_back("quality");
			if (quality_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);

			filterable_names.push_back("counter");
			filterable_data_lg.push_back(-1);

			filterable_names.push_back("delta_change_rel");
			filterable_data.push_back(delta_change_rel);
			filterable_names.push_back("delta_change_abs");
			filterable_data.push_back(delta_change_abs);

            if (event_supplier_nd != NULL)
                event_supplier_nd->push_event(ext->dev,
							"archive",
							filterable_names,
							filterable_data,
							filterable_names_lg,
							filterable_data_lg,
							ad,
							name,
							except);
            if (event_supplier_zmq != NULL)
                event_supplier_zmq->push_event(ext->dev,
							"archive",
							filterable_names,
							filterable_data,
							filterable_names_lg,
							filterable_data_lg,
							ad,
							name,
							except);
		}

		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;

//
// Delete the data values allocated in the attribute
//

		if ((name_lower != "state") && (name_lower != "status"))
			{
			bool data_flag = get_value_flag();
			if ( data_flag == true )
				{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
				set_value_flag (false);
				}
			}
		}
	catch (...)
	{
		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;

//
// Delete the data values allocated in the attribute
//

		if ((name_lower != "state") && (name_lower != "status"))
		{
			bool data_flag = get_value_flag();
			if ( data_flag == true )
			{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
				set_value_flag (false);
			}
		}

		throw;
	}
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::fire_event
//
// description : 	Fire a user event for the attribute value.
//
// in :			filt_names : The filterable fields name
//			filt_vals : The filterable fields value (as double)
//			except : Pointer to a DevFailed exception to fire in case of
// 			         an error to indicate.
//
//--------------------------------------------------------------------------


void Attribute::fire_event(vector<string> &filt_names,vector<double> &filt_vals,DevFailed *except)
{
	cout4 << "Attribute::fire_event() entring ..." << endl;

	if (except != NULL)
		set_value_flag(false);

	Tango::AttributeValue_3 *send_attr = NULL;
	Tango::AttributeValue_4 *send_attr_4 = NULL;

//
// Check if it is needed to send an event
//

	try
	{

//
// Get the event supplier, and simply return if not created
//

		EventSupplier *event_supplier_nd = NULL;
		EventSupplier *event_supplier_zmq = NULL;

		Tango::Util *tg = Util::instance();
		if (use_notifd_event() == true)
            event_supplier_nd = tg->get_notifd_event_supplier();
        if (use_zmq_event() == true)
            event_supplier_zmq = tg->get_zmq_event_supplier();

		if ((event_supplier_nd == NULL) && (event_supplier_zmq == NULL))
		{
			if (name_lower != "state")
			{

//
// Delete the data values allocated in the attribute
//

				bool data_flag = get_value_flag();
				if ( data_flag == true )
				{

//
// For writable scalar attributes the sequence for the
// attribute data is not yet allcoated. This will happen
// only when adding the set point!
//

					if ( !check_scalar_wattribute() )
					{
						if (quality != Tango::ATTR_INVALID)
							delete_seq();
						set_value_flag (false);
					}
				}
			}
			return;
		}

//
// Retrieve device object if not already done
//

		if (ext->dev == NULL)
			ext->dev = tg->get_device_by_name(ext->d_name);

		if (except == NULL)
		{

//
// Check that the attribute value has been set
//

			if ((name_lower != "state") && (name_lower != "status"))
			{
				if (quality != Tango::ATTR_INVALID)
				{
					if (value_flag == false)
					{
						TangoSys_OMemStream o;

						o << "Value for attribute ";
						o << name;
						o << " has not been updated. Can't send user event\n";
						o << "Set the attribute value (using set_value(...) method) before!" << ends;

						Except::throw_exception((const char *)"API_AttrValueNotSet",o.str(),
				        		(const char *)"Attribute::fire_event");
					}
				}
			}
		}

//
// Build one AttributeValue_3 or AttributeValue_4 object
//

		try
		{
			if (ext->dev->get_dev_idl_version() >= 4)
			{
				if (ext->event_user_client_3 == true)
					send_attr = new Tango::AttributeValue_3;
				else
					send_attr_4 = new Tango::AttributeValue_4;
			}
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)"API_MemoryAllocation",
				      	  (const char *)"Can't allocate memory in server",
				      	  (const char *)"Attribute::fire_event");
		}

//
// don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr != NULL)
				Attribute_2_AttributeValue(send_attr,ext->dev);
			else
				Attribute_2_AttributeValue(send_attr_4,ext->dev);
		}

//
// Create the structure used to send data to event system
//

        EventSupplier::AttributeData ad;
        ::memset(&ad,0,sizeof(ad));

        if (send_attr != NULL)
            ad.attr_val_3 = send_attr;
        else
            ad.attr_val_4 = send_attr_4;

//
// Fire event
//

		vector<string> filterable_names_lg;
		vector<long> filterable_data_lg;

        if (event_supplier_nd != NULL)
            event_supplier_nd->push_event(ext->dev,
					   "user_event",
					   filt_names,
					   filt_vals,
					   filterable_names_lg,
					   filterable_data_lg,
					   ad,
					   name,
					   except);
        if (event_supplier_zmq != NULL)
            event_supplier_zmq->push_event(ext->dev,
					   "user_event",
					   filt_names,
					   filt_vals,
					   filterable_names_lg,
					   filterable_data_lg,
					   ad,
					   name,
					   except);


		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;

//
// delete the data values allocated in the attribute
//

		if ((name_lower != "state") && (name_lower != "status"))
		{
			bool data_flag = get_value_flag();
			if ( data_flag == true )
			{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
				set_value_flag (false);
			}
		}
	}
	catch (...)
	{

		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;

//
// delete the data values allocated in the attribute
//

		if ((name_lower != "state") && (name_lower != "status"))
		{
			bool data_flag = get_value_flag();
			if ( data_flag == true )
			{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
				set_value_flag (false);
			}
		}

		throw;
	}
}


//+-------------------------------------------------------------------------
//
// operator overloading : 	set_quality
//
// description : 	Set the attribute quality factor
//
//--------------------------------------------------------------------------

void Attribute::set_quality(Tango::AttrQuality qua,bool send_event)
{
	quality = qua;
	if (send_event == true)

		fire_change_event();

}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::upd_database
//
// description : 	Update the tango database with the new attribute
//			values
//
//--------------------------------------------------------------------------

void Attribute::upd_att_prop_db(Tango::Attr_CheckVal &new_value,
				const char *prop_name)
{
	cout4 << "Entering upd_att_prop_db method for attribute " << name <<", property = " << prop_name << endl;

//
// Build the data sent to database
//

	Tango::DbData db_data;
	Tango::DbDatum att(name),prop(prop_name);
	att << (short)1;

	switch (data_type)
	{
	case Tango::DEV_SHORT:
		prop << new_value.sh;
		break;

	case Tango::DEV_LONG:
		prop << new_value.lg;
		break;

	case Tango::DEV_LONG64:
		prop << new_value.lg64;
		break;

	case Tango::DEV_DOUBLE:
		prop << new_value.db;
		break;

	case Tango::DEV_FLOAT:
		prop << new_value.fl;
		break;

	case Tango::DEV_USHORT:
		prop << new_value.ush;
		break;

	case Tango::DEV_UCHAR:
		prop << new_value.uch;
		break;

	case Tango::DEV_ULONG:
		prop << new_value.ulg;
		break;

	case Tango::DEV_ULONG64:
		prop << new_value.ulg64;
		break;

	case Tango::DEV_STATE:
		prop << (short)new_value.d_sta;
		break;

	}

	db_data.push_back(att);
	db_data.push_back(prop);

//
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
//

	Tango::Util *tg = Tango::Util::instance();
	bool retry = true;
	while (retry == true)
	{
		try
		{
			tg->get_database()->put_device_attribute_property(ext->d_name,db_data);
			retry = false;
		}
		catch (CORBA::COMM_FAILURE)
		{
			tg->get_database()->reconnect(true);
		}
	}

}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::remove_configuration()
//
// description : 	Remove the attribute configuration from the database.
// 					This method can be used to clean-up all the configuration
//                  of an attribute to come back to its default values or the
//                  remove all configuration of a dynamic attribute before deleting it.
//
// 					The method removes all configured attribute properties
//                  and removes the attribute from the list of polled attributes.
//--------------------------------------------------------------------------

void Attribute::remove_configuration()
{
	cout4 << "Entering remove_configuration() method for attribute " << name << endl;

	Tango::Util *tg = Tango::Util::instance();

//
// read all configured properties of the attribute from the database and
// delete them!
//

	DbData db_read_data;
	DbData db_delete_data;

	db_read_data.push_back(DbDatum(name));
 	db_delete_data.push_back(DbDatum(name));

//
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
//

	bool retry = true;
	while (retry == true)
	{
		try
		{
			tg->get_database()->get_device_attribute_property(ext->d_name,db_read_data);
			retry = false;
		}
		catch (CORBA::COMM_FAILURE)
		{
			tg->get_database()->reconnect(true);
		}
	}


    long nb_prop = 0;
    db_read_data[0] >> nb_prop;

    for (int k=1; k<(nb_prop + 1); k++)
    {
    	string &prop_name = db_read_data[k].name;
		db_delete_data.push_back(DbDatum(prop_name));
    }

//
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
//

	if ( nb_prop > 0 )
	{
		retry = true;
		while (retry == true)
		{
			try
			{
				tg->get_database()->delete_device_attribute_property(ext->d_name,db_delete_data);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_att_device
//
// description : 	Return a pointer to the attribute device
//
//--------------------------------------------------------------------------

DeviceImpl *Attribute::get_att_device()
{
	if (ext->dev == NULL)
	{
		Tango::Util *tg = Tango::Util::instance();
		ext->dev = tg->get_device_by_name(ext->d_name);
	}

	return ext->dev;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_att_device
//
// description : 	Return a pointer to the attribute device
//
//--------------------------------------------------------------------------

void Attribute::set_attr_serial_model(AttrSerialModel ser_model)
{
	if (ser_model == Tango::ATTR_BY_USER)
	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->get_serial_model() != Tango::BY_DEVICE)
		{
			Except::throw_exception((const char *)"API_AttrNotAllowed",
				      	  (const char *)"Attribute serial model by user is not allowed when the process is not in BY_DEVICE serialization model",
				      	  (const char *)"Attribute::set_attr_serial_model");
		}
	}

	ext->attr_serial_model=ser_model;
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::log_quality
//
// description :    Send a logging message (on the device) when the attribute
//                  quality factor changes
//
//--------------------------------------------------------------------------

void Attribute::log_quality()
{

//
// Set device if not already done
//

    if (ext->dev == NULL)
    {
        Tango::Util *tg = Tango::Util::instance();
        ext->dev = tg->get_device_by_name(ext->d_name);
    }

//
// Log something if the new quality is different than the old one
//

    if (quality != ext->old_quality)
    {
        if (alarm.any() == false)
        {

//
// No alarm detected
//

            switch(quality)
            {
                case ATTR_INVALID:
                if (ext->dev->get_logger()->is_error_enabled())
                    ext->dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "INVALID quality for attribute " << name << endl;
                break;

                case ATTR_CHANGING:
                if (ext->dev->get_logger()->is_info_enabled())
                    ext->dev->get_logger()->info_stream() << log4tango::LogInitiator::_begin_log << "CHANGING quality for attribute " << name << endl;
                break;

                case ATTR_VALID:
                if (ext->dev->get_logger()->is_info_enabled())
                    ext->dev->get_logger()->info_stream() << log4tango::LogInitiator::_begin_log << "VALID quality for attribute " << name << endl;
                break;

                default:
                break;
            }
        }
        else
        {

//
// Different log according to which alarm is set
//

            if (alarm[min_level] == true)
            {
                if (ext->dev->get_logger()->is_error_enabled())
                    ext->dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MIN ALARM for attribute " << name << endl;
            }
            else if (alarm[max_level] == true)
            {
                if (ext->dev->get_logger()->is_error_enabled())
                    ext->dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MAX ALARM for attribute " << name << endl;
            }
            else if (alarm[rds] == true)
            {
                if (ext->dev->get_logger()->is_warn_enabled())
                    ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) ALARM for attribute " << name << endl;
            }
            else if (alarm[min_warn] == true)
            {
               if (ext->dev->get_logger()->is_warn_enabled())
                    ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MIN WARNING for attribute " << name << endl;
            }
            else if (alarm[max_warn] == true)
            {
               if (ext->dev->get_logger()->is_warn_enabled())
                    ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MAX WARNING for attribute " << name << endl;
            }
        }
    }
    else
    {

//
// The quality is the same but may be the alarm has changed
//

        if (alarm != ext->old_alarm)
        {
            if (alarm[min_level] == true)
            {
                if (ext->dev->get_logger()->is_error_enabled())
                    ext->dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MIN ALARM for attribute " << name << endl;
            }
            else if (alarm[max_level] == true)
            {
                if (ext->dev->get_logger()->is_error_enabled())
                    ext->dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MAX ALARM for attribute " << name << endl;
            }
            else if (alarm[rds] == true)
            {
                if (ext->dev->get_logger()->is_warn_enabled())
                    ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) ALARM for attribute " << name << endl;
            }
            else if (alarm[min_warn] == true)
            {
               if (ext->dev->get_logger()->is_warn_enabled())
                    ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MIN WARNING for attribute " << name << endl;
            }
            else if (alarm[max_warn] == true)
            {
               if (ext->dev->get_logger()->is_warn_enabled())
                    ext->dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MAX WARNING for attribute " << name << endl;
            }
        }
    }
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing instance of the
//			Attribute class
//
//--------------------------------------------------------------------------

#ifndef TANGO_HAS_LOG4TANGO
ostream &operator<<(ostream &o_str,Attribute &p)
{
	Tango::AttributeConfig conf;

//
// Get attribute properties
//

	p.get_properties(conf);

//
// Print all these properties
//


	cout << "Attribute name = " << conf.name.in() << endl;
	cout << "Attribute data_type = ";
	switch (conf.data_type)
	{
	case Tango::DEV_SHORT :
		cout << "Tango::DevShort" << endl;
		break;

	case Tango::DEV_LONG :
		cout << "Tango::DevLong" << endl;
		break;

	case Tango::DEV_LONG64 :
		cout << "Tango::DevLong64" << endl;
		break;

	case Tango::DEV_DOUBLE :
		cout << "Tango::DevDouble" << endl;
		break;

	case Tango::DEV_STRING :
		cout << "Tango::DevString" << endl;
		break;

	case Tango::DEV_FLOAT :
		cout << "Tango::DevFloat" << endl;
		break;

	case Tango::DEV_USHORT :
		cout << "Tango::DevUShort" << endl;
		break;

	case Tango::DEV_UCHAR :
		cout << "Tango::DevUChar" << endl;
		break;

	case Tango::DEV_BOOLEAN :
		cout << "Tango::DevBoolean" << endl;
		break;

	case Tango::DEV_STATE :
		cout << "Tango::DevState" << endl;
		break;

	case Tango::DEV_ULONG :
		cout << "Tango::DevULong" << endl;
		break;

	case Tango::DEV_ULONG64 :
		cout << "Tango::DevULong64" << endl;
		break;

	case Tango::Dev_ENCODED :
		cout << "Tango::DevEncoded" << endl;
		break;
	}

	cout << "Attribute data_format = ";
	switch (conf.data_format)
	{
	case Tango::SCALAR :
		cout << "scalar" << endl;
		break;

	case Tango::SPECTRUM :
		cout << "spectrum, max_dim_x = " << conf.max_dim_x << endl;
		break;

	case Tango::IMAGE :
		cout << "image, max_dim_x = " << conf.max_dim_x << ", max_dim_y = " << conf.max_dim_y << endl;
		break;
	}

	if ((conf.writable == Tango::WRITE) || (conf.writable == Tango::READ_WRITE))
		cout << "Attribute is writable" << endl;
	else
		cout << "Attribute is not writable" << endl;
	cout << "Attribute label = " << conf.label.in() << endl;
	cout << "Attribute description = " << conf.description.in() << endl;
	cout << "Attribute unit = " << conf.unit.in();
	cout << ", standard unit = " << conf.standard_unit.in();
	cout << ", display unit = " << conf.display_unit.in() << endl;
	cout << "Attribute format = " << conf.format.in() << endl;
	cout << "Attribute min value = " << conf.min_value.in() << endl;
	cout << "Attribute max value = " << conf.max_value.in() << endl;
	cout << "Attribute min alarm = " << conf.min_alarm.in() << endl;
	cout << "Attribute max alarm = " << conf.max_alarm.in() << endl;
	cout << "Attribute writable_attr_name = " << conf.writable_attr_name.in() << endl;

	return o_str;
}
#endif // TANGO_HAS_LOG4TANGO


} // End of Tango namespace
