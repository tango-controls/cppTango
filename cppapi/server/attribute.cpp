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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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

namespace Tango
{

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
 enum_nb(0),loc_enum_ptr(Tango_nullptr),poll_period(0),event_period(0),archive_period(0),last_periodic(0.0),
 archive_last_periodic(0.0),periodic_counter(0),archive_periodic_counter(0),
 archive_last_event(0.0),dev(NULL),change_event_implmented(false),
 archive_event_implmented(false),check_change_event_criteria(true),
 check_archive_event_criteria(true),dr_event_implmented(false),
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

//
// Enum init (in case of)
//

	if (data_type == DEV_ENUM)
		init_enum_prop(prop_list);
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
		delete [] loc_enum_ptr;
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
    prev_change_event.err=false;
    prev_change_event.quality=Tango::ATTR_VALID;

	prev_archive_event.inited = false;
    prev_archive_event.err=false;
    prev_archive_event.quality=Tango::ATTR_VALID;

	prev_quality_event.inited = false;
//
// do not start sending events automatically, wait for the first client to subscribe. Sending events automatically
// will put an unnecessary load on the server because all attributes will be polled
//

	event_change3_subscription = 0;
	event_change4_subscription = 0;
	event_change5_subscription = 0;

	event_archive3_subscription = 0;
	event_archive4_subscription = 0;
	event_archive5_subscription = 0;

	event_periodic3_subscription = 0;
	event_periodic4_subscription = 0;
	event_periodic5_subscription = 0;

	event_user3_subscription = 0;
	event_user4_subscription = 0;
	event_user5_subscription = 0;

	event_attr_conf_subscription = 0;
	event_attr_conf5_subscription = 0;

	event_quality_subscription = 0;
	event_data_ready_subscription = 0;

//
// Init client lib parameters
//

	for (int i = 0;i < numEventType;i++)
		client_lib[i].clear();
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_check_val_prop
//
// description :
//
//
// argument :
// 		in :
//			- prop_list : The property vector
//			- dev_name : The device name (usefull for error)
//          - prop_name : Property name
//          - prop_comp : The property object used for checking if min > max
//      out :
//          - prop_str : property value as a string
//          - prop : The property object
//
//-------------------------------------------------------------------------------------------------------------------

bool Attribute::init_check_val_prop (vector<AttrProperty> &prop_list,string &dev_name,const char* prop_name,
                                    string& prop_str,Tango::Attr_CheckVal &prop,Tango::Attr_CheckVal &prop_comp)
{
    prop_str = get_attr_value(prop_list, prop_name);

    if (prop_str == AlrmValueNotSpec)
        return false;

    TangoSys_MemStream str;
    bool is_err_format = false;
    bool is_incoherent_val_err = false;

    bool min_prop = true;
    if (prop_name[1] == 'a')
        min_prop = false;

    str << prop_str;

    switch (data_type)
    {
        case Tango::DEV_SHORT:
            is_err_format = !(str >> prop.sh && str.eof());
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.sh >= prop_comp.sh)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.sh >= prop.sh)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_LONG:
            is_err_format = !(str >> prop.db && str.eof());
            prop.lg = (DevLong)prop.db;
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.lg >= prop_comp.lg)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.lg >= prop.lg)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_LONG64:
            is_err_format = !(str >> prop.db && str.eof());
            prop.lg64 = (DevLong64)prop.db;
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.lg64 >= prop_comp.lg64)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.lg64 >= prop.lg64)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_DOUBLE:
            is_err_format = !(str >> prop.db && str.eof());
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.db >= prop_comp.db)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.db >= prop.db)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_FLOAT:
            is_err_format = !(str >> prop.fl && str.eof());
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.fl >= prop_comp.fl)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.fl >= prop.fl)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_USHORT:
            is_err_format = !(str >> prop.ush && str.eof());
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.ush >= prop_comp.ush)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.ush >= prop.ush)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_UCHAR:
            is_err_format = !(str >> prop.sh && str.eof());
            prop.uch = (DevUChar)prop.sh;
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.uch >= prop_comp.uch)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.uch >= prop.uch)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_ULONG:
            is_err_format = !(str >> prop.db && str.eof());
            prop.ulg = (DevULong)prop.db;
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.ulg >= prop_comp.ulg)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.ulg >= prop.ulg)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_ULONG64:
            is_err_format = !(str >> prop.db && str.eof());
            prop.ulg64 = (DevULong64)prop.db;
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.ulg64 >= prop_comp.ulg64)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.ulg64 >= prop.ulg64)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        case Tango::DEV_ENCODED:
            is_err_format = !(str >> prop.sh && str.eof());
            prop.uch = (DevUChar)prop.sh;
            if (is_err_format == false)
            {
                if (is_value_set(prop_name) == true)
                {
                    if (min_prop == true)
                    {
                        if (prop.uch >= prop_comp.uch)
                            is_incoherent_val_err = true;
                    }
                    else
                    {
                        if (prop_comp.uch >= prop.uch)
                            is_incoherent_val_err = true;
                    }
                }
            }
            break;

        default:
            throw_err_data_type(prop_name, dev_name, "Attribute::init_opt_prop()");
    }

    if (is_err_format)
        throw_err_format(prop_name, dev_name, "Attribute::init_opt_prop()");
    else if (is_incoherent_val_err)
    {
        string opp_prop_name(prop_name);
        opp_prop_name[1] = (opp_prop_name[1] == 'a') ? ('i') : ('a'); // max_... ->  mix_... || min_... ->  man_...
        opp_prop_name[2] = (opp_prop_name[2] == 'x') ? ('n') : ('x'); // mix_... ->  min_... || man_... ->  max_...

        if (prop_name[1] == 'a')
            throw_incoherent_val_err(opp_prop_name.c_str(),prop_name,dev_name,"Attribute::init_opt_prop()");
        else
            throw_incoherent_val_err(prop_name,opp_prop_name.c_str(),dev_name,"Attribute::init_opt_prop()");
    }

    return true;
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
// Init the properties
//

    init_string_prop(prop_list, label, "label");
    init_string_prop(prop_list, description, "description");
    init_string_prop(prop_list, unit, "unit");
    init_string_prop(prop_list, standard_unit, "standard_unit");
    init_string_prop(prop_list, display_unit, "display_unit");
    init_string_prop(prop_list, format, "format");

//
// Init the min alarm property
//

	try
	{
        if (init_check_val_prop(prop_list,dev_name,"min_alarm",min_alarm_str,min_alarm,max_alarm) == true)
            alarm_conf.set(min_level);
	}
	catch (DevFailed &e)
	{
		add_startup_exception("min_alarm",e);
	}

//
// Init the max alarm property
//

	try
	{
        if (init_check_val_prop(prop_list,dev_name,"max_alarm",max_alarm_str,max_alarm,min_alarm) == true)
            alarm_conf.set(max_level);
	}
	catch (DevFailed &e)
	{
		add_startup_exception("max_alarm",e);
	}

//
// Init the min value property
//

	try
	{
        if (init_check_val_prop(prop_list,dev_name,"min_value",min_value_str,min_value,max_value) == true)
            check_min_value = true;
	}
	catch (DevFailed &e)
	{
		add_startup_exception("min_value",e);
	}

//
// Init the max value property
//

	try
	{
        if (init_check_val_prop(prop_list,dev_name,"max_value",max_value_str,max_value,min_value) == true)
            check_max_value = true;
	}
	catch (DevFailed &e)
	{
		add_startup_exception("max_value",e);
	}

