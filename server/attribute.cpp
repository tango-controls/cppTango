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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
// $Log$
// Revision 3.88  2011/01/10 14:39:27  taurel
// - Some compilation errors while compiling Tango 7.2.3
//
// Revision 3.87  2010/12/08 10:13:08  taurel
// - Commit after a merge with the bugfixes branch
//
// Revision 3.86.2.1  2010/11/21 08:41:27  taurel
// - Fix SourceForge bug nb 3110842
// (wrong delete in state for spectrm att with alarm defined when
// quality factor set to ATTR_INVALID)
//
// Revision 3.86  2010/11/02 14:06:54  taurel
// - Replace dynamic_cast with static_cast in attribute.cpp.
// - Release number is now 7.2.1
//
// Revision 3.85  2010/10/22 13:20:14  taurel
// - First bug in Tango 7.2 ! Crash when reading R/W scalar string attribute
// without requiring Tango to free memory
//
// Revision 3.84  2010/09/17 08:22:05  taurel
// - Fix memory leak in cse of scalar attribute R/W for string
//
// Revision 3.83  2010/09/09 13:44:46  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.82  2010/07/16 10:51:53  taurel
// - Now it's possible to fill the polling buffer externally for R/W attribute
// specifying the attribute written part
//
// Revision 3.81  2010/06/21 14:01:13  taurel
// - Yet another merge with the Release_7_1_1-bugfixes branch
//
// Revision 3.78.2.1  2010/02/23 15:56:51  taurel
// - Fix bug 2954995 (attribute quality INVALID)
//
// Revision 3.78  2009/10/27 08:22:27  taurel
// - Attribute default value for periodic event period is back to 1 sec
//
// Revision 3.77  2009/10/23 14:36:27  taurel
// - Tango 7.1.1
// - Fix bugs 2880372 and 2881841
// - Now support event in case of Tango system with multi db server
// - The polling threads start with polling inactive
//
// Revision 3.76  2009/09/18 09:18:05  taurel
// - End of attribute serialization implementation?
//
// Revision 3.75  2009/09/17 08:28:05  taurel
// - Add a mutual exclusion to protect attribute buffer
//
// Revision 3.74  2009/09/16 12:24:45  taurel
// - Fix bug in alarm checking in case of scalar attribute read due to a
// state attribute request in a read_attributes() call
//
// Revision 3.73  2009/08/27 07:23:45  taurel
// - Commit after another merge with Release_7_0_2-bugfixes branch
//
// Revision 3.72.2.4  2009/08/25 14:02:00  taurel
// - Clarify an error message
//
// Revision 3.72.2.3  2009/07/17 08:45:58  taurel
// - Forgot to copy the previous bug fix to all push_xxx_event() methods
//
// Revision 3.72.2.2  2009/07/17 08:27:36  taurel
// - Fix bug (valgring complains about it) in push_xxx_event() methods
// for the status attribute
//
// Revision 3.72.2.1  2009/06/22 06:39:56  taurel
// - Fix bug when pushing event with attribute quality factor set to INVALID
//
// Revision 3.72  2009/04/30 15:09:55  taurel
// - Some code managing only IDL 3 was still there !
//
// Revision 3.71  2009/03/30 15:03:44  taurel
// - Fix last bugs before Tango 7 ??
//
// Revision 3.70  2009/03/19 17:50:21  jlpons
// Added management of DevEncoded format
//
// Revision 3.69  2009/03/18 12:18:40  taurel
// - Fix warnings reported when compiled with the option -Wall
//
// Revision 3.68  2009/03/13 09:33:29  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.67  2009/02/04 10:23:47  taurel
// - Fix double free when firing a change event for an attribute with
// an INVALID quality factor (set by set_value_date_quality)
//
// Revision 3.66  2009/01/29 16:23:49  taurel
// - Commit after merge with branch Release_6_1_1-bugfixes
//
// Revision 3.65  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.64  2009/01/15 13:57:10  taurel
// - Fix bugs found by Jens
//
// Revision 3.63  2009/01/06 09:28:50  taurel
// - Add compatibility between a server IDL V4 and clients IDL V3
//
// Revision 3.62  2008/12/17 09:50:59  taurel
// - First implementation of attributes sent on the wire using IDL Union
// instead of IDL Any
//
// Revision 3.61  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.60  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.59  2008/09/23 14:59:31  taurel
// - Commit after the end of DevEncoded data type implementation
// - The new test suite is also now running fine
//
// Revision 3.58  2008/07/03 12:40:05  taurel
// - Clean memory in case of String in the set_value() method when the
// release flag is set to true
//
// Revision 3.57  2008/07/03 11:29:27  taurel
// - Clean-up memory allocation in set_value() methods
// - Continue Device_4 implementation in fire_xxx event methods
//
// Revision 3.56  2008/07/01 07:38:39  taurel
// - Some more code for a proper implementation of the DevEncoded data type with the new IDL release 4
//
// Revision 3.55  2008/06/14 11:29:15  taurel
// - DevEncoded attribute data type implementation work going on
//
// Revision 3.54  2008/06/10 07:52:14  taurel
// - Add code for the DevEncoded attribute data type
//
// Revision 3.53  2008/05/20 12:44:09  taurel
// - Commit after merge with release 7 branch
// Revision 3.52.2.1  2009/01/29 15:17:01  taurel
// - Add some bug fixes for server used with the -f option (database as
// file)
//
// Revision 3.52  2008/03/26 16:52:59  taurel
// - Remove a buggy test when setting the attribute config (linked to RDS delta_t
// and delta_val)
//
// Revision 3.51  2008/03/25 15:48:44  taurel
// - Fix some warnings for gcc 4.2 (Thank's to F Picca from Soleil)
//
// Revision 3.50  2008/03/25 15:34:25  taurel
// - Add 2 Attribute::set_properties() methods
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

	ext = new AttributeExt();
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

	init_event_prop(prop_list);
	
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

