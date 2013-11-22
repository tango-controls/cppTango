static const char *RcsId = "$Id$\n$Name$";

//====================================================================================================================
//
// file :               Attribute.cpp
//
// description :        C++ source code for the Attribute class. This class is used to manage attribute.
//						A Tango Device object instance has one MultiAttribute object which is an aggregate of
//						Attribute or WAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
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

RANGES_TYPE2CONST(Tango::DevDouble,DEV_DOUBLE)
RANGES_TYPE2CONST(Tango::DevFloat,DEV_FLOAT)
RANGES_TYPE2CONST(Tango::DevLong,DEV_LONG)
RANGES_TYPE2CONST(Tango::DevLong64,DEV_LONG64)
RANGES_TYPE2CONST(Tango::DevShort,DEV_SHORT)
RANGES_TYPE2CONST(Tango::DevUChar,DEV_UCHAR)
RANGES_TYPE2CONST(Tango::DevULong,DEV_ULONG)
RANGES_TYPE2CONST(Tango::DevULong64,DEV_ULONG64)
RANGES_TYPE2CONST(Tango::DevUShort,DEV_USHORT)
RANGES_TYPE2CONST(Tango::DevEncoded,DEV_ENCODED)

RANGES_TYPE2CONST(Tango::DevBoolean,DEV_BOOLEAN)
RANGES_TYPE2CONST(Tango::DevString,DEV_STRING)
RANGES_TYPE2CONST(Tango::DevState,DEV_STATE)

//
// A classical function which will be used as unary predicate for the find_if
// algo. It must be used with the bind2nd adapter to be transform as unary and
// with the ptr_fun adapter to be transform as a function object
//

static bool WantedProp_f(AttrProperty a,const char *n)
{
	return (a.get_name() == n);
}


//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::Attribute
//
// description :
//		Constructor for the Attribute class from the attribute property vector, its type and the device name
//
// argument :
//		in :
//			- prop_list : The attribute property list
//			- tmp_attr :
//			- dev_name : The device name
//			- idx :
//
//--------------------------------------------------------------------------------------------------------------------

Attribute::Attribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx)
:date(true),quality(Tango::ATTR_VALID),check_min_value(false),check_max_value(false),
 poll_period(0),event_period(0),archive_period(0),last_periodic(0.0),
 archive_last_periodic(0.0),periodic_counter(0),archive_periodic_counter(0),
 archive_last_event(0.0),dev(NULL),change_event_implmented(false),
 archive_event_implmented(false),check_change_event_criteria(true),
 check_archive_event_criteria(true),event_periodic_client_3(false),
 event_change_client_3(false),event_archive_client_3(false),
 event_user_client_3(false),dr_event_implmented(false),
 scalar_str_attr_release(false),notifd_event(false),zmq_event(false),
 check_startup_exceptions(false),startup_exceptions_clear(true),att_mem_exception(false)
{

//
// Create the extension class
//

	ext = new Attribute::AttributeExt();


	idx_in_attr = idx;
	d_name = dev_name;
	attr_serial_model = ATTR_BY_KERNEL;
	scalar_str_attr_release = false;

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
	disp_level = tmp_attr.get_disp_level();
	poll_period = tmp_attr.get_polling_period();
	writable_attr_name = tmp_attr.get_assoc();

//
// Init the event characteristics
//

	change_event_implmented  		= tmp_attr.is_change_event();
	check_change_event_criteria 	= tmp_attr.is_check_change_criteria();
	archive_event_implmented 		= tmp_attr.is_archive_event();
	check_archive_event_criteria 	= tmp_attr.is_check_archive_criteria();
	dr_event_implmented				= tmp_attr.is_data_ready_event();

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

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::~Attribute
//
// description :
//		Destructor for the Attribute class
//
//--------------------------------------------------------------------------------------------------------------------

Attribute::~Attribute()
{
	try
	{
		delete ext;
	}
	catch(omni_thread_fatal &){}
}


//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_event_prop
//
// description :
//		Init the event related properties
//
// argument :
// 		in :
//			- prop_list : The property vector
//			- dev_name : The device name
//			- att : The user attribute object
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::init_event_prop(vector<AttrProperty> &prop_list,const string &dev_name,Attr &att)
{
	rel_change[0] = INT_MAX;			// default for relative change is none
	rel_change[1] = INT_MAX;			// default for relative change is none
	abs_change[0] = INT_MAX;			// default for absolute change is none
	abs_change[1] = INT_MAX;			// default for absolute change is none
	archive_rel_change[0] = INT_MAX;		// default for archive change is none
	archive_rel_change[1] = INT_MAX;		// default for archive change is none
	archive_abs_change[0] = INT_MAX;		// default for archive change is none
	archive_abs_change[1] = INT_MAX;		// default for archive change is none

    notifd_event = false;
	zmq_event = false;

	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	size_t nb_user = def_user_prop.size();

//
// Init min and max relative change for change event
//

	try
	{
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
				vector<double> rel_change_tmp;
				vector<bool> rel_change_set_usr_def;
				vector<bool> unused;
				validate_change_properties(dev_name,"rel_change",rel_change_str,rel_change_tmp,rel_change_set_usr_def,unused);
				rel_change[0] = rel_change_tmp[0];
				rel_change[1] = rel_change_tmp[1];

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
								rel_change[0] = rel_change_usr_def[0];
							if(rel_change_set_usr_def[1])
								rel_change[1] = rel_change_usr_def[1];
						}
					}
				}
				cout1 << "Attribute::Attribute(): rel_change = " << rel_change[0] << " " << rel_change[1] << endl;
			}
			else
				throw_err_data_type("rel_change",dev_name,"Attribute::init_event_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("rel_change",e);
	}

//
// Init min and max absolute change for change event
//

	try
	{
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
				vector<double> abs_change_tmp;
				vector<bool> abs_change_set_usr_def;
				vector<bool> unused;
				validate_change_properties(dev_name,"abs_change",abs_change_str,abs_change_tmp,abs_change_set_usr_def,unused);
				abs_change[0] = abs_change_tmp[0];
				abs_change[1] = abs_change_tmp[1];

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
								abs_change[0] = abs_change_usr_def[0];
							if(abs_change_set_usr_def[1])
								abs_change[1] = abs_change_usr_def[1];
						}
					}
				}
				cout1 << "Attribute::Attribute(): abs_change = " << abs_change[0] << " " << abs_change[1] << endl;
			}
			else
				throw_err_data_type("abs_change",dev_name,"Attribute::init_event_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("abs_change",e);
	}