//
// Init the min warning property
//

	try
	{
        if (init_check_val_prop(prop_list,dev_name,"min_warning",min_warning_str,min_warning,max_warning) == true)
            alarm_conf.set(min_warn);
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
        if (init_check_val_prop(prop_list,dev_name,"max_warning",max_warning_str,max_warning,min_warning) == true)
            alarm_conf.set(max_warn);
	}
	catch (DevFailed &e)
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
				(data_type != Tango::DEV_STATE) &&
				(data_type != Tango::DEV_ENUM))
			{
                TangoSys_MemStream str;
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

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_enum_prop
//
// description :
//		Get the enumeration labels from the property vector
//
// argument :
// 		in :
//			- prop_list : The property vector
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::init_enum_prop(vector<AttrProperty> &prop_list)
{
	try
	{
		string tmp_enum_label = get_attr_value(prop_list,"enum_labels");
		build_check_enum_labels(tmp_enum_label);
	}
	catch(DevFailed &e)
	{
		string desc(e.errors[0].desc.in());
		if (desc.find("Property enum_labels is missing") != string::npos)
		{
			stringstream ss;
			ss << "The attribute " << name << " has the DEV_ENUM data type but there is no enumeration label(s) defined";

			e.errors[0].desc = Tango::string_dup(ss.str().c_str());
		}
		add_startup_exception("enum_labels",e);
	}
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::build_check_enum_labels
//
// description :
//		Create vector of enum labels and  check that the same label is not used several times
//
// argument :
// 		in :
//			- labs : The enum labels (labels1,label2,label3,...)
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::build_check_enum_labels(string &labs)
{
	string::size_type pos = 0;
	string::size_type start = 0;
	bool exit = false;

	enum_labels.clear();

//
// Build vector with enum labels
//

	while(exit == false)
	{
		pos = labs.find(',',start);
		if (pos == string::npos)
		{
			string tmp = labs.substr(start);
			enum_labels.push_back(tmp);
			exit = true;
		}
		else
		{
			string tmp = labs.substr(start,pos - start);
			enum_labels.push_back(tmp);
			start = pos + 1;
		}
	}

//
// Check that all labels are different
//

	vector<string> v_s = enum_labels;
	sort(v_s.begin(),v_s.end());
	for (size_t loop = 1;loop < v_s.size();loop++)
	{
		if (v_s[loop - 1] == v_s[loop])
		{
			stringstream ss;
			ss << "Enumeration for attribute " << name << " has two similar labels (";
			ss << v_s[loop - 1] << ", " << v_s[loop] << ")";

			Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::build_check_enum_labels");
		}
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::add_startup_exception
//
// description :
//		Stores an exception raised during the device startup sequence in a map
//
// argument :
// 		in :
//			- prop_name : The property name for which the exception was raised
//			- except : The raised exception
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::add_startup_exception(string prop_name,const DevFailed &except)
{
	startup_exceptions.insert(pair<string,const DevFailed>(prop_name,except));
	check_startup_exceptions = true;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::delete_startup_exception
//
// description :
//		Deletes the exception related to the property name from startup_exceptions map
//
// argument :
// 		in :
//			- prop_name : The property name as a key for which the exception is to be deleted from
//                        startup_exceptions map
//          - dev_name : The device name
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::delete_startup_exception(string prop_name,string dev_name)
{
	if(check_startup_exceptions == true)
	{
		map<string,const DevFailed>::iterator it = startup_exceptions.find(prop_name);
		if(it != startup_exceptions.end())
			startup_exceptions.erase(it);
		if(startup_exceptions.empty() == true)
			check_startup_exceptions = false;

		Util *tg = Util::instance();
		bool dev_restart = false;
		if (dev_name != "None")
		{
			dev_restart = tg->is_device_restarting(dev_name);
		}
		if (tg->is_svr_starting() == false && dev_restart == false)
		{
			DeviceImpl *dev = get_att_device();
			dev->set_run_att_conf_loop(true);
		}
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::throw_err_format
//
// description :
//		Throw a Tango DevFailed exception when an error format is detected in the string which should be converted
//		to a number
//
// argument :
// 		in :
//			- prop_name : The property name
//			- dev_name : The device name
//			- origin : The origin of the exception
//
//--------------------------------------------------------------------------------------------------------------------

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
	o << "\nValue of " << min_prop << " is greater than or equal to " << max_prop << ends;
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

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::is_writ_associated
//
// description :
//		Check if the attribute has an associated writable attribute
//
// returns:
//		This method returns a boolean set to true if the atribute has an associatied writable attribute
//
//-------------------------------------------------------------------------------------------------------------------

bool Attribute::is_writ_associated()
{
	if (writable_attr_name != AssocWritNotSpec)
		return true;
	else
		return false;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::get_attr_value
//
// description :
//		Retrieve a property value as a string from the vector of properties
//
// arguments :
// 		in :
//			- prop_list : The property vector
//			- prop_name : the property name
//
//-------------------------------------------------------------------------------------------------------------------


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

		Except::throw_exception(API_AttrOptProp,o.str(),"Attribute::get_attr_value()");
	}

	return pos->get_value();
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::get_lg_attr_value
//
// description :
//		Retrieve a property value as a long from the vector of properties
//
// argument :
// 		in :
//			- prop_list : The property vector
//			- prop_name : the property name
//
//------------------------------------------------------------------------------------------------------------------


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

		Except::throw_exception(API_AttrOptProp,o.str(),"Attribute::get_attr_value()");
	}

	pos->convert(prop_name);
	return pos->get_lg_value();
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_data_size
//
// description :
//		Compute the attribute amount of data
//
//--------------------------------------------------------------------------------------------------------------------

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

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_time
//
// description :
//		Set the date if the date flag is true
//
//--------------------------------------------------------------------------------------------------------------------

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

	if (is_fwd_att() == false &&  quality != Tango::ATTR_VALID )
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
	case Tango::DEV_ENUM:
		delete value.sh_seq;
		value.sh_seq = Tango_nullptr;
		break;

	case Tango::DEV_LONG:
		delete value.lg_seq;
		value.lg_seq = Tango_nullptr;
		break;

	case Tango::DEV_LONG64:
		delete value.lg64_seq;
		value.lg64_seq = Tango_nullptr;
		break;

	case Tango::DEV_DOUBLE:
		delete value.db_seq;
		value.db_seq = Tango_nullptr;
		break;

	case Tango::DEV_STRING:
		delete value.str_seq;
		value.str_seq = Tango_nullptr;
		break;

	case Tango::DEV_FLOAT:
		delete value.fl_seq;
		value.fl_seq = Tango_nullptr;
		break;

	case Tango::DEV_USHORT:
		delete value.ush_seq;
		value.ush_seq = Tango_nullptr;
		break;

	case Tango::DEV_UCHAR:
		delete value.cha_seq;
		value.cha_seq = Tango_nullptr;
		break;

	case Tango::DEV_BOOLEAN:
		delete value.boo_seq;
		value.boo_seq = Tango_nullptr;
		break;

	case Tango::DEV_ULONG:
		delete value.ulg_seq;
		value.ulg_seq = Tango_nullptr;
		break;

	case Tango::DEV_ULONG64:
		delete value.ulg64_seq;
		value.ulg64_seq = Tango_nullptr;
		break;

	case Tango::DEV_STATE:
		delete value.state_seq;
		value.state_seq = Tango_nullptr;
		break;

	case Tango::DEV_ENCODED:
		delete value.enc_seq;
		value.enc_seq = Tango_nullptr;
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
			strvec[1] = Tango::string_dup((*val_ptr)[0]);
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
			(*value.str_seq)[nb_read + k] = Tango::string_dup((*val_ptr)[k]);
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

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::Attribute_2_AttributeValue
//
// description :
//		Build an AttributeValue_3 object from the Attribute object content
//
// arguments
// 		in :
//			- dev : The device to which the attribute belongs to
//		out :
//			- ptr : Pointer to the AttributeValue_3 object to be filled in
//
//--------------------------------------------------------------------------------------------------------------------


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
			str_seq[0] = Tango::string_dup(d->get_status().c_str());

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

		ptr->name = Tango::string_dup(name.c_str());
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
			case Tango::DEV_ENUM:
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
		ptr->name = Tango::string_dup(name.c_str());
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::Attribute_2_AttributeValue
//
// description :
//		Build an AttributeValue_4 object from the Attribute object content
//
// arguments
// 		in :
//			- d : The device to which the attribute belongs to
//		out :
//			- ptr_4 : Pointer to the AttributeValue_5 object to be filled in
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_4 *ptr_4,Tango::DeviceImpl *d)
{
	Attribute_2_AttributeValue_base(ptr_4,d);

	Tango::AttributeIdlData aid;
	Tango::AttributeValueList_4 dummy_list(1,1,ptr_4,false);
	aid.data_4 = &dummy_list;

	if ((name_lower != "state") && (name_lower != "status") && (quality != Tango::ATTR_INVALID))
        d->data_into_net_object(*this,aid,0,writable,false);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::Attribute_2_AttributeValue
//
// description :
//		Build an AttributeValue_5 object from the Attribute object content
//
// arguments
// 		in :
//			- d : The device to which the attribute belongs to
//		out :
//			- ptr_5 : Pointer to the AttributeValue_5 object to be filled in
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_5 *ptr_5,Tango::DeviceImpl *d)
{
	Attribute_2_AttributeValue_base(ptr_5,d);

	Tango::AttributeIdlData aid;
	Tango::AttributeValueList_5 dummy_list(1,1,ptr_5,false);
	aid.data_5 = &dummy_list;

	if ((name_lower != "state") && (name_lower != "status") && (quality != Tango::ATTR_INVALID))
        d->data_into_net_object(*this,aid,0,writable,false);

	ptr_5->data_type = data_type;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::AttributeValue_4_2_AttributeValue_3
//
// description :
//		Build an AttributeValue_3 object from the AttributeValue_4 object. This method is used in case an event is
//		requested by a client knowing only IDL release 3
//
// args :
// 		in :
//			- ptr_4 : Pointer to the AttributeValue_4 object
//  	out :
//			- ptr_3 : Pointer to the AttributeValue_3 object to be filled in
//
//-------------------------------------------------------------------------------------------------------------------


void Attribute::AttributeValue_4_2_AttributeValue_3(const Tango::AttributeValue_4 *ptr_4,Tango::AttributeValue_3 *ptr_3)
{

//
// First copy the data
//

	if (ptr_4->quality != Tango::ATTR_INVALID)
	{
		AttrValUnion_2_Any(ptr_4,ptr_3->value);
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

void Attribute::AttributeValue_5_2_AttributeValue_3(const Tango::AttributeValue_5 *att_5,Tango::AttributeValue_3 *att_3)
{
//
// First copy the data
//

	if (att_5->quality != Tango::ATTR_INVALID)
	{
		AttrValUnion_2_Any(att_5,att_3->value);
	}

//
// The remaining fields
//

	att_3->time = att_5->time;
	att_3->quality = att_5->quality;
	att_3->name = att_5->name;

	att_3->r_dim = att_5->r_dim;
	att_3->w_dim = att_5->w_dim;

	att_3->err_list = att_5->err_list;
}

void Attribute::AttributeValue_3_2_AttributeValue_4(const Tango::AttributeValue_3 *att_3,Tango::AttributeValue_4 *att_4)
{
// TODO: Code data from Any to Union

//
// The remaining fields
//

	att_4->time = att_3->time;
	att_4->quality = att_3->quality;
	att_4->name = att_3->name;
	att_4->data_format = data_format;

	att_4->r_dim = att_3->r_dim;
	att_4->w_dim = att_3->w_dim;

	att_4->err_list = att_3->err_list;
}

void Attribute::AttributeValue_5_2_AttributeValue_4(const Tango::AttributeValue_5 *att_5,Tango::AttributeValue_4 *att_4)
{
//
// First pass the data from one union to another WITHOUT copying them
//

	AttrValUnion_fake_copy(att_5,att_4);

//
// The remaining fields
//

	att_4->time = att_5->time;
	att_4->quality = att_5->quality;
	att_4->name = att_5->name;
	att_4->data_format = att_5->data_format;

	att_4->r_dim = att_5->r_dim;
	att_4->w_dim = att_5->w_dim;

	att_4->err_list = att_5->err_list;
}

void Attribute::AttributeValue_3_2_AttributeValue_5(const Tango::AttributeValue_3 *att_3,Tango::AttributeValue_5 *att_5)
{
// TODO: Code data from Any to Union

//
// The remaining fields
//

	att_5->time = att_3->time;
	att_5->quality = att_3->quality;
	att_5->name = att_3->name;
	att_5->data_format = data_format;
	att_5->data_type = data_type;

	att_5->r_dim = att_3->r_dim;
	att_5->w_dim = att_3->w_dim;

	att_5->err_list = att_3->err_list;
}

void Attribute::AttributeValue_4_2_AttributeValue_5(const Tango::AttributeValue_4 *att_4,Tango::AttributeValue_5 *att_5)
{
//
// First pass the data from one union to another WITHOUT copying them
//

	AttrValUnion_fake_copy(att_4,att_5);

//
// The remaining fields
//

	att_5->time = att_4->time;
	att_5->quality = att_4->quality;
	att_5->name = att_4->name;
	att_5->data_format = att_4->data_format;
	att_5->data_type = data_type;

	att_5->r_dim = att_4->r_dim;
	att_5->w_dim = att_4->w_dim;

	att_5->err_list = att_4->err_list;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::AttributeConfig_5_2_AttributeConfig_3
//
// description :
//		Build an AttributeConfig_3 object from the AttributeConfig_5 object. This method is used in case an event is
//		requested by a client knowing only IDL release 4
//
// argument:
// 		in :
//			- conf5 : Reference to the AttributeConfig_5 object
//		out :
//			- conf3 : Reference to the AttributeConfig_3 object to be filled in
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::AttributeConfig_5_2_AttributeConfig_3(const Tango::AttributeConfig_5 &conf5,Tango::AttributeConfig_3 &conf3)
{
	size_t j;

	conf3.name = conf5.name;
	conf3.writable = conf5.writable;
	conf3.data_format = conf5.data_format;
	conf3.data_type = conf5.data_type;
	conf3.max_dim_x = conf5.max_dim_x;
	conf3.max_dim_y = conf5.max_dim_y;
	conf3.description = conf5.description;
	conf3.label = conf5.label;
	conf3.unit = conf5.unit;
	conf3.standard_unit = conf5.standard_unit;
	conf3.display_unit = conf5.display_unit;
	conf3.format = conf5.format;
	conf3.min_value = conf5.min_value;
	conf3.max_value = conf5.max_value;
	conf3.writable_attr_name = conf5.writable_attr_name;
	conf3.level = conf5.level;
	conf3.extensions.length(conf5.extensions.length());
	for (j=0; j<conf5.extensions.length(); j++)
	{
		conf3.extensions[j] = string_dup(conf5.extensions[j]);
	}
	for (j=0; j<conf5.sys_extensions.length(); j++)
	{
		conf3.sys_extensions[j] = string_dup(conf5.sys_extensions[j]);
	}

	conf3.att_alarm.min_alarm = conf5.att_alarm.min_alarm;
	conf3.att_alarm.max_alarm = conf5.att_alarm.max_alarm;
	conf3.att_alarm.min_warning = conf5.att_alarm.min_warning;
	conf3.att_alarm.max_warning = conf5.att_alarm.max_warning;
	conf3.att_alarm.delta_t = conf5.att_alarm.delta_t;
	conf3.att_alarm.delta_val = conf5.att_alarm.delta_val;
	for (j=0; j<conf5.att_alarm.extensions.length(); j++)
	{
		conf3.att_alarm.extensions[j] = string_dup(conf5.att_alarm.extensions[j]);
	}

	conf3.event_prop.ch_event.rel_change = conf5.event_prop.ch_event.rel_change;
	conf3.event_prop.ch_event.abs_change = conf5.event_prop.ch_event.abs_change;
	for (j=0; j<conf5.event_prop.ch_event.extensions.length(); j++)
	{
		conf3.event_prop.ch_event.extensions[j] = string_dup(conf5.event_prop.ch_event.extensions[j]);
	}

	conf3.event_prop.per_event.period = conf5.event_prop.per_event.period;
	for (j=0; j<conf5.event_prop.per_event.extensions.length(); j++)
	{
		conf3.event_prop.per_event.extensions[j] = string_dup(conf5.event_prop.per_event.extensions[j]);
	}

	conf3.event_prop.arch_event.rel_change = conf5.event_prop.arch_event.rel_change;
	conf3.event_prop.arch_event.abs_change = conf5.event_prop.arch_event.abs_change;
	conf3.event_prop.arch_event.period = conf5.event_prop.arch_event.period;
	for (j=0; j<conf5.event_prop.ch_event.extensions.length(); j++)
	{
		conf3.event_prop.arch_event.extensions[j] = string_dup(conf5.event_prop.arch_event.extensions[j]);
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::AttributeConfig_3_2_AttributeConfig_5
//
// description :
//		Build an AttributeConfig_3 object from the AttributeConfig_5 object. This method is used in case an event is
//		requested by a client knowing only IDL release 4
//
// argument:
// 		in :
//			- conf3 : Reference to the AttributeConfig_3 object
//		out :
//			- conf5 : Reference to the AttributeConfig_5 object to be filled in
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::AttributeConfig_3_2_AttributeConfig_5(const Tango::AttributeConfig_3 &conf3,Tango::AttributeConfig_5 &conf5)
{
	size_t j;

	conf5.name = conf3.name;
	conf5.writable = conf3.writable;
	conf5.data_format = conf3.data_format;
	conf5.data_type = conf3.data_type;
	conf5.max_dim_x = conf3.max_dim_x;
	conf5.max_dim_y = conf3.max_dim_y;
	conf5.description = conf3.description;
	conf5.label = conf3.label;
	conf5.unit = conf3.unit;
	conf5.standard_unit = conf3.standard_unit;
	conf5.display_unit = conf3.display_unit;
	conf5.format = conf3.format;
	conf5.min_value = conf3.min_value;
	conf5.max_value = conf3.max_value;
	conf5.writable_attr_name = conf3.writable_attr_name;
	conf5.level = conf3.level;
	conf5.extensions.length(conf3.extensions.length());
	for (j=0; j<conf3.extensions.length(); j++)
	{
		conf5.extensions[j] = string_dup(conf3.extensions[j]);
	}
	for (j=0; j<conf3.sys_extensions.length(); j++)
	{
		conf5.sys_extensions[j] = string_dup(conf3.sys_extensions[j]);
	}

	conf5.att_alarm.min_alarm = conf3.att_alarm.min_alarm;
	conf5.att_alarm.max_alarm = conf3.att_alarm.max_alarm;
	conf5.att_alarm.min_warning = conf3.att_alarm.min_warning;
	conf5.att_alarm.max_warning = conf3.att_alarm.max_warning;
	conf5.att_alarm.delta_t = conf3.att_alarm.delta_t;
	conf5.att_alarm.delta_val = conf3.att_alarm.delta_val;
	for (j=0; j<conf3.att_alarm.extensions.length(); j++)
	{
		conf5.att_alarm.extensions[j] = string_dup(conf3.att_alarm.extensions[j]);
	}

	conf5.event_prop.ch_event.rel_change = conf3.event_prop.ch_event.rel_change;
	conf5.event_prop.ch_event.abs_change = conf3.event_prop.ch_event.abs_change;
	for (j=0; j<conf3.event_prop.ch_event.extensions.length(); j++)
	{
		conf5.event_prop.ch_event.extensions[j] = string_dup(conf3.event_prop.ch_event.extensions[j]);
	}

	conf5.event_prop.per_event.period = conf3.event_prop.per_event.period;
	for (j=0; j<conf3.event_prop.per_event.extensions.length(); j++)
	{
		conf5.event_prop.per_event.extensions[j] = string_dup(conf3.event_prop.per_event.extensions[j]);
	}

	conf5.event_prop.arch_event.rel_change = conf3.event_prop.arch_event.rel_change;
	conf5.event_prop.arch_event.abs_change = conf3.event_prop.arch_event.abs_change;
	conf5.event_prop.arch_event.period = conf3.event_prop.arch_event.period;
	for (j=0; j<conf3.event_prop.ch_event.extensions.length(); j++)
	{
		conf5.event_prop.arch_event.extensions[j] = string_dup(conf3.event_prop.arch_event.extensions[j]);
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::fire_change_event
//
// description :
//		Fire a change change event for the attribute value.
//
// arguments:
// 		in :
//			- ptr : Pointer to a DevFailed exception to fire in case of an error to indicate.
//
//---------------------------------------------------------------------------------------------------------------------


void Attribute::fire_change_event(DevFailed *except)
{
	cout4 << "Attribute::fire_change_event() entering ..." << endl;

	if ( except != NULL )
	{
		set_value_flag(false);
	}

//
// Check if it is needed to send an event
//

	Tango::AttributeValue_3 *send_attr = Tango_nullptr;
	Tango::AttributeValue_4 *send_attr_4 = Tango_nullptr;
	Tango::AttributeValue_5 *send_attr_5 = Tango_nullptr;

	try
	{
		time_t now;
		time_t change3_subscription,change4_subscription,change5_subscription;

		now = time(NULL);

		{
			omni_mutex_lock oml(EventSupplier::get_event_mutex());
			change3_subscription = now - event_change3_subscription;
			change4_subscription = now - event_change4_subscription;
			change5_subscription = now - event_change5_subscription;
		}

//
// Get the event supplier(s)
//

		EventSupplier *event_supplier_nd = NULL;
		EventSupplier *event_supplier_zmq = NULL;
        bool pub_socket_created = false;

		Tango::Util *tg = Util::instance();
		if (use_notifd_event() == true)
        {
            event_supplier_nd = tg->get_notifd_event_supplier();
            pub_socket_created = true;
        }
        if (use_zmq_event() == true)
            event_supplier_zmq = tg->get_zmq_event_supplier();


//
// Get client lib and if it's possible to send event (ZMQ socket created)
//

        vector<int> client_libs;
        {
            omni_mutex_lock oml(EventSupplier::get_event_mutex());
            client_libs = get_client_lib(CHANGE_EVENT); 	// We want a copy
            if (use_zmq_event() == true && event_supplier_zmq != NULL)
            {
                string &sock_endpoint = static_cast<ZmqEventSupplier *>(event_supplier_zmq)->get_event_endpoint();
                if (sock_endpoint.empty() == false)
                    pub_socket_created = true;
            }
        }

		vector<int>::iterator ite;
		for (ite = client_libs.begin();ite != client_libs.end();++ite)
		{
			switch (*ite)
			{
				case 5:
				if (change5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(5,string(EventName[CHANGE_EVENT]));
				break;

				case 4:
				if (change4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(4,string(EventName[CHANGE_EVENT]));
				break;

				default:
				if (change3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(3,string(EventName[CHANGE_EVENT]));
				break;
			}
		}

		if (client_libs.empty() == true)
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
//						set_value_flag (false);
					}
				}
			}
			return;
		}

//
// Simply return if event supplier(s) are not created
//

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
//						set_value_flag (false);
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

						Except::throw_exception(API_AttrValueNotSet,o.str(),"Attribute::fire_change_event()");
					}
				}
			}
		}

//
// Build one AttributeValue_3, AttributeValue_4 or AttributeValue_5 object
//

		long vers = dev->get_dev_idl_version();
		try
		{
			if (vers >= 4)
				send_attr_5 = new Tango::AttributeValue_5;
			else if (vers == 4)
				send_attr_4 = new Tango::AttributeValue_4;
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc &)
		{
			Except::throw_exception(API_MemoryAllocation,
									"Can't allocate memory in server","Attribute::fire_change_event()");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr_5 != Tango_nullptr)
				Attribute_2_AttributeValue(send_attr_5,dev);
			else if (send_attr_4 != NULL)
				Attribute_2_AttributeValue(send_attr_4,dev);
			else
				Attribute_2_AttributeValue(send_attr,dev);
		}

//
// Create the structure used to send data to event system
//

        EventSupplier::SuppliedEventData ad;
        ::memset(&ad,0,sizeof(ad));

//
// Fire event
//

		if ( is_check_change_criteria() == true )
		{

			if (send_attr_5 != Tango_nullptr)
				ad.attr_val_5 = send_attr_5;
            else if (send_attr_4 != Tango_nullptr)
                ad.attr_val_4 = send_attr_4;
            else
                ad.attr_val_3 = send_attr;

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
                    if (send_event == true && pub_socket_created == true)
                    {

                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

						event_supplier_zmq->push_event_loop(dev,CHANGE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,*this,except);
                    }
                }
                else
                {
                     if (pub_socket_created == true)
                        event_supplier_zmq->detect_and_push_change_event(dev,ad,*this,name,except,true);
                }

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

			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			{
				omni_mutex_lock oml(EventSupplier::get_event_mutex());

				if ((except != NULL) ||
					(quality == Tango::ATTR_INVALID) ||
					((except == NULL) && (prev_change_event.err == true)) ||
					((quality != Tango::ATTR_INVALID) &&
					(prev_change_event.quality == Tango::ATTR_INVALID)))
				{
					force_change = true;
				}

				if (except != NULL)
				{
					prev_change_event.err    = true;
					prev_change_event.except = *except;
				}
				else
				{
					Tango::AttrQuality the_quality;

					if (send_attr_5 != NULL)
					{
						the_quality = send_attr_5->quality;
						prev_change_event.value_4 = send_attr_5->value;
					}
					else if (send_attr_4 != NULL)
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
			}

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

			if (send_attr_5 != Tango_nullptr)
				ad.attr_val_5 = send_attr_5;
            else if (send_attr_4 != Tango_nullptr)
                ad.attr_val_4 = send_attr_4;
            else
                ad.attr_val_3 = send_attr;

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
                                        except,
                                        false);

            if (event_supplier_zmq != NULL && pub_socket_created == true)
			{
				event_supplier_zmq->push_event_loop(dev,CHANGE_EVENT,filterable_names,filterable_data,filterable_names_lg,filterable_data_lg,ad,*this,except);
			}
		}

//
// Return allocated memory
//

		if (send_attr_5 != Tango_nullptr)
			delete send_attr_5;
		else if (send_attr_4 != Tango_nullptr)
			delete send_attr_4;
		else
			delete send_attr;

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
//				set_value_flag (false);
			}
		}
	}
	catch (...)
	{
		if (send_attr_5 != Tango_nullptr)
			delete send_attr_5;
		else if (send_attr_4 != NULL)
			delete send_attr_4;
		else
			delete send_attr;

		if ( (name_lower != "state") && (name_lower != "status"))
		{

// delete the data values allocated in the attribute

			bool data_flag = get_value_flag();
			if ( data_flag == true )
			{
				if (quality != Tango::ATTR_INVALID)
					delete_seq();
//				set_value_flag (false);
			}
		}

		throw;
	}
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::fire_archive_event
//
// description :
//		Fire a archive change event for the attribute value.
//
// arguments:
// 		in :
//			- ptr : Pointer to a DevFailed exception to fire in case of an error to indicate.
//
//-------------------------------------------------------------------------------------------------------------------

void Attribute::fire_archive_event(DevFailed *except)
{
	cout4 << "Attribute::fire_archive_event() entering ..." << endl;

	if ( except != NULL )
	{
		set_value_flag (false);
	}

//
// Check if it is needed to send an event
//

	Tango::AttributeValue_3 *send_attr = Tango_nullptr;
	Tango::AttributeValue_4 *send_attr_4 = Tango_nullptr;
	Tango::AttributeValue_5 *send_attr_5 = Tango_nullptr;

	try
	{
		time_t now;
		time_t archive3_subscription,archive4_subscription,archive5_subscription;

		now = time(NULL);

		{
			omni_mutex_lock oml(EventSupplier::get_event_mutex());
			archive3_subscription = now - event_archive3_subscription;
			archive4_subscription = now - event_archive4_subscription;
			archive5_subscription = now - event_archive5_subscription;
		}

//
// Get the event supplier(s)
//

		EventSupplier *event_supplier_nd = NULL;
		EventSupplier *event_supplier_zmq = NULL;
		bool pub_socket_created = false;

		Tango::Util *tg = Util::instance();
		if (use_notifd_event() == true)
        {
            event_supplier_nd = tg->get_notifd_event_supplier();
            pub_socket_created = true;
        }
        if (use_zmq_event() == true)
            event_supplier_zmq = tg->get_zmq_event_supplier();

//
// Get client lib and if it's possible to send event (ZMQ socket created)
//

        vector<int> client_libs;
        {
            omni_mutex_lock oml(EventSupplier::get_event_mutex());
            client_libs = get_client_lib(ARCHIVE_EVENT);        // We want a copy
            if (use_zmq_event() == true && event_supplier_zmq != NULL)
            {
                string &sock_endpoint = static_cast<ZmqEventSupplier *>(event_supplier_zmq)->get_event_endpoint();
                if (sock_endpoint.empty() == false)
                    pub_socket_created = true;
            }
        }

		vector<int>::iterator ite;
		for (ite = client_libs.begin();ite != client_libs.end();++ite)
		{
			switch (*ite)
			{
				case 5:
				if (archive5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(5,string(EventName[ARCHIVE_EVENT]));
				break;

				case 4:
				if (archive4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(4,string(EventName[ARCHIVE_EVENT]));
				break;

				default:
				if (archive3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(3,string(EventName[ARCHIVE_EVENT]));
				break;
			}
		}

		if (client_libs.empty() == true)
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
// attribute data is not yet allocated. This will happen
// only when adding the set point!
//

					if ( !check_scalar_wattribute() )
					{
						if (quality != Tango::ATTR_INVALID)
							delete_seq();
//						set_value_flag (false);
					}
                }
            }
			return;
        }

//
// Simply return if event supplier(s) are not created
//

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
//						set_value_flag (false);
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
// Build one AttributeValue_3, AttributeValue_4 or AttributeValue_5 object
//

		try
		{
			if (dev->get_dev_idl_version() > 4)
				send_attr_5 = new Tango::AttributeValue_5;
			else if (dev->get_dev_idl_version() == 4)
				send_attr_4 = new Tango::AttributeValue_4;
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc &)
		{
			Except::throw_exception(API_MemoryAllocation,
									"Can't allocate memory in server","Attribute::fire_archive_event()");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr_5 != Tango_nullptr)
				Attribute_2_AttributeValue(send_attr_5,dev);
			else if (send_attr_4 != Tango_nullptr)
				Attribute_2_AttributeValue(send_attr_4,dev);
			else
				Attribute_2_AttributeValue(send_attr,dev);
		}

//
// Create the structure used to send data to event system
//

        EventSupplier::SuppliedEventData ad;
        ::memset(&ad,0,sizeof(ad));

		if (send_attr_5 != Tango_nullptr)
			ad.attr_val_5 = send_attr_5;
        else if (send_attr_4 != Tango_nullptr)
            ad.attr_val_4 = send_attr_4;
        else
            ad.attr_val_3 = send_attr;

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
                    if (send_event == true && pub_socket_created == true)
                    {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

						event_supplier_zmq->push_event_loop(dev,ARCHIVE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,*this,except);
                    }
                }
                else
                {
                    if (pub_socket_created == true)
                        event_supplier_zmq->detect_and_push_archive_event(dev,ad,*this,name,except,&now_timeval,true);
                }

            }
		}
		else
		{

//
//

			bool force_change   = false;
			bool quality_change = false;
			double delta_change_rel = 0.0;
			double delta_change_abs = 0.0;

			{
				omni_mutex_lock oml(EventSupplier::get_event_mutex());

				// Execute detect_change only to calculate the delta_change_rel and
				// delta_change_abs and force_change !

				if (event_supplier_nd || event_supplier_zmq)
				{
					EventSupplier* event_supplier = event_supplier_nd ? event_supplier_nd : event_supplier_zmq;
					event_supplier->detect_change(
					    *this,
					    ad,
					    true,
					    delta_change_rel,
					    delta_change_abs,
					    except,
					    force_change,
					    dev);
				}

				if (except != NULL)
				{
					prev_archive_event.err    = true;
					prev_archive_event.except = *except;
				}
				else
				{
					Tango::AttrQuality the_quality;

					if (send_attr_5 != Tango_nullptr)
					{
						prev_archive_event.value_4 = send_attr_5->value;
						the_quality = send_attr_5->quality;
					}
					else if (send_attr_4 != Tango_nullptr)
					{
						prev_archive_event.value_4 = send_attr_4->value;
						the_quality = send_attr_4->quality;
					}
					else
					{
						prev_archive_event.value = send_attr->value;
						the_quality = send_attr->quality;
					}

					if (prev_archive_event.quality != the_quality)
					{
						quality_change = true;
					}

					prev_archive_event.quality = the_quality;
					prev_archive_event.err = false;
				}
				prev_archive_event.inited = true;
			}

			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

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
							except,
							false);

            if (event_supplier_zmq != NULL && pub_socket_created == true)
			{
				event_supplier_zmq->push_event_loop(dev,ARCHIVE_EVENT,filterable_names,filterable_data,filterable_names_lg,filterable_data_lg,ad,*this,except);
			}
		}

		if (send_attr_5 != Tango_nullptr)
			delete send_attr_5;
		else if (send_attr_4 != Tango_nullptr)
			delete send_attr_4;
		else
			delete send_attr;

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
//				set_value_flag (false);
			}
		}
	}
	catch (...)
	{
		if (send_attr_5 != Tango_nullptr)
			delete send_attr_5;
		else if (send_attr_4 != Tango_nullptr)
			delete send_attr_4;
		else
			delete send_attr;

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
//				set_value_flag (false);
			}
		}

		throw;
	}
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::fire_event
//
// description :
//		Fire a user event for the attribute value.
//
// arguments:
// 		in :
//			- filt_names : The filterable fields name
//			- filt_vals : The filterable fields value (as double)
//			- except : Pointer to a DevFailed exception to fire in case of an error to indicate.
//
//--------------------------------------------------------------------------------------------------------------------