void Attribute::init_event_prop(vector<AttrProperty> &prop_list)
{
	ext->rel_change[0] = INT_MAX;			// default for relative change is none
	ext->rel_change[1] = INT_MAX;			// default for relative change is none 
	ext->abs_change[0] = INT_MAX;			// default for absolute change is none
	ext->abs_change[1] = INT_MAX;			// default for absolute change is none
	ext->archive_rel_change[0] = INT_MAX;		// default for archive change is none 
	ext->archive_rel_change[1] = INT_MAX;		// default for archive change is none
	ext->archive_abs_change[0] = INT_MAX;		// default for archive change is none
	ext->archive_abs_change[1] = INT_MAX;		// default for archive change is none

//
// Init min and max relative change for change event
//

	try
	{
        string rel_change_str;
        rel_change_str = get_attr_value(prop_list,"rel_change");

        TangoSys_MemStream str;
		double rel_change_min=INT_MAX, rel_change_max=INT_MAX;
               str << rel_change_str << ends;
		str >> rel_change_min;
		str >> rel_change_max;

		if (fabs(rel_change_min) > 0 && rel_change_min != INT_MAX) 
		{
			ext->rel_change[0] = -fabs(rel_change_min);
			ext->rel_change[1] = fabs(rel_change_min);
        }                                 
		if (rel_change_max > 0 && rel_change_max != INT_MAX) 
		{
			ext->rel_change[1] = fabs(rel_change_max);
        }                                 
		cout1 << "Attribute::Attribute(): rel_change = " << ext->rel_change[0] << " " << ext->rel_change[1] << endl;
	}
	catch (...) {}

//
// Init min and max absolute change for change event
//

	try
	{
        string abs_change_str;
        abs_change_str = get_attr_value(prop_list,"abs_change");

        TangoSys_MemStream str;
		double abs_change_min=INT_MAX, abs_change_max=INT_MAX;
        str << abs_change_str << ends;
		str >> abs_change_min;
		str >> abs_change_max;
        if (abs_change_min != INT_MAX) 
		{
			ext->abs_change[0] = -fabs(abs_change_min);
			ext->abs_change[1] = fabs(abs_change_min);
        }                                 
        if (abs_change_max != INT_MAX) 
		{
			ext->abs_change[0] = abs_change_min;
			ext->abs_change[1] = abs_change_max;
        }                                 
		cout1 << "Attribute::Attribute(): rel_change = " << ext->rel_change[0] << " " << ext->rel_change[1] << endl;
		str.seekp(0); 
		str.seekg(0);
	}
	catch (...) { }

//
// Init min and max relative change for archive event
//

	try
	{
        string archive_rel_change_str;
        archive_rel_change_str = get_attr_value(prop_list,"archive_rel_change");

        TangoSys_MemStream str;
		double archive_rel_change_min=INT_MAX, archive_rel_change_max=INT_MAX;
        str << archive_rel_change_str << ends;
		str >> archive_rel_change_min;
		str >> archive_rel_change_max;
        if (fabs(archive_rel_change_min) > 0 && archive_rel_change_min != INT_MAX) 
		{
			ext->archive_rel_change[0] = -fabs(archive_rel_change_min);
			ext->archive_rel_change[1] = fabs(archive_rel_change_min);
        }                                 
        if (archive_rel_change_max > 0 && archive_rel_change_max != INT_MAX) 
		{
			ext->archive_rel_change[1] = fabs(archive_rel_change_max);
        }                                 
		cout1 << "Attribute::Attribute(): archive rel_change = " << ext->archive_rel_change[0] << " " << ext->archive_rel_change[1] << endl;
		str.seekp(0); 
		str.seekg(0);
	}
	catch (...) { }

//
// Init min and max absolute change for archive event
//

	try
	{
        string archive_abs_change_str;
        archive_abs_change_str = get_attr_value(prop_list,"archive_abs_change");

        TangoSys_MemStream str;
		double archive_abs_change_min=INT_MAX, archive_abs_change_max=INT_MAX;
        str << archive_abs_change_str << ends;
		str >> archive_abs_change_min;
		str >> archive_abs_change_max;
        if (fabs(archive_abs_change_min) > 0 && archive_abs_change_min != INT_MAX) 
		{
			ext->archive_abs_change[0] = -fabs(archive_abs_change_min);
			ext->archive_abs_change[1] = fabs(archive_abs_change_min);
        }                                 
        if (archive_abs_change_max > 0 && archive_abs_change_max != INT_MAX) 
		{
			ext->archive_abs_change[1] = fabs(archive_abs_change_max);
        }                                 
		cout1 << "Attribute::Attribute(): archive rel_change = " << ext->archive_rel_change[0] << " " << ext->archive_rel_change[1] << endl;
		str.seekp(0); 
		str.seekg(0);
	}
	catch (...) { }

//
// Init period for periodic event
//

	ext->event_period = (int)(DEFAULT_EVENT_PERIOD);  	// default for event period is 1 second
//	ext->event_period = (int)(INT_MAX);

	try
	{
    	string event_period_str;
    	event_period_str = get_attr_value(prop_list,"event_period");

    	TangoSys_MemStream str;
    	if (event_period_str != AlrmValueNotSpec)
    	{
			long event_period = 0;
     		str << event_period_str << ends;
         	str >> event_period;
			if (event_period > 0)
			{
				ext->event_period = event_period;
			}
			cout1 << "Attribute::Attribute(): event_period_str " << event_period_str << " event_period = " << ext->event_period << endl;
    	}                                 
	}
	catch (...) { }

//
// Init period for archive event
//

	ext->archive_period = (int)(INT_MAX);

	try
	{
    	string archive_period_str;
    	archive_period_str = get_attr_value(prop_list,"archive_period");
		
    	TangoSys_MemStream str;
    	if (archive_period_str != AlrmValueNotSpec)
    	{
			long arch_period = 0;
     		str << archive_period_str << ends;
			str >> arch_period;
			if (arch_period > 0)
			{
				ext->archive_period = arch_period;
			}
			cout1 << "Attribute::Attribute(): archive event_period = " << ext->archive_period << endl;
        }                                 
	}
	catch (...) { }

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
	if ((min_alarm_str != AlrmValueNotSpec) && 
	    (data_type != Tango::DEV_STRING) &&
	    (data_type != Tango::DEV_BOOLEAN) &&
	    (data_type != Tango::DEV_STATE))
	{
		str << min_alarm_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> min_alarm.sh))
				throw_err_format("min_alarm",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> min_alarm.db))
				throw_err_format("min_alarm",dev_name);
			min_alarm.lg = (DevLong)min_alarm.db;
			break;
			
		case Tango::DEV_LONG64:
			if (!(str >> min_alarm.db))
				throw_err_format("min_alarm",dev_name);
			min_alarm.lg64 = (DevLong64)min_alarm.db;
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> min_alarm.db))
				throw_err_format("min_alarm",dev_name);
			break;
		case Tango::DEV_FLOAT:
			if (!(str >> min_alarm.fl))
				throw_err_format("min_alarm",dev_name);
			break;
			
		case Tango::DEV_USHORT:
			if (!(str >> min_alarm.ush))
				throw_err_format("min_alarm",dev_name);
			break;
			
		case Tango::DEV_UCHAR:
			if (!(str >> min_alarm.sh))
				throw_err_format("min_alarm",dev_name);
			min_alarm.uch = (unsigned char)min_alarm.sh;
			break;
			
		case Tango::DEV_ULONG:
			if (!(str >> min_alarm.db))
				throw_err_format("min_alarm",dev_name);
			min_alarm.ulg = (DevULong)min_alarm.db;
			break;
			
		case Tango::DEV_ULONG64:
			if (!(str >> min_alarm.db))
				throw_err_format("min_alarm",dev_name);
			min_alarm.ulg64 = (DevULong64)min_alarm.db;
			break;
			
		case Tango::DEV_ENCODED:
			if (!(str >> min_alarm.sh))
				throw_err_format("min_alarm",dev_name);
			min_alarm.uch = (unsigned char)min_alarm.sh;
			break;
		}
		alarm_conf.set(min_level);
	}
	