//
// Init min and max relative change for archive event
//

	try
	{
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
				vector<double> archive_rel_change_tmp;
				vector<bool> archive_rel_change_set_usr_def;
				vector<bool> unused;
				validate_change_properties(dev_name,"archive_rel_change",archive_rel_change_str,archive_rel_change_tmp,archive_rel_change_set_usr_def,unused);
				archive_rel_change[0] = archive_rel_change_tmp[0];
				archive_rel_change[1] = archive_rel_change_tmp[1];

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
								archive_rel_change[0] = archive_rel_change_usr_def[0];
							if(archive_rel_change_set_usr_def[1])
								archive_rel_change[1] = archive_rel_change_usr_def[1];
						}
					}
				}
				cout1 << "Attribute::Attribute(): archive_rel_change = " << archive_rel_change[0] << " " << archive_rel_change[1] << endl;
			}
			else
				throw_err_data_type("archive_rel_change",dev_name,"Attribute::init_event_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("archive_rel_change",e);
	}

//
// Init min and max absolute change for archive event
//

	try
	{
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
				vector<double> archive_abs_change_tmp;
				vector<bool> archive_abs_change_set_usr_def;
				vector<bool> unused;
				validate_change_properties(dev_name,"archive_abs_change",archive_abs_change_str,archive_abs_change_tmp,archive_abs_change_set_usr_def,unused);
				archive_abs_change[0] = archive_abs_change_tmp[0];
				archive_abs_change[1] = archive_abs_change_tmp[1];

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
								archive_abs_change[0] = archive_abs_change_usr_def[0];
							if(archive_abs_change_set_usr_def[1])
								archive_abs_change[1] = archive_abs_change_usr_def[1];
						}
					}
				}
				cout1 << "Attribute::Attribute(): archive_abs_change = " << archive_abs_change[0] << " " << archive_abs_change[1] << endl;
			}
			else
				throw_err_data_type("archive_abs_change",dev_name,"Attribute::init_event_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("archive_abs_change",e);
	}

//
// Init period for periodic event
//

	try
	{
		event_period = (int)(DEFAULT_EVENT_PERIOD);  	// default for event period is 1 second

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
			int event_period_tmp = 0;
			str << event_period_str;
			if(str >> event_period_tmp && str.eof())
			{
				if (event_period_tmp > 0)
					event_period = event_period_tmp;
				cout1 << "Attribute::Attribute(): event_period_str " << event_period_str << " event_period = " << event_period << endl;
			}
			else
				throw_err_format("event_period",dev_name,"Attribute::init_event_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("event_period",e);
	}

//
// Init period for archive event
//

	try
	{
		archive_period = (int)(INT_MAX);

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
			int archive_period_tmp = 0;
			str << archive_period_str;
			if(str >> archive_period_tmp && str.eof())
			{
				if (archive_period_tmp > 0)
				{
					archive_period = archive_period_tmp;
				}
				cout1 << "Attribute::Attribute(): archive_period_str " << archive_period_str << " archive_period = " << archive_period << endl;
			}
			else
				throw_err_format("archive_period",dev_name,"Attribute::init_event_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("archive_period",e);
	}

//
// Init remaining parameters
//

	periodic_counter = 0;
	archive_periodic_counter = 0;
	last_periodic = 0.;
	archive_last_periodic = 0.;
	prev_change_event.inited = false;
	prev_quality_event.inited = false;
	prev_archive_event.inited = false;

//
// do not start sending events automatically, wait for the first client to subscribe. Sending events automatically
// will put an unnecessary load on the server because all attributes will be polled
//

	event_change_subscription = 0;
	event_archive_subscription = 0;
	event_quality_subscription = 0;
	event_periodic_subscription = 0;
	event_user_subscription = 0;
	event_attr_conf_subscription = 0;
	event_data_ready_subscription = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_opt_prop
//
// description :
//		Init the optional properties
//
// argument :
// 		in :
//			- prop_list : The property vector
//			- dev_name : The device name (usefull for error)
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name)
{

//
// Init the label property
//

	try
	{
		label = get_attr_value(prop_list,"label");
	}
	catch(DevFailed &e)
	{
		add_startup_exception("label",e);
	}

//
// Init the description property
//

	try
	{
		description = get_attr_value(prop_list,"description");
	}
	catch(DevFailed &e)
	{
		add_startup_exception("description",e);
	}

//
// Init the unit property
//

	try
	{
		unit = get_attr_value(prop_list,"unit");
	}
	catch(DevFailed &e)
	{
		add_startup_exception("unit",e);
	}

//
// Init the standard unit property
//

	try
	{
		standard_unit = get_attr_value(prop_list,"standard_unit");
	}
	catch(DevFailed &e)
	{
		add_startup_exception("standard_unit",e);
	}

//
// Init the display unit property
//

	try
	{
		display_unit = get_attr_value(prop_list,"display_unit");
	}
	catch(DevFailed &e)
	{
		add_startup_exception("display_unit",e);
	}

//
// Init the format property
//

	try
	{
		format = get_attr_value(prop_list,"format");
	}
	catch(DevFailed &e)
	{
		add_startup_exception("format",e);
	}


	TangoSys_MemStream str;
	bool empty = true;

//
// Init the min alarm property
//

	try
	{
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
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_level) && min_alarm.sh >= max_alarm.sh)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> min_alarm.db && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					min_alarm.lg = (DevLong)min_alarm.db;
					if(alarm_conf.test(max_level) && min_alarm.lg >= max_alarm.lg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG64:
					if (!(str >> min_alarm.db && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					min_alarm.lg64 = (DevLong64)min_alarm.db;
					if(alarm_conf.test(max_level) && min_alarm.lg64 >= max_alarm.lg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> min_alarm.db && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_level) && min_alarm.db >= max_alarm.db)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;
				case Tango::DEV_FLOAT:
					if (!(str >> min_alarm.fl && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_level) && min_alarm.fl >= max_alarm.fl)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> min_alarm.ush && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_level) && min_alarm.ush >= max_alarm.ush)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> min_alarm.sh && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					min_alarm.uch = (DevUChar)min_alarm.sh;
					if(alarm_conf.test(max_level) && min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG:
					if (!(str >> min_alarm.db && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					min_alarm.ulg = (DevULong)min_alarm.db;
					if(alarm_conf.test(max_level) && min_alarm.ulg >= max_alarm.ulg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> min_alarm.db && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					min_alarm.ulg64 = (DevULong64)min_alarm.db;
					if(alarm_conf.test(max_level) && min_alarm.ulg64 >= max_alarm.ulg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> min_alarm.sh && str.eof()))
						throw_err_format("min_alarm",dev_name,"Attribute::init_opt_prop()");
					min_alarm.uch = (DevUChar)min_alarm.sh;
					if(alarm_conf.test(max_level) && min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;
				}
				alarm_conf.set(min_level);
			}
			else
				throw_err_data_type("min_alarm",dev_name,"Attribute::init_opt_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("min_alarm",e);
	}

//
// Init the max alarm property
//

	try
	{
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
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_level) && min_alarm.sh >= max_alarm.sh)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> max_alarm.db && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					max_alarm.lg = (DevLong)max_alarm.db;
					if(alarm_conf.test(min_level) && min_alarm.lg >= max_alarm.lg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG64:
					if (!(str >> max_alarm.db && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					max_alarm.lg64 = (DevLong64)max_alarm.db;
					if(alarm_conf.test(min_level) && min_alarm.lg64 >= max_alarm.lg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> max_alarm.db && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_level) && min_alarm.db >= max_alarm.db)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_FLOAT:
					if (!(str >> max_alarm.fl && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_level) && min_alarm.fl >= max_alarm.fl)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> max_alarm.ush && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_level) && min_alarm.ush >= max_alarm.ush)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> max_alarm.sh && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					max_alarm.uch = (DevUChar)max_alarm.sh;
					if(alarm_conf.test(min_level) && min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG:
					if (!(str >> max_alarm.db && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					max_alarm.ulg = (DevULong)max_alarm.db;
					if(alarm_conf.test(min_level) && min_alarm.ulg >= max_alarm.ulg)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> max_alarm.db && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					max_alarm.ulg64 = (DevULong64)max_alarm.db;
					if(alarm_conf.test(min_level) && min_alarm.ulg64 >= max_alarm.ulg64)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> max_alarm.sh && str.eof()))
						throw_err_format("max_alarm",dev_name,"Attribute::init_opt_prop()");
					max_alarm.uch = (DevUChar)max_alarm.sh;
					if(alarm_conf.test(min_level) && min_alarm.uch >= max_alarm.uch)
						throw_incoherent_val_err("min_alarm","max_alarm",dev_name,"Attribute::init_opt_prop()");
					break;
				}
				alarm_conf.set(max_level);
			}
			else
				throw_err_data_type("max_alarm",dev_name,"Attribute::init_opt_prop");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("max_alarm",e);
	}