void Attribute::fire_event(vector<string> &filt_names,vector<double> &filt_vals,DevFailed *except)
{
	cout4 << "Attribute::fire_event() entring ..." << endl;

	if (except != NULL)
		set_value_flag(false);

	Tango::AttributeValue_3 *send_attr = Tango_nullptr;
	Tango::AttributeValue_4 *send_attr_4 = Tango_nullptr;
	Tango::AttributeValue_5 *send_attr_5 = Tango_nullptr;

//
// Check if it is needed to send an event
//

	try
	{

		time_t now;
		time_t user3_subscription,user4_subscription,user5_subscription;

		now = time(NULL);

		{
			omni_mutex_lock oml(EventSupplier::get_event_mutex());
			user3_subscription = now - event_user3_subscription;
			user4_subscription = now - event_user4_subscription;
			user5_subscription = now - event_user5_subscription;
		}

//
// Get the event supplier(s)
//

		EventSupplier *event_supplier_nd = NULL;
		EventSupplier *event_supplier_zmq = NULL;
		bool pub_socket_created = false;

		Tango::Util *tg = Util::instance();
		if (use_notifd_event() == true)
        {
            event_supplier_nd = tg->get_notifd_event_supplier();
            pub_socket_created = true;
        }
        if (use_zmq_event() == true)
            event_supplier_zmq = tg->get_zmq_event_supplier();

//
// Get client lib and if it's possible to send event (ZMQ socket created)
//

        vector<int> client_libs;
        {
            omni_mutex_lock oml(EventSupplier::get_event_mutex());
            client_libs = get_client_lib(USER_EVENT);        // We want a copy
            if (use_zmq_event() == true && event_supplier_zmq != NULL)
            {
                string &sock_endpoint = static_cast<ZmqEventSupplier *>(event_supplier_zmq)->get_event_endpoint();
                if (sock_endpoint.empty() == false)
                    pub_socket_created = true;
            }
        }

		vector<int>::iterator ite;
		for (ite = client_libs.begin();ite != client_libs.end();++ite)
		{
			switch (*ite)
			{
				case 5:
				if (user5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(5,string(EventName[USER_EVENT]));
				break;

				case 4:
				if (user4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(4,string(EventName[USER_EVENT]));
				break;

				default:
				if (user3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
					remove_client_lib(3,string(EventName[USER_EVENT]));
				break;
			}
		}

//
// Simply return if event suplier(s) are not created
//

		if (((event_supplier_nd == NULL) && (event_supplier_zmq == NULL)) || client_libs.empty() == true)
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
//						set_value_flag (false);
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

						Except::throw_exception(API_AttrValueNotSet,o.str(),"Attribute::fire_event()");
					}
				}
			}
		}

//
// Build one AttributeValue_3, AttributeValue_4 object or Attribute_5 object
//

		try
		{
			if (dev->get_dev_idl_version() > 4)
				send_attr_5 = new Tango::AttributeValue_5;
			else if (dev->get_dev_idl_version() == 4)
				send_attr_4 = new Tango::AttributeValue_4;
			else
				send_attr = new Tango::AttributeValue_3;
		}
		catch (bad_alloc &)
		{
			Except::throw_exception(API_MemoryAllocation,"Can't allocate memory in server","Attribute::fire_event()");
		}

//
// Don`t try to access the attribute data when an exception was indicated
//

		if ( except == NULL )
		{
			if (send_attr_5 != Tango_nullptr)
				Attribute_2_AttributeValue(send_attr_5,dev);
			else if (send_attr_4 != Tango_nullptr)
				Attribute_2_AttributeValue(send_attr_4,dev);
			else
				Attribute_2_AttributeValue(send_attr,dev);
		}

//
// Create the structure used to send data to event system
//

        EventSupplier::SuppliedEventData ad;
        ::memset(&ad,0,sizeof(ad));

		if (send_attr_5 != Tango_nullptr)
			ad.attr_val_5 = send_attr_5;
        else if (send_attr_4 != NULL)
            ad.attr_val_4 = send_attr_4;
        else
            ad.attr_val_3 = send_attr;

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
					   except,
					   false);
        if (event_supplier_zmq != NULL && pub_socket_created == true)
		{
			event_supplier_zmq->push_event_loop(dev,USER_EVENT,filt_names,filt_vals,filterable_names_lg,filterable_data_lg,ad,*this,except);
		}

		if (send_attr_5 != Tango_nullptr)
			delete send_attr_5;
		else if (send_attr_4 != Tango_nullptr)
			delete send_attr_4;
		else
			delete send_attr;

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
//				set_value_flag (false);
			}
		}
	}
	catch (...)
	{
		if (send_attr_5 != Tango_nullptr)
			delete send_attr_5;
		else if (send_attr_4 != Tango_nullptr)
			delete send_attr_4;
		else
			delete send_attr;

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
//				set_value_flag (false);
			}
		}

		throw;
	}
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::fire_error_periodic_event
//
// description :
//		Fire a periodic event with error set. This is used when attribute polling is stopped
//
// arguments:
// 		in :
//			- except : Pointer to a DevFailed exception to fire in case of an error to indicate.
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::fire_error_periodic_event(DevFailed *except)
{
	cout4 << "Attribute::fire_error_periodic_event() entring ..." << endl;

//
// Check if it is needed to send an event
//

	time_t now;
	time_t periodic3_subscription,periodic4_subscription,periodic5_subscription;

	now = time(NULL);

	{
		omni_mutex_lock oml(EventSupplier::get_event_mutex());
		periodic3_subscription = now - event_periodic3_subscription;
		periodic4_subscription = now - event_periodic4_subscription;
		periodic5_subscription = now - event_periodic5_subscription;
	}

	vector<int> client_libs = get_client_lib(PERIODIC_EVENT); 	// We want a copy

	vector<int>::iterator ite;
	for (ite = client_libs.begin();ite != client_libs.end();++ite)
	{
		switch (*ite)
		{
			case 5:
			if (periodic5_subscription >= EVENT_RESUBSCRIBE_PERIOD)
				remove_client_lib(5,string(EventName[PERIODIC_EVENT]));
			break;

			case 4:
			if (periodic4_subscription >= EVENT_RESUBSCRIBE_PERIOD)
				remove_client_lib(4,string(EventName[PERIODIC_EVENT]));
			break;

			default:
			if (periodic3_subscription >= EVENT_RESUBSCRIBE_PERIOD)
				remove_client_lib(3,string(EventName[PERIODIC_EVENT]));
			break;
		}
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

	if (((event_supplier_nd == NULL) && (event_supplier_zmq == NULL)) || client_libs.empty() == true)
	{
		return;
	}

//
// Retrieve device object if not already done
//

	if (dev == NULL)
		dev = tg->get_device_by_name(d_name);

//
// Create the structure used to send data to event system
//

	EventSupplier::SuppliedEventData ad;
	::memset(&ad,0,sizeof(ad));

//
// Fire event
//

	vector<string> filterable_names_lg,filt_names;
	vector<long> filterable_data_lg;
	vector<double> filt_vals;

	if (event_supplier_nd != NULL)
		event_supplier_nd->push_event(dev,
				   "periodic_event",
				   filt_names,
				   filt_vals,
				   filterable_names_lg,
				   filterable_data_lg,
				   ad,
				   name,
				   except,
				   false);
	if (event_supplier_zmq != NULL)
	{
		event_supplier_zmq->push_event_loop(dev,PERIODIC_EVENT,filt_names,filt_vals,filterable_names_lg,filterable_data_lg,ad,*this,except);
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
	case Tango::DEV_ENUM:
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
		catch (CORBA::COMM_FAILURE &)
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
		catch (CORBA::COMM_FAILURE &)
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
			catch (CORBA::COMM_FAILURE &)
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

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::get_att_device_class
//
// description :
//		Return a pointer to the attribute device class
//
// argument :
//		in :
//			- dev_name: The device name
//
//-------------------------------------------------------------------------------------------------------------------

DeviceClass *Attribute::get_att_device_class(string &dev_name)
{

//
// Get device class. When the server is started, it's an easy task. When the server is in its starting phase, it's more
// tricky. Get from the DeviceClass list the first one for which the device_factory method has not yet been fully
// executed. This is the DeviceClass with the device in its init_device() method has called Attribute::set_properties()
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
            else
            {
                // If a server wants to set attribute properties during a "write memorized attribute value at init"
                // phase, we might be in this case...
                vector< DeviceImpl * > & dev_list = tmp_cl_list[loop]->get_device_list();
                // Check whether our device is listed in this class
                for (size_t i = 0; i < dev_list.size(); ++i)
                {
                    if (dev_list[i]->get_name() == dev_name)
                    {
                        // Our device is listed in this class, returns the corresponding DeviceClass pointer
                        return tmp_cl_list[loop];
                    }
                }
            }
        }

        if (loop != tmp_cl_list.size())
        {
            dev_class = tmp_cl_list[loop];
        }
        else
        {
			stringstream o;
			o << "Device " << dev_name << "-> Attribute : " << name;
			o << "\nCan't retrieve device class!" << ends;

			Except::throw_exception(API_CantRetrieveClass,o.str(),"Attribute::set_properties()");
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

    const bool has_quality_changed = quality != old_quality;
    const bool has_alarm_changed = alarm != old_alarm;
    const bool is_alarm_set = alarm.any();

    if (has_quality_changed)
    {
        if (! is_alarm_set)
        {
            switch (quality)
            {
            case ATTR_INVALID:
                DEV_ERROR_STREAM(dev) << "INVALID quality for attribute " << name << std::endl;
                break;

            case ATTR_CHANGING:
                DEV_INFO_STREAM(dev) << "CHANGING quality for attribute " << name << std::endl;
                break;

            case ATTR_VALID:
                DEV_INFO_STREAM(dev) << "INFO quality for attribute " << name << std::endl;
                break;

            default:
                break;
            }
        }
        else
        {
            log_alarm_quality();
        }
    }
    else if (has_alarm_changed)
    {
        log_alarm_quality();
    }
}

void Attribute::log_alarm_quality() const
{
    if (alarm[min_level])
    {
        DEV_ERROR_STREAM(dev) << "MIN ALARM for attribute " << name << std::endl;
    }
    else if (alarm[max_level])
    {
        DEV_ERROR_STREAM(dev) << "MAX ALARM for attribute " << name << std::endl;
    }
    else if (alarm[rds])
    {
        DEV_WARN_STREAM(dev) << "RDS (Read Different Set) ALARM for attribute " << name << std::endl;
    }
    else if (alarm[min_warn])
    {
        DEV_WARN_STREAM(dev) << "MIN WARNING for attribute " << name << std::endl;
    }
    else if (alarm[max_warn])
    {
        DEV_WARN_STREAM(dev) << "MAX WARNING for attribute " << name << std::endl;
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
            catch (CORBA::COMM_FAILURE &)
            {
                tg->get_database()->reconnect(true);
            }
        }
    }
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::avns_in_att()
//
// description :
//		Store in att the famous AVNS (AlrmValueNotSpec) for a specific attribute property
//
// Arguments:
//		in :
//			- pt : Property type
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
	case DEV_ENUM:
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
	case DEV_ENUM:
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
	case DEV_ENUM:
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
	time_t now = time(NULL);

	if (event_change5_subscription != 0)
	{
		if (now - event_change5_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	if (ret == false)
	{
		if (event_change4_subscription != 0)
		{
			if (now - event_change4_subscription > EVENT_RESUBSCRIBE_PERIOD)
				ret = false;
			else
				ret = true;
		}

		if (ret == false)
		{
			if (event_change3_subscription != 0)
			{
				if (now - event_change3_subscription > EVENT_RESUBSCRIBE_PERIOD)
					ret = false;
				else
					ret = true;
			}
		}
	}
	return ret;
}

bool Attribute::periodic_event_subscribed()
{
	bool ret = false;
	time_t now = time(NULL);

	if (event_periodic5_subscription != 0)
	{
		if (now - event_periodic5_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	if (ret == false)
	{
		if (event_periodic4_subscription != 0)
		{
			if (now - event_periodic4_subscription > EVENT_RESUBSCRIBE_PERIOD)
				ret = false;
			else
				ret = true;
		}

		if (ret == false)
		{
			if (event_periodic3_subscription != 0)
			{
				if (now - event_periodic3_subscription > EVENT_RESUBSCRIBE_PERIOD)
					ret = false;
				else
					ret = true;
			}
		}
	}
	return ret;
}

bool Attribute::archive_event_subscribed()
{
	bool ret = false;
	time_t now = time(NULL);

	if (event_archive5_subscription != 0)
	{
		if (now - event_archive5_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	if (ret == false)
	{
		if (event_archive4_subscription != 0)
		{
			if (now - event_archive4_subscription > EVENT_RESUBSCRIBE_PERIOD)
				ret = false;
			else
				ret = true;
		}

		if (ret == false)
		{
			if (event_archive3_subscription != 0)
			{
				if (now - event_archive3_subscription > EVENT_RESUBSCRIBE_PERIOD)
					ret = false;
				else
					ret = true;
			}
		}
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
	time_t now = time(NULL);

	if (event_user5_subscription != 0)
	{
		if (now - event_user5_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	if (ret == false)
	{
		if (event_user4_subscription != 0)
		{
			if (now - event_user4_subscription > EVENT_RESUBSCRIBE_PERIOD)
				ret = false;
			else
				ret = true;
		}

		if (ret == false)
		{
			if (event_user3_subscription != 0)
			{
				if (now - event_user3_subscription > EVENT_RESUBSCRIBE_PERIOD)
					ret = false;
				else
					ret = true;
			}
		}
	}
	return ret;
}

bool Attribute::attr_conf_event_subscribed()
{
	bool ret = false;

	if (event_attr_conf5_subscription != 0)
	{
		time_t now = time(NULL);
		if (now - event_attr_conf5_subscription > EVENT_RESUBSCRIBE_PERIOD)
			ret = false;
		else
			ret = true;
	}

	if (ret == false)
	{
		if (event_attr_conf_subscription != 0)
		{
			time_t now = time(NULL);
			if (now - event_attr_conf_subscription > EVENT_RESUBSCRIBE_PERIOD)
				ret = false;
			else
				ret = true;
		}
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

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_client_lib()
//
// description :
//		Set client lib (for event compatibility)
//
// argument :
//		in :
//			- _l : Client lib release
//			- ev_name : Event name
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::set_client_lib(int _l,string &ev_name)
{
	cout4 << "Attribute::set_client_lib(" << _l << "," << ev_name << ")" << endl;
	int i;
	for (i = 0; i < numEventType; i++)
	{
		if (ev_name == EventName[i])
		{
			break;
		}
	}

	if (count(client_lib[i].begin(), client_lib[i].end(), _l) == 0)
	{
		client_lib[i].push_back(_l);
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::remove_client_lib()
//
// description :
//		Remove a client lib (for event compatibility)
//
// argument :
//		in :
//			- _l : Client lib release
//			- ev_name : Event name
//
//--------------------------------------------------------------------------------------------------------------------

void Attribute::remove_client_lib(int _l,const string &ev_name)
{
	int i;
	for (i = 0;i < numEventType;i++)
	{
		if (ev_name == EventName[i])
			break;
	}

	vector<int>::iterator pos = find(client_lib[i].begin(),client_lib[i].end(),_l);
	if (pos != client_lib[i].end())
		client_lib[i].erase(pos);
}

//-------------------------------------------------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing Attribute class instance
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


	o_str << "Attribute name = " << conf.name.in() << endl;
	o_str << "Attribute data_type = ";
	switch (conf.data_type)
	{
	case Tango::DEV_SHORT :
		o_str << "Tango::DevShort" << endl;
		break;

	case Tango::DEV_LONG :
		o_str << "Tango::DevLong" << endl;
		break;

	case Tango::DEV_LONG64 :
		o_str << "Tango::DevLong64" << endl;
		break;

	case Tango::DEV_DOUBLE :
		o_str << "Tango::DevDouble" << endl;
		break;

	case Tango::DEV_STRING :
		o_str << "Tango::DevString" << endl;
		break;

	case Tango::DEV_FLOAT :
		o_str << "Tango::DevFloat" << endl;
		break;

	case Tango::DEV_USHORT :
		o_str << "Tango::DevUShort" << endl;
		break;

	case Tango::DEV_UCHAR :
		o_str << "Tango::DevUChar" << endl;
		break;

	case Tango::DEV_BOOLEAN :
		o_str << "Tango::DevBoolean" << endl;
		break;

	case Tango::DEV_STATE :
		o_str << "Tango::DevState" << endl;
		break;

	case Tango::DEV_ULONG :
		o_str << "Tango::DevULong" << endl;
		break;

	case Tango::DEV_ULONG64 :
		o_str << "Tango::DevULong64" << endl;
		break;

	case Tango::Dev_ENCODED :
		o_str << "Tango::DevEncoded" << endl;
		break;

	case Tango::DEV_ENUM :
		o_str << "Tango::DevEnum" << endl;
		break;

	case Tango::DATA_TYPE_UNKNOWN :
		o_str << "Unknown" << endl;
		break;
	}

	o_str << "Attribute data_format = ";
	switch (conf.data_format)
	{
	case Tango::SCALAR :
		o_str << "scalar" << endl;
		break;

	case Tango::SPECTRUM :
		o_str << "spectrum, max_dim_x = " << conf.max_dim_x << endl;
		break;

	case Tango::IMAGE :
		o_str << "image, max_dim_x = " << conf.max_dim_x << ", max_dim_y = " << conf.max_dim_y << endl;
		break;

	case Tango::FMT_UNKNOWN :
		o_str << "Unknown" << endl;
		break;
	}

	if ((conf.writable == Tango::WRITE) || (conf.writable == Tango::READ_WRITE))
		o_str << "Attribute is writable" << endl;
	else
		o_str << "Attribute is not writable" << endl;
	o_str << "Attribute label = " << conf.label.in() << endl;
	o_str << "Attribute description = " << conf.description.in() << endl;
	o_str << "Attribute unit = " << conf.unit.in();
	o_str << ", standard unit = " << conf.standard_unit.in();
	o_str << ", display unit = " << conf.display_unit.in() << endl;
	o_str << "Attribute format = " << conf.format.in() << endl;
	o_str << "Attribute min value = " << conf.min_value.in() << endl;
	o_str << "Attribute max value = " << conf.max_value.in() << endl;
	o_str << "Attribute min alarm = " << conf.min_alarm.in() << endl;
	o_str << "Attribute max alarm = " << conf.max_alarm.in() << endl;
	o_str << "Attribute writable_attr_name = " << conf.writable_attr_name.in() << endl;

	return o_str;
}
#endif // TANGO_HAS_LOG4TANGO

} // End of Tango namespace