//
// Init the max alarm property
//

	max_alarm_str = get_attr_value(prop_list,"max_alarm");
	if ((max_alarm_str != AlrmValueNotSpec) &&
	    (data_type != Tango::DEV_STRING) &&
	    (data_type != Tango::DEV_BOOLEAN) &&
	    (data_type != Tango::DEV_STATE))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << max_alarm_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> max_alarm.sh))
				throw_err_format("max_alarm",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> max_alarm.db))
				throw_err_format("max_alarm",dev_name);
			max_alarm.lg = (DevLong)max_alarm.db;
			break;
			
		case Tango::DEV_LONG64:
			if (!(str >> max_alarm.db))
				throw_err_format("max_alarm",dev_name);
			max_alarm.lg64 = (DevLong64)max_alarm.db;
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> max_alarm.db))
				throw_err_format("max_alarm",dev_name);
			break;
			
		case Tango::DEV_FLOAT:
			if (!(str >> max_alarm.fl))
				throw_err_format("max_alarm",dev_name);
			break;
			
		case Tango::DEV_USHORT:
			if (!(str >> max_alarm.ush))
				throw_err_format("max_alarm",dev_name);
			break;
			
		case Tango::DEV_UCHAR:
			if (!(str >> max_alarm.sh))
				throw_err_format("max_alarm",dev_name);
			max_alarm.uch = (unsigned char)max_alarm.sh;
			break;
			
		case Tango::DEV_ULONG:
			if (!(str >> max_alarm.db))
				throw_err_format("max_alarm",dev_name);
			max_alarm.ulg = (DevULong)max_alarm.db;
			break;
			
		case Tango::DEV_ULONG64:
			if (!(str >> max_alarm.db))
				throw_err_format("max_alarm",dev_name);
			max_alarm.ulg64 = (DevULong64)max_alarm.db;
			break;
			
		case Tango::DEV_ENCODED:
			if (!(str >> max_alarm.sh))
				throw_err_format("max_alarm",dev_name);
			max_alarm.uch = (unsigned char)max_alarm.sh;
			break;
		}
		alarm_conf.set(max_level);
	}
		
//
// Init the min value property
//

	min_value_str = get_attr_value(prop_list,"min_value");
	if ((min_value_str != AlrmValueNotSpec) &&
	    (data_type != Tango::DEV_STRING) &&
	    (data_type != Tango::DEV_BOOLEAN) &&
	    (data_type != Tango::DEV_STATE))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << min_value_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> min_value.sh))
				throw_err_format("min_value",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> min_value.db))
				throw_err_format("min_value",dev_name);
			min_value.lg = (DevLong)min_value.db;
			break;
			
		case Tango::DEV_LONG64:
			if (!(str >> min_value.db))
				throw_err_format("min_value",dev_name);
			min_value.lg64 = (DevLong64)min_value.db;
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> min_value.db))
				throw_err_format("min_value",dev_name);
			break;
			
		case Tango::DEV_FLOAT:
			if (!(str >> min_value.fl))
				throw_err_format("min_value",dev_name);
			break;
			
		case Tango::DEV_USHORT:
			if (!(str >> min_value.ush))
				throw_err_format("min_value",dev_name);
			break;
			
		case Tango::DEV_UCHAR:
			if (!(str >> min_value.sh))
				throw_err_format("min_value",dev_name);
			min_value.uch = (unsigned char)min_value.sh;
			break;
			
		case Tango::DEV_ULONG:
			if (!(str >> min_value.db))
				throw_err_format("min_value",dev_name);
			min_value.ulg = (DevULong)min_value.db;
			break;
			
		case Tango::DEV_ULONG64:
			if (!(str >> min_value.db))
				throw_err_format("min_value",dev_name);
			min_value.ulg64 = (DevULong64)min_value.db;
			break;
			
		case Tango::DEV_ENCODED:
			if (!(str >> min_value.sh))
				throw_err_format("min_value",dev_name);
			min_value.uch = (unsigned char)min_value.sh;
			break;
		}
		check_min_value = true;
	}
		
//
// Init the max value property
//

	max_value_str = get_attr_value(prop_list,"max_value");
	if ((max_value_str != AlrmValueNotSpec) &&
	    (data_type != Tango::DEV_STRING) &&
	    (data_type != Tango::DEV_BOOLEAN) &&
	    (data_type != Tango::DEV_STATE))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << max_value_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> max_value.sh))
				throw_err_format("max_value",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> max_value.db))
				throw_err_format("max_value",dev_name);
			max_value.lg = (DevLong)max_value.db;
			break;
			
		case Tango::DEV_LONG64:
			if (!(str >> max_value.db))
				throw_err_format("max_value",dev_name);
			max_value.lg64 = (DevLong64)max_value.db;
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> max_value.db))
				throw_err_format("max_value",dev_name);
			break;
			
		case Tango::DEV_FLOAT:
			if (!(str >> max_value.fl))
				throw_err_format("max_value",dev_name);
			break;
			
		case Tango::DEV_USHORT:
			if (!(str >> max_value.ush))
				throw_err_format("max_value",dev_name);
			break;
			
		case Tango::DEV_UCHAR:
			if (!(str >> max_value.sh))
				throw_err_format("max_value",dev_name);
			max_value.uch = (unsigned char)max_value.sh;
			break;
			
		case Tango::DEV_ULONG:
			if (!(str >> max_value.db))
				throw_err_format("max_value",dev_name);
			max_value.ulg = (DevULong)max_value.db;
			break;
			
		case Tango::DEV_ULONG64:
			if (!(str >> max_value.db))
				throw_err_format("max_value",dev_name);
			max_value.ulg64 = (DevULong64)max_value.db;
			break;
			
		case Tango::DEV_ENCODED:
			if (!(str >> max_value.sh))
				throw_err_format("max_value",dev_name);
			max_value.uch = (unsigned char)max_value.sh;
			break;
		}
		check_max_value = true;
	}

//
// Init the min warning property
//

	min_warning_str = get_attr_value(prop_list,"min_warning");
	if ((min_warning_str != AlrmValueNotSpec) && 
	    (data_type != Tango::DEV_STRING) &&
	    (data_type != Tango::DEV_BOOLEAN) &&
	    (data_type != Tango::DEV_STATE))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}
		
		str << min_warning_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> min_warning.sh))
				throw_err_format("min_warning",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> min_warning.db))
				throw_err_format("min_warning",dev_name);
			min_warning.lg = (DevLong)min_warning.db;
			break;
			
		case Tango::DEV_LONG64:
			if (!(str >> min_warning.db))
				throw_err_format("min_warning",dev_name);
			min_warning.lg64 = (DevLong64)min_warning.db;
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> min_warning.db))
				throw_err_format("min_warning",dev_name);
			break;
		case Tango::DEV_FLOAT:
			if (!(str >> min_warning.fl))
				throw_err_format("min_warning",dev_name);
			break;
			
		case Tango::DEV_USHORT:
			if (!(str >> min_warning.ush))
				throw_err_format("min_warning",dev_name);
			break;
			
		case Tango::DEV_UCHAR:
			if (!(str >> min_warning.sh))
				throw_err_format("min_warning",dev_name);
			min_warning.uch = (unsigned char)min_warning.sh;
			break;
			
		case Tango::DEV_ULONG:
			if (!(str >> min_warning.db))
				throw_err_format("min_warning",dev_name);
			min_warning.ulg = (DevULong)min_warning.db;
			break;
			
		case Tango::DEV_ULONG64:
			if (!(str >> min_warning.db))
				throw_err_format("min_warning",dev_name);
			min_warning.ulg64 = (DevULong64)min_warning.db;
			break;
			
		case Tango::DEV_ENCODED:
			if (!(str >> min_warning.sh))
				throw_err_format("min_warning",dev_name);
			min_warning.uch = (unsigned char)min_warning.sh;
			break;
		}
		alarm_conf.set(min_warn);
	}
	