//
// Init the min value property
//

	try
	{
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
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					if(check_max_value && min_value.sh >= max_value.sh)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> min_value.db && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					min_value.lg = (DevLong)min_value.db;
					if(check_max_value && min_value.lg >= max_value.lg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG64:
					if (!(str >> min_value.db && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					min_value.lg64 = (DevLong64)min_value.db;
					if(check_max_value && min_value.lg64 >= max_value.lg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> min_value.db && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					if(check_max_value && min_value.db >= max_value.db)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_FLOAT:
					if (!(str >> min_value.fl && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					if(check_max_value && min_value.fl >= max_value.fl)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> min_value.ush && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					if(check_max_value && min_value.ush >= max_value.ush)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> min_value.sh && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					min_value.uch = (DevUChar)min_value.sh;
					if(check_max_value && min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG:
					if (!(str >> min_value.db && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					min_value.ulg = (DevULong)min_value.db;
					if(check_max_value && min_value.ulg >= max_value.ulg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> min_value.db && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					min_value.ulg64 = (DevULong64)min_value.db;
					if(check_max_value && min_value.ulg64 >= max_value.ulg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> min_value.sh && str.eof()))
						throw_err_format("min_value",dev_name,"Attribute::init_opt_prop()");
					min_value.uch = (DevUChar)min_value.sh;
					if(check_max_value && min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;
				}
				check_min_value = true;
			}
			else
				throw_err_data_type("min_value",dev_name,"Attribute::init_opt_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("min_value",e);
	}

//
// Init the max value property
//

	try
	{
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
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					if(check_min_value && min_value.sh >= max_value.sh)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> max_value.db && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					max_value.lg = (DevLong)max_value.db;
					if(check_min_value && min_value.lg >= max_value.lg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG64:
					if (!(str >> max_value.db && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					max_value.lg64 = (DevLong64)max_value.db;
					if(check_min_value && min_value.lg64 >= max_value.lg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> max_value.db && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					if(check_min_value && min_value.db >= max_value.db)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_FLOAT:
					if (!(str >> max_value.fl && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					if(check_min_value && min_value.fl >= max_value.fl)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> max_value.ush && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					if(check_min_value && min_value.ush >= max_value.ush)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> max_value.sh && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					max_value.uch = (DevUChar)max_value.sh;
					if(check_min_value && min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG:
					if (!(str >> max_value.db && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					max_value.ulg = (DevULong)max_value.db;
					if(check_min_value && min_value.ulg >= max_value.ulg)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> max_value.db && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					max_value.ulg64 = (DevULong64)max_value.db;
					if(check_min_value && min_value.ulg64 >= max_value.ulg64)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> max_value.sh && str.eof()))
						throw_err_format("max_value",dev_name,"Attribute::init_opt_prop()");
					max_value.uch = (DevUChar)max_value.sh;
					if(check_min_value && min_value.uch >= max_value.uch)
						throw_incoherent_val_err("min_value","max_value",dev_name,"Attribute::init_opt_prop()");
					break;
				}
				check_max_value = true;
			}
			else
				throw_err_data_type("max_value",dev_name,"Attribute::init_opt_prop");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("max_value",e);
	}

//
// Init the min warning property
//

	try
	{
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
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_warn) && min_warning.sh >= max_warning.sh)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> min_warning.db && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					min_warning.lg = (DevLong)min_warning.db;
					if(alarm_conf.test(max_warn) && min_warning.lg >= max_warning.lg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG64:
					if (!(str >> min_warning.db && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					min_warning.lg64 = (DevLong64)min_warning.db;
					if(alarm_conf.test(max_warn) && min_warning.lg64 >= max_warning.lg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> min_warning.db && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_warn) && min_warning.db >= max_warning.db)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;
				case Tango::DEV_FLOAT:
					if (!(str >> min_warning.fl && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_warn) && min_warning.fl >= max_warning.fl)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> min_warning.ush && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(max_warn) && min_warning.ush >= max_warning.ush)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> min_warning.sh && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					min_warning.uch = (DevUChar)min_warning.sh;
					if(alarm_conf.test(max_warn) && min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG:
					if (!(str >> min_warning.db && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					min_warning.ulg = (DevULong)min_warning.db;
					if(alarm_conf.test(max_warn) && min_warning.ulg >= max_warning.ulg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> min_warning.db && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					min_warning.ulg64 = (DevULong64)min_warning.db;
					if(alarm_conf.test(max_warn) && min_warning.ulg64 >= max_warning.ulg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> min_warning.sh && str.eof()))
						throw_err_format("min_warning",dev_name,"Attribute::init_opt_prop()");
					min_warning.uch = (DevUChar)min_warning.sh;
					if(alarm_conf.test(max_warn) && min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;
				}
				alarm_conf.set(min_warn);
			}
			else
				throw_err_data_type("min_warning",dev_name,"Attribute::init_opt_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("min_warning",e);
	}

//
// Init the max warning property
//

	try
	{
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
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_warn) && min_warning.sh >= max_warning.sh)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> max_warning.db && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					max_warning.lg = (DevLong)max_warning.db;
					if(alarm_conf.test(min_warn) && min_warning.lg >= max_warning.lg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG64:
					if (!(str >> max_warning.db && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					max_warning.lg64 = (DevLong64)max_warning.db;
					if(alarm_conf.test(min_warn) && min_warning.lg64 >= max_warning.lg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> max_warning.db && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_warn) && min_warning.db >= max_warning.db)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_FLOAT:
					if (!(str >> max_warning.fl && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_warn) && min_warning.fl >= max_warning.fl)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> max_warning.ush && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					if(alarm_conf.test(min_warn) && min_warning.ush >= max_warning.ush)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> max_warning.sh && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					max_warning.uch = (DevUChar)max_warning.sh;
					if(alarm_conf.test(min_warn) && min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG:
					if (!(str >> max_warning.db && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					max_warning.ulg = (DevULong)max_warning.db;
					if(alarm_conf.test(min_warn) && min_warning.ulg >= max_warning.ulg)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> max_warning.db && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					max_warning.ulg64 = (DevULong64)max_warning.db;
					if(alarm_conf.test(min_warn) && min_warning.ulg64 >= max_warning.ulg64)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> max_warning.sh && str.eof()))
						throw_err_format("max_warning",dev_name,"Attribute::init_opt_prop()");
					max_warning.uch = (DevUChar)max_warning.sh;
					if(alarm_conf.test(min_warn) && min_warning.uch >= max_warning.uch)
						throw_incoherent_val_err("min_warning","max_warning",dev_name,"Attribute::init_opt_prop()");
					break;
				}
				alarm_conf.set(max_warn);
			}
			else
				throw_err_data_type("max_warning",dev_name,"Attribute::init_opt_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("max_warning",e);
	}

//
// Get delta_t property
//

	bool delta_t_defined = false;
	try
	{
		delta_t_str = get_attr_value(prop_list,"delta_t");
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
				throw_err_data_type("delta_t",dev_name,"Attribute::init_opt_prop()");
		}
		else
			delta_t =  0;
	}
	catch(DevFailed &e)
	{
		add_startup_exception("delta_t",e);
	}

//
// Get delta_val property
//

	bool delta_val_defined = false;
	try
	{
		delta_val_str = get_attr_value(prop_list,"delta_val");
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
				switch (data_type)
				{
				case Tango::DEV_SHORT:
					if (!(str >> delta_val.sh && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_LONG:
					if (!(str >> delta_val.db && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					delta_val.lg = (DevLong)delta_val.db;
					break;

				case Tango::DEV_LONG64:
					if (!(str >> delta_val.db && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					delta_val.lg64 = (DevLong64)delta_val.db;
					break;

				case Tango::DEV_DOUBLE:
					if (!(str >> delta_val.db && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_FLOAT:
					if (!(str >> delta_val.fl && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_USHORT:
					if (!(str >> delta_val.ush && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					break;

				case Tango::DEV_UCHAR:
					if (!(str >> delta_val.sh && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					delta_val.uch = (DevUChar)delta_val.sh;
					break;

				case Tango::DEV_ULONG:
					if (!(str >> delta_val.db && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					delta_val.ulg = (DevULong)delta_val.db;
					break;

				case Tango::DEV_ULONG64:
					if (!(str >> delta_val.db && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					delta_val.ulg64 = (DevULong64)delta_val.db;
					break;

				case Tango::DEV_ENCODED:
					if (!(str >> delta_val.sh && str.eof()))
						throw_err_format("delta_val",dev_name,"Attribute::init_opt_prop()");
					delta_val.uch = (DevUChar)delta_val.sh;
					break;
				}
				if(delta_t_defined)
					alarm_conf.set(rds); // set RDS flag only if both delta_t and delta_val are set
				delta_val_defined = true;
			}
			else
				throw_err_data_type("delta_val",dev_name,"Attribute::init_opt_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("delta_val",e);
	}

//
// Throw exception if only one RDS property is defined
//

	try
	{
		if (((delta_t_defined == true) && (delta_val_defined == false)) ||
			((delta_t_defined == false) && (delta_val_defined == true)))
		{
			TangoSys_OMemStream o;

			o << "RDS alarm properties (delta_t and delta_val) are not correctly defined for attribute " << name;
			o << " in device " << dev_name << ends;
			Except::throw_exception((const char *)API_AttrOptProp,
						  o.str(),
						  (const char *)"Attribute::init_opt_prop()");
		}
	}
	catch(DevFailed &e)
	{
		add_startup_exception("rds_alarm",e);
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::add_startup_exception
//
// description : 	Stores an exception raised during the device startup
//			sequence in a map
//
// in :			prop_name : The property name for which the exception was
//			raised
//			except : The raised exceptoin
//
//--------------------------------------------------------------------------

void Attribute::add_startup_exception(string prop_name,const DevFailed &except)
{
	startup_exceptions.insert(pair<string,const DevFailed>(prop_name,except));
	check_startup_exceptions = true;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::delete_startup_exception
//
// description : 	Deletes the exception related to the property name from
//			startup_exceptoins map
//
// in :			prop_name : The property name as a key for which the
//			exception is to be deleted from startup_exceptions map
//
//--------------------------------------------------------------------------

void Attribute::delete_startup_exception(string prop_name)
{
	if(check_startup_exceptions == true)
	{
		map<string,const DevFailed>::iterator it = startup_exceptions.find(prop_name);
		if(it != startup_exceptions.end())
			startup_exceptions.erase(it);
		if(startup_exceptions.empty() == true)
			check_startup_exceptions = false;

        DeviceImpl *dev = get_att_device();
        dev->set_run_att_conf_loop(true);
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
	Except::throw_exception((const char *)API_AttrOptProp,
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
	Except::throw_exception((const char *)API_IncoherentValues,
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
	Except::throw_exception((const char *)API_AttrOptProp,
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
	Except::throw_exception((const char *)API_AttrOptProp,
			      o.str(),
			      (const char *)"Attribute::throw_min_max_value()");
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
    if ( dev == NULL )
    {
        dev = tg->get_device_by_name(d_name);
    }

	string &att_name = get_name_lower();

	vector<string> &attr_list = dev->get_polled_attr();

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

		vector<string> &napa = dev->get_non_auto_polled_attr();
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
    if ((the_dev != NULL) && (dev == NULL))
    {
        dev = the_dev;
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
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::get_attr_value()");
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
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::get_attr_value()");
	}

	pos->convert();
	return pos->get_lg_value();
}

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
		Except::throw_exception((const char *)API_AttrNoAlarm,o.str(),
				      (const char *)"Attribute::check_alarm()");
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
					tmp_val = tmp_lo64[0];

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
					tmp_val = tmp_ulo64[0];

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
					tmp_val = tmp_ulo[0];

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
					tmp_val = tmp_lo64[0];

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
					tmp_val = tmp_ulo[0];

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
					tmp_val = tmp_ulo64[0];

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
				Tango::DevShort tmp_val;
				tmp_val = date == false ? (*value.sh_seq)[0] : tmp_sh[0];

				if (tmp_val <= min_warning.sh)
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
				Tango::DevLong tmp_val;
				tmp_val = date == false ? (*value.lg_seq)[0] : tmp_lo[0];

				if (tmp_val <= min_warning.lg)
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
				Tango::DevLong64 tmp_val;
				tmp_val = date == false ? (*value.lg64_seq)[0] : tmp_lo64[0];

				if (tmp_val <= min_warning.lg64)
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
				Tango::DevDouble tmp_val;
				tmp_val = date == false ? (*value.db_seq)[0] : tmp_db[0];

				if (tmp_val <= min_warning.db)
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
				Tango::DevFloat tmp_val;
				tmp_val = date == false ? (*value.fl_seq)[0] : tmp_fl[0];

				if (tmp_val <= min_warning.fl)
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
				Tango::DevUShort tmp_val;
				tmp_val = date == false ? (*value.ush_seq)[0] : tmp_ush[0];

				if (tmp_val <= min_warning.ush)
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
				Tango::DevUChar tmp_val;
				tmp_val = date == false ? (*value.cha_seq)[0] : tmp_cha[0];

				if (tmp_val <= min_warning.uch)
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
				Tango::DevULong tmp_val;
				tmp_val = date == false ? (*value.ulg_seq)[0] : tmp_ulo[0];

				if (tmp_val <= min_warning.ulg)
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
				Tango::DevULong64 tmp_val;
				tmp_val = date == false ? (*value.ulg64_seq)[0] : tmp_ulo64[0];

				if (tmp_val <= min_warning.ulg64)
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
                    Tango::DevUChar tmp_val;
                    tmp_val = date == false ? (*value.enc_seq)[0].encoded_data[i] : tmp_enc[0].encoded_data[i];

					if (tmp_val <= min_warning.uch)
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
				Tango::DevShort tmp_val;
				tmp_val = date == false ? (*value.sh_seq)[0] : tmp_sh[0];

				if (tmp_val >= max_warning.sh)
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
				Tango::DevLong tmp_val;
				tmp_val = date == false ? (*value.lg_seq)[0] : tmp_lo[0];

				if (tmp_val >= max_warning.lg)
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
				Tango::DevLong64 tmp_val;
				tmp_val = date == false ? (*value.lg64_seq)[0] : tmp_lo64[0];

				if (tmp_val >= max_warning.lg64)
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
				Tango::DevDouble tmp_val;
				tmp_val = date == false ? (*value.db_seq)[0] : tmp_db[0];

				if (tmp_val >= max_warning.db)
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
				Tango::DevFloat tmp_val;
				tmp_val = date == false ? (*value.fl_seq)[0] : tmp_fl[0];

				if (tmp_val >= max_warning.fl)
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
				Tango::DevUShort tmp_val;
				tmp_val = date == false ? (*value.ush_seq)[0] : tmp_ush[0];

				if (tmp_val >= max_warning.ush)
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
				Tango::DevUChar tmp_val;
				tmp_val = date == false ? (*value.cha_seq)[0] : tmp_cha[0];

				if (tmp_val >= max_warning.uch)
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
				Tango::DevULong tmp_val;
				tmp_val = date == false ? (*value.ulg_seq)[0] : tmp_ulo[0];

				if (tmp_val >= max_warning.ulg)
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
				Tango::DevULong64 tmp_val;
				tmp_val = date == false ? (*value.ulg64_seq)[0] : tmp_ulo64[0];

				if (tmp_val >= max_warning.ulg64)
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
                    Tango::DevUChar tmp_val;
                    tmp_val = date == false ? (*value.enc_seq)[0].encoded_data[i] : tmp_enc[0].encoded_data[i];

					if (tmp_val >= max_warning.uch)
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
		tmp_lo64[1] = (*val_ptr)[0];
		value.lg64_seq = new Tango::DevVarLong64Array(data_size + 1,data_size + 1,tmp_lo64,false);
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
		if (scalar_str_attr_release == true)
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
		tmp_ulo[1] = (*val_ptr)[0];
		value.ulg_seq = new Tango::DevVarULongArray(data_size + 1,data_size + 1,tmp_ulo,false);
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
		tmp_ulo64[1] = (*val_ptr)[0];
		value.ulg64_seq = new Tango::DevVarULong64Array(data_size + 1,data_size + 1,tmp_ulo64,false);
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
		tmp_state[1] = (*val_ptr)[0];
		value.state_seq = new Tango::DevVarStateArray(data_size + 1,data_size + 1,tmp_state,false);
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


void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_3 *ptr,Tango::DeviceImpl *d)
{
	if ((name_lower == "state") || (name_lower == "status"))
	{
		ptr->quality = Tango::ATTR_VALID;
		CORBA::Any &a = ptr->value;

		if (name_lower == "state")
		{
			a <<= d->get_state();
		}
		else
		{
			Tango::DevVarStringArray str_seq(1);
			str_seq.length(1);
			str_seq[0] = CORBA::string_dup(d->get_status().c_str());

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
			MultiAttribute *m_attr = d->get_device_attr();

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


void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_4 *ptr_4,Tango::DeviceImpl *d)
{
	if ((name_lower == "state") || (name_lower == "status"))
	{
		ptr_4->quality = Tango::ATTR_VALID;

		if (name_lower == "state")
		{
			ptr_4->value.dev_state_att(d->get_state());
		}
		else
		{
			Tango::DevVarStringArray str_seq(1);
			str_seq.length(1);
			str_seq[0] = CORBA::string_dup(d->get_status().c_str());

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
			MultiAttribute *m_attr = d->get_device_attr();

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
			d->data_into_net_object(*this,(Tango::AttributeValueList_3 *)NULL,&dummy_list,0,writable,false);

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
		change_subscription = (int)now - event_change_subscription;

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

		if (dev == NULL)
			dev = tg->get_device_by_name(d_name);

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

						Except::throw_exception((const char *)API_AttrValueNotSet,o.str(),
				        		(const char *)"Attribute::fire_change_event()");
					}
				}
			}
		}

//
// Build one AttributeValue_3 or AttributeValue_4 object
//

		try
		{
			if (dev->get_dev_idl_version() >= 4)
			{
				if (event_change_client_3 == true)
					send_attr = new Tango::AttributeValue_3;
				else
					send_attr_4 = new Tango::AttributeValue_4;
			}
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)API_MemoryAllocation,
				      	  (const char *)"Can't allocate memory in server",
				      	  (const char *)"Attribute::fire_change_event()");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr != NULL)
				Attribute_2_AttributeValue(send_attr,dev);
			else
				Attribute_2_AttributeValue(send_attr_4,dev);
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
                send_event = event_supplier_nd->detect_and_push_change_event(dev,ad,*this,name,except,true);
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

                        event_supplier_zmq->push_event(dev,"change",f_names,f_data,f_names_lg,f_data_lg,ad,name,except);
                    }
                }
                else
                    event_supplier_zmq->detect_and_push_change_event(dev,ad,*this,name,except,true);
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
				((except == NULL) && (prev_change_event.err == true)) ||
				((quality != Tango::ATTR_INVALID) &&
				(prev_change_event.quality == Tango::ATTR_INVALID)))
			{
				force_change = true;
			}

			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
			{
				prev_change_event.err    = true;
				prev_change_event.except = *except;
			}
			else
			{
				Tango::AttrQuality the_quality;

				if (send_attr_4 != NULL)
				{
					the_quality = send_attr_4->quality;
					prev_change_event.value_4 = send_attr_4->value;
				}
				else
				{
					the_quality = send_attr->quality;
					prev_change_event.value = send_attr->value;
				}

				if (prev_change_event.quality !=  the_quality)
				{
					quality_change = true;
				}

				prev_change_event.quality = the_quality;
				prev_change_event.err = false;
			}
			prev_change_event.inited = true;

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
                event_supplier_nd->push_event(dev,
                                        "change",
                                        filterable_names,
                                        filterable_data,
                                        filterable_names_lg,
                                        filterable_data_lg,
                                        ad,
                                        name,
                                        except);
            if (event_supplier_zmq != NULL)
                event_supplier_zmq->push_event(dev,
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
		archive_subscription = (int)now - event_archive_subscription;

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

		if (dev == NULL)
			dev = tg->get_device_by_name(d_name);

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

						Except::throw_exception((const char *)API_AttrValueNotSet,o.str(),
				        		(const char *)"Attribute::fire_archive_event()");
					}
				}
			}
		}

//
// Build one AttributeValue_3 or AttributeValue_4 object
//

		try
		{
			if (dev->get_dev_idl_version() >= 4)
			{
				if (event_archive_client_3 == true)
					send_attr = new Tango::AttributeValue_3;
				else
					send_attr_4 = new Tango::AttributeValue_4;
			}
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)API_MemoryAllocation,
				      	  (const char *)"Can't allocate memory in server",
				      	  (const char *)"Attribute::fire_archive_event()");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr != NULL)
				Attribute_2_AttributeValue(send_attr,dev);
			else
				Attribute_2_AttributeValue(send_attr_4,dev);
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
                send_event = event_supplier_nd->detect_and_push_archive_event(dev,ad,*this,name,except,&now_timeval,true);
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

                        event_supplier_zmq->push_event(dev,"archive",f_names,f_data,f_names_lg,f_data_lg,ad,name,except);
                    }
                }
                else
                    event_supplier_zmq->detect_and_push_archive_event(dev,ad,*this,name,except,&now_timeval,true);
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
							dev);
            else if (event_supplier_zmq != NULL)
                event_supplier_zmq->detect_change(*this, ad,true,
							delta_change_rel,
							delta_change_abs,
							except,
							force_change,
							dev);


			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
			{
				prev_archive_event.err    = true;
				prev_archive_event.except = *except;
			}
			else
			{
				Tango::AttrQuality the_quality;

				if (send_attr_4 != NULL)
				{
					prev_archive_event.value_4 = send_attr_4->value;
					the_quality = send_attr_4->quality;
				}
				else
				{
					prev_archive_event.value = send_attr->value;
					the_quality = send_attr->quality;
				}

				if (prev_archive_event.quality !=  the_quality)
				{
					quality_change = true;
				}

				prev_archive_event.quality = the_quality;
				prev_archive_event.err = false;
			}
			prev_archive_event.inited = true;

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
                event_supplier_nd->push_event(dev,
							"archive",
							filterable_names,
							filterable_data,
							filterable_names_lg,
							filterable_data_lg,
							ad,
							name,
							except);
            if (event_supplier_zmq != NULL)
                event_supplier_zmq->push_event(dev,
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

		if (dev == NULL)
			dev = tg->get_device_by_name(d_name);

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

						Except::throw_exception((const char *)API_AttrValueNotSet,o.str(),
				        		(const char *)"Attribute::fire_event()");
					}
				}
			}
		}

//
// Build one AttributeValue_3 or AttributeValue_4 object
//

		try
		{
			if (dev->get_dev_idl_version() >= 4)
			{
				if (event_user_client_3 == true)
					send_attr = new Tango::AttributeValue_3;
				else
					send_attr_4 = new Tango::AttributeValue_4;
			}
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)API_MemoryAllocation,
				      	  (const char *)"Can't allocate memory in server",
				      	  (const char *)"Attribute::fire_event()");
		}

//
// don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr != NULL)
				Attribute_2_AttributeValue(send_attr,dev);
			else
				Attribute_2_AttributeValue(send_attr_4,dev);
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
            event_supplier_nd->push_event(dev,
					   "user_event",
					   filt_names,
					   filt_vals,
					   filterable_names_lg,
					   filterable_data_lg,
					   ad,
					   name,
					   except);
        if (event_supplier_zmq != NULL)
            event_supplier_zmq->push_event(dev,
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
// method : 		Attribute::upd_att_prop_db
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
			tg->get_database()->put_device_attribute_property(d_name,db_data);
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
			tg->get_database()->get_device_attribute_property(d_name,db_read_data);
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
				tg->get_database()->delete_device_attribute_property(d_name,db_delete_data);
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
	if (dev == NULL)
	{
		Tango::Util *tg = Tango::Util::instance();
		dev = tg->get_device_by_name(d_name);
	}

	return dev;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_attr_serial_method
//
// description : 	Set attribute serialization method
//
//--------------------------------------------------------------------------

void Attribute::set_attr_serial_model(AttrSerialModel ser_model)
{
	if (ser_model == Tango::ATTR_BY_USER)
	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->get_serial_model() != Tango::BY_DEVICE)
		{
			Except::throw_exception((const char *)API_AttrNotAllowed,
				      	  (const char *)"Attribute serial model by user is not allowed when the process is not in BY_DEVICE serialization model",
				      	  (const char *)"Attribute::set_attr_serial_model");
		}
	}

	attr_serial_model=ser_model;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_att_device_class
//
// description : 	Return a pointer to the attribute device class
//
//--------------------------------------------------------------------------

DeviceClass *Attribute::get_att_device_class(string &dev_name)
{

//
// Get device class
// When the server is started, it's an easy task
// When the server is in its starting phase, it's more tricky
// Get from the DeviceClass list the first one for which the device
// factory method has not yet been fully executed.
// This is the DeviceClass with the device in its init_device() method
// has called Attribute::set_properties()
//

    Tango::Util *tg = Tango::Util::instance();
    Tango::DeviceClass *dev_class = NULL;

    if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
    {
        Tango::DeviceImpl *dev = get_att_device();
        dev_class = dev->get_device_class();
    }
    else
    {
        const vector<DeviceClass *> &tmp_cl_list = *tg->get_class_list();
        size_t loop;
        for (loop = 0;loop < tmp_cl_list.size();++loop)
        {
            if (tmp_cl_list[loop]->get_device_factory_done() == false)
                break;
        }

        if (loop != tmp_cl_list.size())
        {
            dev_class = tmp_cl_list[loop];
        }
        else
        {
				TangoSys_OMemStream o;

				o << "Device " << dev_name << "-> Attribute : " << name;
				o << "\nCan't retrieve device class!" << ends;
				Except::throw_exception((const char *)API_CantRetrieveClass,
							  o.str(),
							  (const char *)"Attribute::set_properties()");
        }
    }

    return dev_class;
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

    if (dev == NULL)
    {
        Tango::Util *tg = Tango::Util::instance();
        dev = tg->get_device_by_name(d_name);
    }

//
// Log something if the new quality is different than the old one
//

    if (quality != old_quality)
    {
        if (alarm.any() == false)
        {

//
// No alarm detected
//

            switch(quality)
            {
                case ATTR_INVALID:
                if (dev->get_logger()->is_error_enabled())
                    dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "INVALID quality for attribute " << name << endl;
                break;

                case ATTR_CHANGING:
                if (dev->get_logger()->is_info_enabled())
                    dev->get_logger()->info_stream() << log4tango::LogInitiator::_begin_log << "CHANGING quality for attribute " << name << endl;
                break;

                case ATTR_VALID:
                if (dev->get_logger()->is_info_enabled())
                    dev->get_logger()->info_stream() << log4tango::LogInitiator::_begin_log << "VALID quality for attribute " << name << endl;
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
                if (dev->get_logger()->is_error_enabled())
                    dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MIN ALARM for attribute " << name << endl;
            }
            else if (alarm[max_level] == true)
            {
                if (dev->get_logger()->is_error_enabled())
                    dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MAX ALARM for attribute " << name << endl;
            }
            else if (alarm[rds] == true)
            {
                if (dev->get_logger()->is_warn_enabled())
                    dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) ALARM for attribute " << name << endl;
            }
            else if (alarm[min_warn] == true)
            {
               if (dev->get_logger()->is_warn_enabled())
                    dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MIN WARNING for attribute " << name << endl;
            }
            else if (alarm[max_warn] == true)
            {
               if (dev->get_logger()->is_warn_enabled())
                    dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MAX WARNING for attribute " << name << endl;
            }
        }
    }
    else
    {

//
// The quality is the same but may be the alarm has changed
//

        if (alarm != old_alarm)
        {
            if (alarm[min_level] == true)
            {
                if (dev->get_logger()->is_error_enabled())
                    dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MIN ALARM for attribute " << name << endl;
            }
            else if (alarm[max_level] == true)
            {
                if (dev->get_logger()->is_error_enabled())
                    dev->get_logger()->error_stream() << log4tango::LogInitiator::_begin_log << "MAX ALARM for attribute " << name << endl;
            }
            else if (alarm[rds] == true)
            {
                if (dev->get_logger()->is_warn_enabled())
                    dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "RDS (Read Different Set) ALARM for attribute " << name << endl;
            }
            else if (alarm[min_warn] == true)
            {
               if (dev->get_logger()->is_warn_enabled())
                    dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MIN WARNING for attribute " << name << endl;
            }
            else if (alarm[max_warn] == true)
            {
               if (dev->get_logger()->is_warn_enabled())
                    dev->get_logger()->warn_stream() << log4tango::LogInitiator::_begin_log << "MAX WARNING for attribute " << name << endl;
            }
        }
    }
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::avns_in_db()
//
// description : 	Store in db the famous AVNS (AlrmValueNotSpec)
//                  for a specific attribute property
//
// Arg in :			prop_name : Property name
//
//--------------------------------------------------------------------------

void Attribute::avns_in_db(const char *prop_name,string &dev_name)
{
    Tango::Util *tg = Tango::Util::instance();

    if (Tango::Util::_UseDb == true)
    {
        DbDatum attr_dd(name), prop_dd(prop_name);
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
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::avns_in_att()
//
// description : 	Store in att the famous AVNS (AlrmValueNotSpec)
//                  for a specific attribute property
//
// Arg in :			pt : Property type
//
//---------------------------------------------------------------------------------------------------------------------

void Attribute::avns_in_att(prop_type pt)
{
    Tango::Util *tg = Tango::Util::instance();
    Tango::TangoMonitor *mon_ptr = NULL;
    if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
        mon_ptr = &(get_att_device()->get_att_conf_monitor());

    {
        AutoTangoMonitor sync1(mon_ptr);

        switch (pt)
        {
        case MIN_VALUE:
            check_min_value = false;
            min_value_str = AlrmValueNotSpec;
            break;

        case MAX_VALUE:
            check_max_value = false;
            max_value_str = AlrmValueNotSpec;
            break;

        case MIN_WARNING:
            min_warning_str = AlrmValueNotSpec;
            break;

        case MAX_WARNING:
            max_warning_str = AlrmValueNotSpec;
            break;

        case MIN_ALARM:
            min_alarm_str = AlrmValueNotSpec;
            break;

        case MAX_ALARM:
            max_alarm_str = AlrmValueNotSpec;
            break;

        default:
            break;
        }

        if (tg->is_svr_starting() == false && tg->is_device_restarting(d_name) == false)
            get_att_device()->push_att_conf_event(this);
    }
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_format_notspec()
//
// description :
//		Set the attribute format property to the default value which depends on attribute data type
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::set_format_notspec()
{
	switch (data_type)
	{
	case DEV_SHORT:
	case DEV_LONG:
	case DEV_LONG64:
	case DEV_UCHAR:
	case DEV_USHORT:
	case DEV_ULONG:
	case DEV_ULONG64:
		format = FormatNotSpec_INT;
		break;

	case DEV_STRING:
		format = FormatNotSpec_STR;
		break;

	case DEV_STATE:
	case DEV_ENCODED:
	case DEV_BOOLEAN:
		format = AlrmValueNotSpec;
		break;

	case DEV_FLOAT:
	case DEV_DOUBLE:
		format = FormatNotSpec_FL;
		break;

	default:
		break;
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::is_format_notspec()
//
// description :
//		Set the attribute format property to the default value which depends on attribute data type
//
// argument :
//		in :
//			- format : The format property string
//
// return :
//		This method retruns true if the format string is the default value
//
//--------------------------------------------------------------------------------------------------------------------

bool Attribute::is_format_notspec(const char *format)
{
	bool ret = false;

	switch (data_type)
	{
	case DEV_SHORT:
	case DEV_LONG:
	case DEV_LONG64:
	case DEV_UCHAR:
	case DEV_USHORT:
	case DEV_ULONG:
	case DEV_ULONG64:
		if (TG_strcasecmp(format,FormatNotSpec_INT) == 0)
			ret = true;
		break;

	case DEV_STRING:
		if (TG_strcasecmp(format,FormatNotSpec_STR) == 0)
			ret = true;
		break;

	case DEV_STATE:
	case DEV_ENCODED:
	case DEV_BOOLEAN:
		if (TG_strcasecmp(format,AlrmValueNotSpec) == 0)
			ret = true;
		break;

	case DEV_FLOAT:
	case DEV_DOUBLE:
		if (TG_strcasecmp(format,FormatNotSpec_FL) == 0)
			ret = true;
		break;

	default:
		break;
	}

	return ret;
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::def_format_in_dbdatum()
//
// description :
//		Insert the default format string in a DbDatum instance. This default value depends on the attribute
//		data type
//
// argument :
//		in :
//			- db : Reference to the DbDatum object
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::def_format_in_dbdatum(DbDatum &db)
{
	switch (data_type)
	{
	case DEV_SHORT:
	case DEV_LONG:
	case DEV_LONG64:
	case DEV_UCHAR:
	case DEV_USHORT:
	case DEV_ULONG:
	case DEV_ULONG64:
		db << FormatNotSpec_INT;
		break;

	case DEV_STRING:
		db << FormatNotSpec_STR;
		break;

	case DEV_STATE:
	case DEV_ENCODED:
	case DEV_BOOLEAN:
		db << AlrmValueNotSpec;
		break;

	case DEV_FLOAT:
	case DEV_DOUBLE:
		db << FormatNotSpec_FL;
		break;

	default:
		break;
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::xxx_event_subscribed()
//
// description :
//		Returns true if there are some subscriber listening for event. This is a method family. There is one method
//		for each event type
//		Cannot replce this methods family by one method with event type as parameter for compatibility reason
//
//--------------------------------------------------------------------------------------------------------------------

bool Attribute::change_event_subscribed()
{
	bool ret = false;

	if (event_change_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_change_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

bool Attribute::periodic_event_subscribed()
{
	bool ret = false;

	if (event_periodic_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_periodic_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

bool Attribute::archive_event_subscribed()
{
	bool ret = false;

	if (event_archive_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_archive_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

bool Attribute::quality_event_subscribed()
{
	bool ret = false;

	if (event_quality_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_quality_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

bool Attribute::user_event_subscribed()
{
	bool ret = false;

	if (event_user_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_user_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

bool Attribute::attr_conf_event_subscribed()
{
	bool ret = false;

	if (event_attr_conf_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_attr_conf_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

bool Attribute::data_ready_event_subscribed()
{
	bool ret = false;

	if (event_data_ready_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_data_ready_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing instance of the
//			Attribute class
//
//-------------------------------------------------------------------------------------------------------------------

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

	case Tango::DATA_TYPE_UNKNOWN :
		cout << "Unknown" << endl;
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

	case Tango::FMT_UNKNOWN :
		cout << "Unknown" << endl;
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