//
// Init the max warning property
//

	max_warning_str = get_attr_value(prop_list,"max_warning");
	if ((max_warning_str != AlrmValueNotSpec) &&
	    (data_type != Tango::DEV_STRING) &&
	    (data_type != Tango::DEV_BOOLEAN) &&
	    (data_type != Tango::DEV_STATE))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << max_warning_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> max_warning.sh))
				throw_err_format("max_warning",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> max_warning.db))
				throw_err_format("max_warning",dev_name);
			max_warning.lg = (DevLong)max_warning.db;
			break;
			
		case Tango::DEV_LONG64:
			if (!(str >> max_warning.db))
				throw_err_format("max_warning",dev_name);
			max_warning.lg64 = (DevLong64)max_warning.db;
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> max_warning.db))
				throw_err_format("max_warning",dev_name);
			break;
			
		case Tango::DEV_FLOAT:
			if (!(str >> max_warning.fl))
				throw_err_format("max_warning",dev_name);
			break;
			
		case Tango::DEV_USHORT:
			if (!(str >> max_warning.ush))
				throw_err_format("max_warning",dev_name);
			break;
			
		case Tango::DEV_UCHAR:
			if (!(str >> max_warning.sh))
				throw_err_format("max_warning",dev_name);
			max_warning.uch = (unsigned char)max_warning.sh;
			break;
			
		case Tango::DEV_ULONG:
			if (!(str >> max_warning.db))
				throw_err_format("max_warning",dev_name);
			max_warning.ulg = (DevULong)max_warning.db;
			break;
			
		case Tango::DEV_ULONG64:
			if (!(str >> max_warning.db))
				throw_err_format("max_warning",dev_name);
			max_warning.ulg64 = (DevULong64)max_warning.db;
			break;
			
		case Tango::DEV_ENCODED:
			if (!(str >> max_warning.sh))
				throw_err_format("max_warning",dev_name);
			max_warning.uch = (unsigned char)max_warning.sh;
			break;
		}
		alarm_conf.set(max_warn);
	}
		
//
// Get delta_t property
//

	delta_t_str = get_attr_value(prop_list,"delta_t");
	bool delta_t_defined = false;
	if ((data_type != Tango::DEV_STRING) && (data_type != Tango::DEV_BOOLEAN))
	{
		delta_t = get_lg_attr_value(prop_list,"delta_t");
		if (delta_t != 0)
			delta_t_defined = true;
	}
	else
		delta_t =  0;

//
// Get delta_val property
//

	delta_val_str = get_attr_value(prop_list,"delta_val");
	bool delta_val_defined = false;
	if (delta_t_defined == true)
	{
		if ((delta_val_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			if (empty == false)
			{
				str.seekp(0);
				str.seekg(0);
			}

			str << delta_val_str << ends;
			empty = false;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> delta_val.sh))
					throw_err_format("delta_val",dev_name);
				break;

			case Tango::DEV_LONG:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.lg = (DevLong)delta_val.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.lg64 = (DevLong64)delta_val.db;
				break;

			case Tango::DEV_DOUBLE:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				break;

			case Tango::DEV_FLOAT:
				if (!(str >> delta_val.fl))
					throw_err_format("delta_val",dev_name);
				break;

			case Tango::DEV_USHORT:
				if (!(str >> delta_val.ush))
					throw_err_format("delta_val",dev_name);
				break;

			case Tango::DEV_UCHAR:
				if (!(str >> delta_val.sh))
					throw_err_format("delta_val",dev_name);
				delta_val.uch = (unsigned char)delta_val.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.ulg = (DevULong)delta_val.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.ulg64 = (DevULong64)delta_val.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> delta_val.sh))
					throw_err_format("delta_val",dev_name);
				delta_val.uch = (unsigned char)delta_val.sh;
				break;
			}
			alarm_conf.set(rds);
			delta_val_defined = true;
		}	
	}

//
// Throw exception if only one the RDS property is defined
//

	if (((delta_t_defined == true) && (delta_val_defined == false)) ||
	    ((delta_t_defined == false) && (delta_val_defined == true)))
	{
		TangoSys_OMemStream o;
		
		o << "RDS alarm properties (delta_t and delta_val) are not correctly defined for attribute " << name;
		o << " for device " << dev_name << ends;
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
//
//--------------------------------------------------------------------------

void Attribute::throw_err_format(const char *prop_name,string &dev_name)
{
	TangoSys_OMemStream o;
			
	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nThe property " << prop_name << " is defined in a unsupported format" << ends;
	Except::throw_exception((const char *)"API_AttrOptProp",
			      o.str(),
			      (const char *)"Attribute::throw_err_format");
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
//
//--------------------------------------------------------------------------

void Attribute::throw_err_data_type(const char *prop_name,string &dev_name)
{
	TangoSys_OMemStream o;
			
	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nThe property " << prop_name << " is not settable for the attribute data type" << ends;
	Except::throw_exception((const char *)"API_AttrOptProp",
			      o.str(),
			      (const char *)"Attribute::throw_err_data_type");
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::is_polled
// 
// description : 	Check if the attribute polled
//
// This method returns a boolean set to true if the atribute is polled
//
//--------------------------------------------------------------------------

bool Attribute::is_polled()
{
	Tango::Util *tg = Util::instance();
	if ( ext->dev == NULL )
	{
		ext->dev = tg->get_device_by_name(ext->d_name);
	}

	string att_name(get_name());
	transform(att_name.begin(),att_name.end(),att_name.begin(),::tolower);
	
	vector<string> &attr_list = ext->dev->get_polled_attr();
	for (unsigned int i = 0;i < attr_list.size();i = i+2)
	{
		//	Convert to lower case before comparison
		string	name_lowercase(attr_list[i]);
		transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
		if ( att_name == name_lowercase )
		{		
			// when the polling buffer is externally filled (polling period == 0)
			// mark the attribute as not polled! No events can be send by the polling thread!
			
			if ( attr_list[i+1] == "0" )
			{
				return false;
			}
			else
				return true;
		}
	}
		
	// now check wether a pooling period is set ( for example by pogo)
	if ( get_polling_period() > 0 )
	{
		// check the list of non_auto_polled attributes to verify wether
		// the polling was disabled
			
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
		
	if (ext->event_period == INT_MAX)	
		conf.event_prop.per_event.period = CORBA::string_dup(AlrmValueNotSpec);	
	else
	{
		int per = (int)((double)ext->event_period);
		str << per;
		MEM_STREAM_2_CORBA(conf.event_prop.per_event.period,str);
	}

//
// Copy change event properties
//
	
	if (ext->rel_change[0] == INT_MAX)
		conf.event_prop.ch_event.rel_change = CORBA::string_dup(AlrmValueNotSpec);
	else
	{
		if (fabs(ext->rel_change[0]) == fabs(ext->rel_change[1]))
		{		
			str << ext->rel_change[1];
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.rel_change,str);
		}
		else
		{
			str << fabs(ext->rel_change[0]) << "," << ext->rel_change[1];			
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.rel_change,str);
		}
	}
	
	if (ext->abs_change[0] == INT_MAX)
		conf.event_prop.ch_event.abs_change = CORBA::string_dup(AlrmValueNotSpec);
	else
	{		
		if (fabs(ext->abs_change[0]) == fabs(ext->abs_change[1]))
		{
			str << ext->abs_change[1];
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.abs_change,str);
		}
		else
		{
			str << fabs(ext->abs_change[0]) << "," << ext->abs_change[1];			
			MEM_STREAM_2_CORBA(conf.event_prop.ch_event.abs_change,str);
		}
	}
	
//
// Copy archive event properties
//
	
	if (ext->archive_period == INT_MAX)
	{
		conf.event_prop.arch_event.period = CORBA::string_dup(AlrmValueNotSpec);
	}
	else
	{
		int per = (int)((double)ext->archive_period);
		str << per;		
		MEM_STREAM_2_CORBA(conf.event_prop.arch_event.period,str);
	}
		
	if (ext->archive_rel_change[0] == INT_MAX)
		conf.event_prop.arch_event.rel_change = CORBA::string_dup(AlrmValueNotSpec);
	else
	{		
		if (fabs(ext->archive_rel_change[0]) == fabs(ext->archive_rel_change[1]))
		{
			str << ext->archive_rel_change[1];
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.rel_change,str);
		}
		else
		{
			str << fabs(ext->archive_rel_change[0]) << "," << ext->archive_rel_change[1];			
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.rel_change,str);
		}
	}
	
	if (ext->archive_abs_change[0] == INT_MAX)
		conf.event_prop.arch_event.abs_change = CORBA::string_dup(AlrmValueNotSpec);
	else
	{		
		if (fabs(ext->archive_abs_change[0]) == fabs(ext->archive_abs_change[1]))
		{
			str << ext->archive_abs_change[1];
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.abs_change,str);
		}
		else
		{
			str << fabs(ext->archive_abs_change[0]) << "," << ext->archive_abs_change[1];			
			MEM_STREAM_2_CORBA(conf.event_prop.arch_event.abs_change,str);
		}
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
// Copy only a sub-set of the new properties
// For each "string" property, an empty string means returns to its
// default value which could be the library default value or the 
// user defined default value
//

	Tango::DeviceImpl *dev;
	if (ext->dev != NULL)
		dev = ext->dev;
	else
	{
		Tango::Util *tg = Tango::Util::instance();
		dev = tg->get_device_by_name(dev_name);
	}
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	long nb_user = def_user_prop.size();

//
// First the description
//
	
	if ((TG_strcasecmp(conf.description,AlrmValueNotSpec) != 0) &&
	    (TG_strcasecmp(conf.description,DescNotSet) != 0) &&
		(strlen(conf.description) != 0))
		description = conf.description;
	else
	{
		if (nb_user != 0)
		{
			int i;
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

//
// The label
//
		
	if ((TG_strcasecmp(conf.label,AlrmValueNotSpec) != 0) &&
	    (TG_strcasecmp(conf.label,name.c_str()) != 0) &&
		(strlen(conf.label) != 0))
		label = conf.label;
	else
	{
		if (nb_user != 0)
		{
			int i;
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

//
// The unit
//
		
	if ((TG_strcasecmp(conf.unit,AlrmValueNotSpec) != 0) &&
	    (TG_strcasecmp(conf.unit,UnitNotSpec) != 0) &&
		(strlen(conf.unit) != 0))
		unit = conf.unit;
	else
	{
		if (nb_user != 0)
		{
			int i;
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

//
// The standard unit
//
	
	if ((TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) != 0) &&
	    (TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) != 0) &&
		(strlen(conf.standard_unit) != 0))	
		standard_unit = conf.standard_unit;
	else
	{
		if (nb_user != 0)
		{
			int i;
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

//
// The display unit
//
	
	if ((TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) != 0) &&
	    (TG_strcasecmp(conf.display_unit,DispUnitNotSpec) != 0) &&
		(strlen(conf.display_unit) != 0))
		display_unit = conf.display_unit;
	else
	{
		if (nb_user != 0)
		{
			int i;
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

//
// The format
//
	
	if ((TG_strcasecmp(conf.format,AlrmValueNotSpec) != 0) &&
	    (TG_strcasecmp(conf.format,FormatNotSpec) != 0) &&
		(strlen(conf.format) != 0))
		format = conf.format;
	else
	{
		if (nb_user != 0)
		{
			int i;
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

//
// For the last four properties, convert their value to the right type
//

	TangoSys_MemStream str;
	min_value_str = conf.min_value;
	if (min_value_str == NotANumber)
	{
		min_value_str = AlrmValueNotSpec;
		check_min_value = false;
	}
	else
	{
		if ((min_value_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str << min_value_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_value.sh))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> min_value.db))
					throw_err_format("min_value",dev_name);
				min_value.lg = (DevLong)min_value.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> min_value.db))
					throw_err_format("min_value",dev_name);
				min_value.lg64 = (DevLong64)min_value.db;
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> min_value.db))
					throw_err_format("min_value",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> min_value.fl))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> min_value.ush))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> min_value.sh))
					throw_err_format("min_value",dev_name);
				min_value.uch = (unsigned char)min_value.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> min_value.db))
					throw_err_format("min_value",dev_name);
				min_value.ulg = (DevULong)min_value.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> min_value.db))
					throw_err_format("min_value",dev_name);
				min_value.ulg64 = (DevULong64)min_value.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> min_value.sh))
					throw_err_format("min_value",dev_name);
				min_value.uch = (unsigned char)min_value.sh;
				break;
			}
			check_min_value = true;
		}
		else
			check_min_value = false;
	}

	max_value_str = conf.max_value;
	if (max_value_str == NotANumber)
	{
		max_value_str = AlrmValueNotSpec;
		check_max_value = false;
	}
	else
	{	
		if ((max_value_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << max_value_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_value.sh))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> max_value.db))
					throw_err_format("max_value",dev_name);
				max_value.lg = (DevLong)max_value.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> max_value.db))
					throw_err_format("max_value",dev_name);
				max_value.lg64 = (DevLong64)max_value.db;
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> max_value.db))
					throw_err_format("max_value",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> max_value.fl))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> max_value.ush))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> max_value.sh))
					throw_err_format("max_value",dev_name);
				max_value.uch = (unsigned char)max_value.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> max_value.db))
					throw_err_format("max_value",dev_name);
				max_value.ulg = (DevULong)max_value.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> max_value.db))
					throw_err_format("max_value",dev_name);
				max_value.ulg64 = (DevULong64)max_value.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> max_value.sh))
					throw_err_format("max_value",dev_name);
				max_value.uch = (unsigned char)max_value.sh;
				break;
			}
			check_max_value = true;
		}
		else
			check_max_value = false;
	}

	min_alarm_str = conf.min_alarm;
	if (min_alarm_str == NotANumber)
	{
		min_alarm_str = AlrmValueNotSpec;
		alarm_conf.reset(min_level);
	}
	else
	{		
		if ((min_alarm_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << min_alarm_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_alarm.sh))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> min_alarm.db))
					throw_err_format("min_alarm",dev_name);
				min_alarm.lg = (DevLong)min_alarm.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> min_alarm.db))
					throw_err_format("min_alarm",dev_name);
				min_alarm.lg64 = (DevLong64)min_alarm.db;
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> min_alarm.db))
					throw_err_format("min_alarm",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> min_alarm.fl))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> min_alarm.ush))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> min_alarm.sh))
					throw_err_format("min_alarm",dev_name);
				min_alarm.uch = (unsigned char)min_alarm.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> min_alarm.db))
					throw_err_format("min_alarm",dev_name);
				min_alarm.ulg = (DevULong)min_alarm.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> min_alarm.db))
					throw_err_format("min_alarm",dev_name);
				min_alarm.ulg64 = (DevULong64)min_alarm.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> min_alarm.sh))
					throw_err_format("min_alarm",dev_name);
				min_alarm.uch = (unsigned char)min_alarm.sh;
				break;
			}
			alarm_conf.set(min_level);
		}
		else
		{
			alarm_conf.reset(min_level);
		}
	}

	max_alarm_str = conf.max_alarm;
	if (max_alarm_str == NotANumber)
	{
		max_alarm_str = AlrmValueNotSpec;
		alarm_conf.reset(max_level);
	}
	else
	{		
		if ((max_alarm_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << max_alarm_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_alarm.sh))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> max_alarm.db))
					throw_err_format("max_alarm",dev_name);
				max_alarm.lg = (DevLong)max_alarm.db;
				break;

			case Tango::DEV_LONG64:
				if (!(str >> max_alarm.db))
					throw_err_format("max_alarm",dev_name);
				max_alarm.lg64 = (DevLong64)max_alarm.db;
				break;
							
			case Tango::DEV_DOUBLE:
				if (!(str >> max_alarm.db))
					throw_err_format("max_alarm",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> max_alarm.fl))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> max_alarm.ush))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> max_alarm.sh))
					throw_err_format("max_alarm",dev_name);
				max_alarm.uch = (unsigned char)max_alarm.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> max_alarm.db))
					throw_err_format("max_alarm",dev_name);
				max_alarm.ulg = (DevULong)max_alarm.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> max_alarm.db))
					throw_err_format("max_alarm",dev_name);
				max_alarm.ulg64 = (DevULong64)max_alarm.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> max_alarm.sh))
					throw_err_format("max_alarm",dev_name);
				max_alarm.uch = (unsigned char)max_alarm.sh;
				break;
			}
			alarm_conf.set(max_level);
		}
		else
		{
			alarm_conf.reset(max_level);
		}
	}
		
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
// The min_warning case
//

	TangoSys_MemStream str;
	
	min_warning_str = conf.att_alarm.min_warning;
	if (min_warning_str == NotANumber)
	{
		min_warning_str = AlrmValueNotSpec;
		alarm_conf.reset(min_warn);
	}
	else
	{		
		if ((min_warning_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str << min_warning_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_warning.sh))
					throw_err_format("min_",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> min_warning.db))
					throw_err_format("min_warning",dev_name);
				min_warning.lg = (DevLong)min_warning.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> min_warning.db))
					throw_err_format("min_warning",dev_name);
				min_warning.lg64 = (DevLong64)min_warning.db;
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> min_warning.db))
					throw_err_format("min_warning",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> min_warning.fl))
					throw_err_format("min_warning",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> min_warning.ush))
					throw_err_format("min_warning",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> min_warning.sh))
					throw_err_format("min_warning",dev_name);
				min_warning.uch = (unsigned char)min_warning.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> min_warning.db))
					throw_err_format("min_warning",dev_name);
				min_warning.ulg = (DevULong)min_warning.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> min_warning.db))
					throw_err_format("min_warning",dev_name);
				min_warning.ulg64 = (DevULong64)min_warning.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> min_warning.sh))
					throw_err_format("min_warning",dev_name);
				min_warning.uch = (unsigned char)min_warning.sh;
				break;
			}
			alarm_conf.set(min_warn);
		}
		else
		{
			alarm_conf.reset(min_warn);
		}
	}

//
// Now, ladies and gentleman, the max warning case
//

	max_warning_str = conf.att_alarm.max_warning;
	if (max_warning_str == NotANumber)
	{
		max_warning_str = AlrmValueNotSpec;
		alarm_conf.reset(max_warn);
	}
	else
	{		
		if ((max_warning_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << max_warning_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_warning.sh))
					throw_err_format("max_warning",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> max_warning.db))
					throw_err_format("max_warning",dev_name);
				max_warning.lg = (DevLong)max_warning.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> max_warning.db))
					throw_err_format("max_warning",dev_name);
				max_warning.lg64 = (DevLong64)max_warning.db;
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> max_warning.db))
					throw_err_format("max_warning",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> max_warning.fl))
					throw_err_format("max_warning",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> max_warning.ush))
					throw_err_format("max_warning",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> max_warning.sh))
					throw_err_format("max_warning",dev_name);
				max_warning.uch = (unsigned char)max_warning.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> max_warning.db))
					throw_err_format("max_warning",dev_name);
				max_warning.ulg = (DevULong)max_warning.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> max_warning.db))
					throw_err_format("max_warning",dev_name);
				max_warning.ulg64 = (DevULong64)max_warning.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> max_warning.sh))
					throw_err_format("max_warning",dev_name);
				max_warning.uch = (unsigned char)max_warning.sh;
				break;
			}
			alarm_conf.set(max_warn);
		}
		else
		{
			alarm_conf.reset(max_warn);
		}
	}
	
//
// Then, the delta_val
//

	delta_val_str = conf.att_alarm.delta_val;
	if (delta_val_str == NotANumber)
	{
		delta_val_str = AlrmValueNotSpec;
		alarm_conf.reset(rds);
	}
	else
	{		
		if ((delta_val_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << delta_val_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> delta_val.sh))
					throw_err_format("delta_val",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.lg = (DevLong)delta_val.db;
				break;
				
			case Tango::DEV_LONG64:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.lg64 = (DevLong64)delta_val.db;
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				break;
				
			case Tango::DEV_FLOAT:
				if (!(str >> delta_val.fl))
					throw_err_format("delta_val",dev_name);
				break;
			
			case Tango::DEV_USHORT:
				if (!(str >> delta_val.ush))
					throw_err_format("delta_val",dev_name);
				break;
			
			case Tango::DEV_UCHAR:
				if (!(str >> delta_val.sh))
					throw_err_format("delta_val",dev_name);
				delta_val.uch = (unsigned char)delta_val.sh;
				break;
				
			case Tango::DEV_ULONG:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.ulg = (DevULong)delta_val.db;
				break;
				
			case Tango::DEV_ULONG64:
				if (!(str >> delta_val.db))
					throw_err_format("delta_val",dev_name);
				delta_val.ulg64 = (DevULong64)delta_val.db;
				break;
				
			case Tango::DEV_ENCODED:
				if (!(str >> delta_val.sh))
					throw_err_format("delta_val",dev_name);
				delta_val.uch = (unsigned char)delta_val.sh;
				break;
			}
			alarm_conf.set(rds);
		}
		else
		{
			alarm_conf.reset(rds);
		}
	}
	
//
// And the delta_t
//

	delta_t_str = conf.att_alarm.delta_t;
	if (delta_t_str == NotANumber)
	{
		delta_t_str = AlrmValueNotSpec;
		alarm_conf.reset(rds);
	}
	else
	{		
		if ((delta_t_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN) &&
		    (data_type != Tango::DEV_STATE))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << delta_t_str << ends;
			if (!(str >> delta_t))
				throw_err_format("delta_t",dev_name);
			alarm_conf.set(rds);
		}
		else
		{
			alarm_conf.reset(rds);
		}
	}



//
// Now, the 4 changes parameters (for change and archive events)
//

	SET_EV_PROP(conf.event_prop.ch_event.rel_change,str,rel_change);
	SET_EV_PROP(conf.event_prop.ch_event.abs_change,str,abs_change);
	SET_EV_PROP(conf.event_prop.arch_event.rel_change,str,archive_rel_change);
	SET_EV_PROP(conf.event_prop.arch_event.abs_change,str,archive_abs_change);

//
// And finally, the last two event periods
//

	if (strcmp(conf.event_prop.per_event.period,NotANumber) == 0)
	{
		ext->event_period = DEFAULT_EVENT_PERIOD;
//		ext->event_period = INT_MAX;
	}
	else
	{		
		if (strcmp(conf.event_prop.per_event.period,AlrmValueNotSpec) != 0)
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << conf.event_prop.per_event.period << ends;
			int tmp_period;
			if (!(str >> tmp_period))
				throw_err_format("event_period",dev_name);

			ext->event_period = tmp_period;
		}
		else
		{
			ext->event_period = DEFAULT_EVENT_PERIOD;
//			ext->event_period = INT_MAX;
		}
	}
	
	if (strcmp(conf.event_prop.arch_event.period,NotANumber) == 0)
	{
		ext->archive_period = INT_MAX;
	}
	else
	{		
		if (strcmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) != 0)
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();

			str << conf.event_prop.arch_event.period << ends;
			int tmp_period;
			if (!(str >> tmp_period))
				throw_err_format("archive_period",dev_name);

			ext->archive_period = tmp_period;
		}
		else
		{
			ext->archive_period = INT_MAX;
		}
	}		
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
// Check if "string" properties must be updated in db and if
// needed insert a corresponding DbDatum object in the DbData
// vector
//

	check_str_prop(conf,db_d,prop_to_update,db_del,prop_to_delete);

//
// For the last four, if the data type is not string, checks that the input
// strings are really number
//

	TangoSys_MemStream str;

	CHECK_PROP(conf.min_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_value");		

	CHECK_PROP(conf.max_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_value");
		   
	CHECK_PROP(conf.min_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_alarm");
		   
	CHECK_PROP(conf.max_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_alarm");
										
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
	
	check_str_prop(tmp_conf,db_d,prop_to_update,db_del,prop_to_delete);

//
// For the last two, if the data type is not string, checks that the input
// strings are really number
//

	TangoSys_MemStream str;

	CHECK_PROP(conf.min_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_value");		

	CHECK_PROP(conf.max_value,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_value");

//
// Check for alarm related data
//
		   
	CHECK_PROP(conf.att_alarm.min_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_alarm");
		   
	CHECK_PROP(conf.att_alarm.max_alarm,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_alarm");

	CHECK_PROP(conf.att_alarm.min_warning,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"min_warning");
		   
	CHECK_PROP(conf.att_alarm.max_warning,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"max_warning");
			
	CHECK_PROP(conf.att_alarm.delta_val,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"delta_val");

	if ((strcmp(conf.att_alarm.delta_t,AlrmValueNotSpec) != 0) && 
	    (strcmp(conf.att_alarm.delta_t,NotANumber) != 0))
	{
		if ((data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN))
		{
			str.seekp(0);
			str.seekg(0);
			str.clear();
			str << conf.att_alarm.delta_t << ends;
			int lg;
			if (!(str >> lg))
				throw_err_format("delta_t",dev_name);
			const char *tmp = conf.att_alarm.delta_t.in();
			for (unsigned int loop = 0;loop < strlen(tmp);loop++)
			{
				if (isdigit(tmp[loop]) == 0)
					throw_err_format("delta_t",dev_name);
			} 
			if ((delta_val_str == AlrmValueNotSpec) &&
				(strcmp(conf.att_alarm.delta_val.in(),AlrmValueNotSpec) == 0))
			{
				Tango::Except::throw_exception((const char *)"API_AttrOptProp",
						(const char *)"It is not allowed to set the delta_t property if the delta_val property is not set",
						(const char *)"Attribute::upd_database");
			}
			DbDatum max_val("delta_t");
			max_val << tmp;
			db_d.push_back(max_val);
			prop_to_update++;
		}
	}

	if ((strcmp(conf.att_alarm.delta_t,NotANumber) == 0) || (TG_strcasecmp(conf.att_alarm.delta_t,AlrmValueNotSpec) == 0))
	{
		DbDatum max_val("delta_t");
		db_del.push_back(max_val);
		prop_to_delete++;
	}
	 			   
//
// Check for event related data
//

	CHECK_CH_PROP(conf.event_prop.ch_event.rel_change,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"rel_change");
		   
	CHECK_CH_PROP(conf.event_prop.ch_event.abs_change,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"abs_change");
		   
	CHECK_CH_PROP(conf.event_prop.arch_event.rel_change,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"archive_rel_change");
		   
	CHECK_CH_PROP(conf.event_prop.arch_event.abs_change,str,dev_name,db_d,db_del,
	           prop_to_update,prop_to_delete,"archive_abs_change");
		   
		   
	if ((strcmp(conf.event_prop.per_event.period,AlrmValueNotSpec) != 0) && 
	    (strcmp(conf.event_prop.per_event.period,NotANumber) != 0))
	{
		str.seekp(0);
		str.seekg(0);
		str.clear();
		str << conf.event_prop.per_event.period << ends;
		int lg;
		if (!(str >> lg))
			throw_err_format("event_period",dev_name);
		DbDatum max_val("event_period");
		const char *tmp = conf.event_prop.per_event.period.in();
		max_val << tmp;
		db_d.push_back(max_val);
		prop_to_update++;
	}

	if ((strcmp(conf.event_prop.per_event.period,NotANumber) == 0) || (TG_strcasecmp(conf.event_prop.per_event.period,AlrmValueNotSpec) == 0))
	{
		DbDatum max_val("event_period");
		db_del.push_back(max_val);
		prop_to_delete++;
	}

	if ((strcmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) != 0) && 
	    (strcmp(conf.event_prop.arch_event.period,NotANumber) != 0))
	{
		str.seekp(0);
		str.seekg(0);
		str.clear();
		str << conf.event_prop.arch_event.period << ends;
		int lg;
		if (!(str >> lg))
			throw_err_format("archive_period",dev_name);
		DbDatum max_val("archive_period");
		const char *tmp = conf.event_prop.arch_event.period.in();
		max_val << tmp;
		db_d.push_back(max_val);
		prop_to_update++;
	}

	if ((strcmp(conf.event_prop.arch_event.period,NotANumber) == 0) || (TG_strcasecmp(conf.event_prop.arch_event.period,AlrmValueNotSpec) == 0))
	{
		DbDatum max_val("archive_period");
		db_del.push_back(max_val);
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
			       Tango::DbData &db_del,long &prop_to_delete)
{

//
// Check if each attribute property is not the default value. Only properties
// not having the default value will be sent to the database
//
	
	if ((TG_strcasecmp(conf.description,DescNotSpec) != 0) &&
	    (TG_strcasecmp(conf.description,AlrmValueNotSpec) != 0))
	{
		DbDatum desc("description");
		const char *tmp = conf.description.in();
		desc << tmp;
		db_d.push_back(desc);
		prop_to_update++;
	}
	
	if ((strlen(conf.description) == 0) || 
	    (TG_strcasecmp(conf.description,AlrmValueNotSpec) == 0) ||
		(TG_strcasecmp(conf.description,DescNotSpec) == 0))
	{
		DbDatum del_desc("description");
		db_del.push_back(del_desc);
		prop_to_delete++;
	}

//
// The label case
//
	
	if ((TG_strcasecmp(conf.label,name.c_str()) != 0) &&
	    (TG_strcasecmp(conf.label,AlrmValueNotSpec) != 0))
	{
		DbDatum lab("label");
		const char *tmp = conf.label.in();
		lab << tmp;
		db_d.push_back(lab);
		prop_to_update++;
	}
	
	if ((strlen(conf.label) == 0) || 
	    (TG_strcasecmp(conf.label,AlrmValueNotSpec) == 0) ||
		(TG_strcasecmp(conf.label,name.c_str()) == 0))
	{
		DbDatum del_lab("label");
		db_del.push_back(del_lab);
		prop_to_delete++;
	}

//
// The unit case
//
	
	if ((TG_strcasecmp(conf.unit,UnitNotSpec) != 0) &&
	    (TG_strcasecmp(conf.unit,AlrmValueNotSpec) != 0))
	{
		DbDatum uni("unit");
		const char *tmp = conf.unit.in();
		uni << tmp;
		db_d.push_back(uni);
		prop_to_update++;
	}
	
	if ((strlen(conf.unit) == 0) || 
	    (TG_strcasecmp(conf.unit,AlrmValueNotSpec) == 0) ||
		(TG_strcasecmp(conf.unit,UnitNotSpec) == 0))
	{
		DbDatum del_unit("unit");
		db_del.push_back(del_unit);
		prop_to_delete++;
	}

//
// The standard unit
//
	
	if ((TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) != 0) &&
	    (TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) != 0))
	{
		DbDatum std_uni("standard_unit");
		const char *tmp = conf.standard_unit.in();
		std_uni << tmp;
		db_d.push_back(std_uni);
		prop_to_update++;
	}

	if ((strlen(conf.standard_unit) == 0) || 
	    (TG_strcasecmp(conf.standard_unit,AlrmValueNotSpec) == 0) ||
		(TG_strcasecmp(conf.standard_unit,StdUnitNotSpec) == 0))
	{
		DbDatum del_std_unit("standard_unit");
		db_del.push_back(del_std_unit);
		prop_to_delete++;
	}

//
// The display unit
//
	
	if ((TG_strcasecmp(conf.display_unit,DispUnitNotSpec) != 0) &&
	    (TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) != 0))
	{
		DbDatum disp_uni("display_unit");
		const char *tmp = conf.display_unit.in();
		disp_uni << tmp;
		db_d.push_back(disp_uni);
		prop_to_update++;
	}

	if ((strlen(conf.display_unit) == 0) || 
	    (TG_strcasecmp(conf.display_unit,AlrmValueNotSpec) == 0) ||
		(TG_strcasecmp(conf.display_unit,DispUnitNotSpec) == 0))
	{
		DbDatum del_disp_unit("display_unit");
		db_del.push_back(del_disp_unit);
		prop_to_delete++;
	}

//
// The format
//
	
	if ((TG_strcasecmp(conf.format,FormatNotSpec) != 0) &&
	    (TG_strcasecmp(conf.format,AlrmValueNotSpec) != 0))
	{
		DbDatum form("format");
		const char *tmp = conf.format.in();
		form << tmp;
		db_d.push_back(form);
		prop_to_update++;
	}

	if ((strlen(conf.format) == 0) || 
	    (TG_strcasecmp(conf.format,AlrmValueNotSpec) == 0) ||
		(TG_strcasecmp(conf.format,FormatNotSpec) == 0))
	{
		DbDatum del_for("format");
		db_del.push_back(del_for);
		prop_to_delete++;
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
		return returned;
	}

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


void Attribute::AttributeValue_4_2_AttributeValue_3(Tango::AttributeValue_4 *ptr_4,Tango::AttributeValue_3 *ptr_3)
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

		EventSupplier *event_supplier;
		Tango::Util *tg = Util::instance();
		event_supplier = tg->get_event_supplier();
		if (event_supplier == NULL)
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
		
// don`t try to access the attribute data when an exception was indicated

		if ( except == NULL )
		{	
			if (send_attr != NULL)			
				Attribute_2_AttributeValue(send_attr,ext->dev);
			else
				Attribute_2_AttributeValue(send_attr_4,ext->dev);
		}

// 
// Fire event
//

		if ( is_check_change_criteria() == true )
		{
			event_supplier->detect_and_push_change_event_3(ext->dev,
						 		send_attr,
						 		send_attr_4,
								*this,
								name,
								except);
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

			event_supplier->push_event_3(ext->dev,
						"change",
						filterable_names,
						filterable_data,
						filterable_names_lg,
						filterable_data_lg,
						send_attr,
						send_attr_4,
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
		
// delete the data values allocated in the attribute

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
		//string &name_lower = get_name_lower();

		now = time(NULL);
		archive_subscription = (int)now - ext->event_archive_subscription;

		if (archive_subscription > EVENT_RESUBSCRIBE_PERIOD)
			{
			if ( (name_lower != "state") && (name_lower != "status") )
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
	// Get the event supplier, and simply return if not created
	//

		EventSupplier *event_supplier;
		Tango::Util *tg = Util::instance();
		event_supplier = tg->get_event_supplier();
		if (event_supplier == NULL)
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

// don`t try to access the attribute data when an exception was indicated

		if ( except == NULL )
		{	
			if (send_attr != NULL)			
				Attribute_2_AttributeValue(send_attr,ext->dev);
			else
				Attribute_2_AttributeValue(send_attr_4,ext->dev);
		}

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

			event_supplier->detect_and_push_archive_event_3(ext->dev,
						 			send_attr,
						 			send_attr_4,
									*this,
									name,
									except,
									&now_timeval);
		}
		else
		{

// Execute detect_change only to calculate the delta_change_rel and
// delta_change_abs and force_change !
//

			bool force_change   = false;
			bool quality_change = false;
			double delta_change_rel = 0.0;
			double delta_change_abs = 0.0;
			
			event_supplier->detect_change_3 (*this, send_attr, send_attr_4,true,
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

			event_supplier->push_event_3(ext->dev,
							"archive",
							filterable_names,
							filterable_data,
							filterable_names_lg,
							filterable_data_lg,
							send_attr,
							send_attr_4,
							name,
							except);	
		}

		if (send_attr != NULL)
			delete send_attr;
		else
			delete send_attr_4;
		
		// delete the data values allocated in the attribute
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
		
		// delete the data values allocated in the attribute
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

		EventSupplier *event_supplier;
		Tango::Util *tg = Util::instance();
		event_supplier = tg->get_event_supplier();
		if (event_supplier == NULL)
		{
			if (name_lower != "state")
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
// Fire event
//

		vector<string> filterable_names_lg;
		vector<long> filterable_data_lg;

		event_supplier->push_event_3(ext->dev,
					   "user_event",
					   filt_names,
					   filt_vals,
					   filterable_names_lg,
					   filterable_data_lg,
					   send_attr,
					   send_attr_4,
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
		
	prop << new_value.db;
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
// 					This method can be used to clean-up all the configuration of an attribute to come back to
// 					its default values or the remove all configuration of a dynamic attribute before deleting it.
//
// 					The method removes all configured attribute properties and removes the attribute from the 
// 					list of polled attributes.
//--------------------------------------------------------------------------

void Attribute::remove_configuration()
{
	cout4 << "Entering remove_configuration() method for attribute " << name << endl;

	Tango::Util *tg = Tango::Util::instance();
	
// read all configured properties of the attribute from the database and
// delete them!

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


    long nb_prop;
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

	if (conf.writable == static_cast<unsigned char>(true))
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
