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
// $Revision$
//
// $Log$
// Revision 3.17  2005/11/09 09:43:57  jensmeyer
// Stopped automatic polling when subscribing for events!
// Added exceptions when polling is not started for an attribute.
//
// Changed the default of the archiving period from 10 seconds to
// no periodic event!
// Added exception when trying to subscribe a not configured attribute.
//
// Revision 3.16  2005/10/03 15:56:50  jensmeyer
// Corrected archive periode reading during a restart.
//
// Revision 3.15  2005/07/28 07:34:28  taurel
// - Fix some incompatibility between 5.1 and 5.2
//
// Revision 3.14  2005/06/29 08:30:51  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.13  2005/05/25 06:05:40  taurel
// - Again some minors changes before 5.2
//
// Revision 3.12  2005/05/20 15:17:13  taurel
// - Some changes to support gcc on Solaris
// - Fix bug in fire_event_quality()
//
// Revision 3.11  2005/03/04 10:00:28  taurel
// - End of changes (all attribute type supported) for quality change event generation
//
// Revision 3.10  2005/03/03 15:36:15  taurel
// - Change in attribute quality factor change event. It is now fired by the Attribute
// set_quailty() and set_value_date_quality() methods. For scannable device.
//
// Revision 3.9  2005/01/13 09:27:52  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 3.7.2.7  2004/11/15 12:32:43  taurel
// - Some changes for omniORB 4.0.5 (Don't use USE_stub_in_nt_dll...)
//
// Revision 3.7.2.6  2004/10/27 05:59:45  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.7.2.5  2004/10/22 11:25:00  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.7.2.4  2004/09/27 09:09:05  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.7.2.3  2004/09/15 06:45:44  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.7.2.2  2004/08/19 07:44:03  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.7.2.1  2004/07/15 15:04:05  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.7  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.6  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.5  2003/08/22 12:52:54  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.4  2003/08/21 09:10:17  taurel
// - Fix a small bug in the data init. of the writable attributes
//
// Revision 3.3  2003/08/21 07:23:46  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.2.2.11  2004/05/24 14:31:46  taurel
// - Fix bug when setting attribute property min_alarm, max_alarm or max_value
// - Fix bug when reading attribute from CACHE if attribute quality factor is set to
//   ATTR_INVALID
//
// Revision 3.2.2.10  2004/05/19 09:22:33  taurel
// - Add Attribute quality factor default initialisation at object construction
//
// Revision 3.2.2.9  2004/04/02 14:58:16  taurel
// Changes for release 4.1
// - Change the event.h inclusion place in tango.h
// - Fix bugs in event.cpp file and add a clean way to shutdown event system
// - Now support attribute min,max,min_alarm and max_alarm defined in scientific notation for long attribute
// - Added debian30 support in Make.rules
//
// Revision 3.2.2.8  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.2.2.7  2004/03/02 07:41:56  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.2.2.6  2004/02/25 16:27:44  taurel
// Minor changes to compile library using Solaris CC compiler
//
// Revision 3.2.2.5  2004/02/18 15:06:17  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 3.2.2.4  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.2.2.3  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.2.2.2  2003/12/10 16:08:56  taurel
// Last commit before merging with the event branch.
// Revision 3.0.2.4  2003/11/21 11:30:21  andy_gotz
// changed string references in subscribe_event() calls to const's. Added
// filter parameter but not interpreting it yet. Made event type an
// enum. Fixed problem with resubscription. Minor memory leak in clients
// still persists.
//
// Revision 3.0.2.3  2003/11/16 22:10:43  andy_gotz
// New version which defines 4 types of events - change, quality, periodic and
// archive. Code has been factorised to reduce redundancy. Minimum and maximum
// changes are supported. Event period is taken into account. Relative and
// absolute changes are detected. Whole sequence is taken into account when
// determining change.
//
// Revision 3.0.2.2  2003/07/17 03:46:46  andy_gotz
// Added new events "on_valid" and "on_state_change"; factorized EventSupplier::detect_and_push_events() method;
// Catching more exceptions when creating EventSupplier, device server will start without Notification service;
// Added AttributeProxy and DbAttribute classes to api; created test programs in test directory
//
// Revision 3.2.2.1  2003/09/30 11:49:56  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.2  2003/05/28 14:55:06  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.1  2003/05/16 08:46:15  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
// Revision 3.0.2.1  2003/04/08 13:12:44  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:41:51  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
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

#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */
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

	switch(data_format)
	{
	case Tango::SPECTRUM:
		max_x = dynamic_cast<SpectrumAttr &>(tmp_attr).get_max_x();
		max_y = 0;
		dim_y = 0;
		break;
				
	case Tango::IMAGE:
		max_x = dynamic_cast<ImageAttr &>(tmp_attr).get_max_x();
		max_y = dynamic_cast<ImageAttr &>(tmp_attr).get_max_y();
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
	change_event_implmented = false;
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

	ext->event_period = (int)(DEFAULT_EVENT_PERIOD * DELTA_PERIODIC);  	// default for event period is 1 second

	try
	{
        	string event_period_str;
        	event_period_str = get_attr_value(prop_list,"event_period");

        	TangoSys_MemStream str;
        	if (event_period_str != AlrmValueNotSpec)
        	{
			long event_period=0;
                	str << event_period_str << ends;
                	str >> event_period;
			if (event_period > 0)
			{
#ifdef WIN32
				double tmp = (double)event_period * DELTA_PERIODIC;
				double int_part,eve_round;
				double frac = modf(tmp,&int_part);
				if (frac >= 0.5)
					eve_round = ceil(tmp);
				else
					eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
				double eve_round = rint((double)event_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
				double eve_round = rint((double)event_period * DELTA_PERIODIC);
		#else
				double eve_round = round((double)event_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
				ext->event_period = (int)eve_round;
			}
			cout1 << "Attribute::Attribute(): event_period_str " << event_period_str << " event_period = " << ext->event_period << endl;
        	}                                 
	}
	catch (...) { }

//
// Init period for archive event
//

	//ext->archive_period = 10 * (int)(DEFAULT_EVENT_PERIOD * DELTA_PERIODIC);  
	ext->archive_period = (int)(INT_MAX * DELTA_PERIODIC);

	try
	{
        	string archive_period_str;
        	archive_period_str = get_attr_value(prop_list,"archive_period");
			
        	TangoSys_MemStream str;
        	if (archive_period_str != AlrmValueNotSpec)
        	{
				long arch_period=0;
         	str << archive_period_str << ends;
				str >> arch_period;
				if (arch_period > 0)
				{
					//ext->archive_period = arch_period;
#ifdef WIN32
				double tmp = (double)arch_period * DELTA_PERIODIC;
				double int_part,eve_round;
				double frac = modf(tmp,&int_part);
				if (frac >= 0.5)
					eve_round = ceil(tmp);
				else
					eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
				double eve_round = rint((double)arch_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
				double eve_round = rint((double)arch_period * DELTA_PERIODIC);
		#else
				double eve_round = round((double)arch_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
				ext->archive_period = (int)eve_round;
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
	    (data_type != Tango::DEV_BOOLEAN))
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
			min_alarm.lg = (long)min_alarm.db;
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
			if (!(str >> min_alarm.uch))
				throw_err_format("min_alarm",dev_name);
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
	    (data_type != Tango::DEV_BOOLEAN))
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
			max_alarm.lg = (long)max_alarm.db;
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
			if (!(str >> max_alarm.uch))
				throw_err_format("max_alarm",dev_name);
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
	    (data_type != Tango::DEV_BOOLEAN))
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
			min_value.lg = (long)min_value.db;
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
			if (!(str >> min_value.uch))
				throw_err_format("min_value",dev_name);
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
	    (data_type != Tango::DEV_BOOLEAN))
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
			max_value.lg = (long)max_value.db;
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
			if (!(str >> max_value.uch))
				throw_err_format("max_value",dev_name);
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
	    (data_type != Tango::DEV_BOOLEAN))
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
			min_warning.lg = (long)min_warning.db;
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
			if (!(str >> min_warning.uch))
				throw_err_format("min_warning",dev_name);
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
	    (data_type != Tango::DEV_BOOLEAN))
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
			max_alarm.lg = (long)max_warning.db;
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
			if (!(str >> max_warning.uch))
				throw_err_format("max_warning",dev_name);
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

//
// Get delta_val property
//

	delta_val_str = get_attr_value(prop_list,"delta_val");
	bool delta_val_defined = false;
	if (delta_t_defined == true)
	{
		if ((delta_val_str != AlrmValueNotSpec) &&
		    (data_type != Tango::DEV_STRING) &&
		    (data_type != Tango::DEV_BOOLEAN))
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
				delta_val.lg = (long)delta_val.db;
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
				if (!(str >> delta_val.uch))
					throw_err_format("delta_val",dev_name);
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
	for (int i = 0;i < attr_list.size();i = i+2)
		{	
		if ( att_name == attr_list[i] )
			{	
			//cout << "Event OK : "<<ext->d_name <<"/"<<get_name()<< endl;		
			return true;
			}
		}
		
	//cout << "Event NOTOK : "<<ext->d_name <<"/"<<get_name()<< endl;	
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
	
	//if (ext->event_period == DEFAULT_EVENT_PERIOD * DELTA_PERIODIC)	
	//	conf.event_prop.per_event.period = CORBA::string_dup(AlrmValueNotSpec);
	
	if (ext->event_period == INT_MAX * DELTA_PERIODIC)	
		conf.event_prop.per_event.period = CORBA::string_dup(AlrmValueNotSpec);	
	else
	{

		int per = (int)((double)ext->event_period / DELTA_PERIODIC);
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

	//if (ext->archive_period == 10 * (int)(DEFAULT_EVENT_PERIOD * DELTA_PERIODIC))
	//	conf.event_prop.arch_event.period = CORBA::string_dup(AlrmValueNotSpec);
	
	if (ext->archive_period == (int)(INT_MAX * DELTA_PERIODIC))
	{
		conf.event_prop.arch_event.period = CORBA::string_dup(AlrmValueNotSpec);
	}
	else
	{
		int per = (int)((double)ext->archive_period / DELTA_PERIODIC);
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

void Attribute::set_properties(const Tango::AttributeConfig &conf,string &dev_name)
{

//
// Copy only a sub-set of the new properties
// For each "string" property, an empty string means returns to its
// default value which could be the library default value or the 
// user defined default value
//


	Tango::Util *tg = Tango::Util::instance();
	Tango::DeviceImpl *dev = tg->get_device_by_name(dev_name);
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);
	vector<AttrProperty> &def_user_prop = att.get_user_default_properties();
	long nb_user = def_user_prop.size();
	
	if (strlen(conf.description) != 0)
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
		
	if (strlen(conf.label) != 0)
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
		
	if (strlen(conf.unit) != 0)
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
	
	if (strlen(conf.standard_unit) != 0)	
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
	
	if (strlen(conf.display_unit) != 0)
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
	
	if (strlen(conf.format) != 0)
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				min_value.lg = (long)min_value.db;
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
				if (!(str >> min_value.uch))
					throw_err_format("min_value",dev_name);
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				max_value.lg = (long)max_value.db;
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
				if (!(str >> max_value.uch))
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				min_alarm.lg = (long)min_alarm.db;
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
				if (!(str >> min_alarm.uch))
					throw_err_format("min_alarm",dev_name);
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				max_alarm.lg = (long)max_alarm.db;
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
				if (!(str >> max_alarm.uch))
					throw_err_format("max_alarm",dev_name);
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				min_alarm.lg = (long)min_alarm.db;
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
				if (!(str >> min_warning.uch))
					throw_err_format("min_warning",dev_name);
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				max_alarm.lg = (long)max_alarm.db;
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
				if (!(str >> max_warning.uch))
					throw_err_format("max_warning",dev_name);
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
		    (data_type != Tango::DEV_BOOLEAN))
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
				max_alarm.lg = (long)max_alarm.db;
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
				if (!(str >> delta_val.uch))
					throw_err_format("delta_val",dev_name);
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
		    (data_type != Tango::DEV_BOOLEAN))
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
		double tmp_period = DEFAULT_EVENT_PERIOD;
#ifdef WIN32
		double tmp = tmp_period * DELTA_PERIODIC;
		double int_part,eve_round;
		double frac = modf(tmp,&int_part);
		if (frac >= 0.5)
			eve_round = ceil(tmp);
		else
			eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
		double eve_round = rint(tmp_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
		double eve_round = rint(tmp_period * DELTA_PERIODIC);
		#else
		double eve_round = round(tmp_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
		ext->event_period = (int)eve_round;
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
#ifdef WIN32
			double tmp = (double)tmp_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
				eve_round = ceil(tmp);
			else
				eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
			double eve_round = rint((double)tmp_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint((double)tmp_period * DELTA_PERIODIC);
		#else
			double eve_round = round((double)tmp_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			ext->event_period = (int)eve_round;
		}
		else
		{
			double tmp_period = DEFAULT_EVENT_PERIOD;
#ifdef WIN32
			double tmp = tmp_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
				eve_round = ceil(tmp);
			else
				eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
			double eve_round = rint(tmp_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint(tmp_period * DELTA_PERIODIC);
		#else
			double eve_round = round(tmp_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			ext->event_period = (int)eve_round;
		}
	}
	
	if (strcmp(conf.event_prop.arch_event.period,NotANumber) == 0)
	{
		//double tmp_period = 10 * DEFAULT_EVENT_PERIOD;
		double tmp_period = INT_MAX;
		
#ifdef WIN32
		double tmp = tmp_period * DELTA_PERIODIC;
		double int_part,eve_round;
		double frac = modf(tmp,&int_part);
		if (frac >= 0.5)
			eve_round = ceil(tmp);
		else
			eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
		double eve_round = rint(tmp_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
		double eve_round = rint(tmp_period * DELTA_PERIODIC);
		#else
		double eve_round = round(tmp_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
		ext->archive_period = (int)eve_round;
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
#ifdef WIN32
			double tmp = (double)tmp_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
				eve_round = ceil(tmp);
			else
				eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
			double eve_round = rint((double)tmp_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint((double)tmp_period * DELTA_PERIODIC);
		#else
			double eve_round = round((double)tmp_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			ext->archive_period = (int)eve_round;
		}
		else
		{
			//double tmp_period = 10 * DEFAULT_EVENT_PERIOD;
			double tmp_period = INT_MAX;
			
#ifdef WIN32
			double tmp = tmp_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
				eve_round = ceil(tmp);
			else
				eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD) || (defined __HP_aCC))
			double eve_round = rint(tmp_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint(tmp_period * DELTA_PERIODIC);
		#else
			double eve_round = round(tmp_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			ext->archive_period = (int)eve_round;
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
			DbDatum max_val("delta_t");
			const char *tmp = conf.att_alarm.delta_t.in();
			max_val << tmp;
			db_d.push_back(max_val);
			prop_to_update++;
		}
	}

	if (strcmp(conf.att_alarm.delta_t,NotANumber) == 0)
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

	if (strcmp(conf.event_prop.per_event.period,NotANumber) == 0)
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

	if (strcmp(conf.event_prop.arch_event.period,NotANumber) == 0)
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
	
	if (strcmp(conf.description,DescNotSpec) != 0)
	{
		DbDatum desc("description");
		const char *tmp = conf.description.in();
		desc << tmp;
		db_d.push_back(desc);
		prop_to_update++;
	}
	
	if (strlen(conf.description) == 0)
	{
		DbDatum del_desc("description");
		db_del.push_back(del_desc);
		prop_to_delete++;
	}
	
	if (strcmp(conf.label,name.c_str()) != 0)
	{
		DbDatum lab("label");
		const char *tmp = conf.label.in();
		lab << tmp;
		db_d.push_back(lab);
		prop_to_update++;
	}
	
	if (strlen(conf.label) == 0)
	{
		DbDatum del_lab("label");
		db_del.push_back(del_lab);
		prop_to_delete++;
	}
	
	if (strcmp(conf.unit,UnitNotSpec) != 0)
	{
		DbDatum uni("unit");
		const char *tmp = conf.unit.in();
		uni << tmp;
		db_d.push_back(uni);
		prop_to_update++;
	}
	
	if (strlen(conf.unit) == 0)
	{
		DbDatum del_unit("unit");
		db_del.push_back(del_unit);
		prop_to_delete++;
	}
	
	if (strcmp(conf.standard_unit,StdUnitNotSpec) != 0)
	{
		DbDatum std_uni("standard_unit");
		const char *tmp = conf.standard_unit.in();
		std_uni << tmp;
		db_d.push_back(std_uni);
		prop_to_update++;
	}

	if (strlen(conf.standard_unit) == 0)
	{
		DbDatum del_std_unit("standard_unit");
		db_del.push_back(del_std_unit);
		prop_to_delete++;
	}
	
	if (strcmp(conf.display_unit,DispUnitNotSpec) != 0)
	{
		DbDatum disp_uni("display_unit");
		const char *tmp = conf.display_unit.in();
		disp_uni << tmp;
		db_d.push_back(disp_uni);
		prop_to_update++;
	}

	if (strlen(conf.display_unit) == 0)
	{
		DbDatum del_disp_unit("display_unit");
		db_del.push_back(del_disp_unit);
		prop_to_delete++;
	}
	
	if (strcmp(conf.format,FormatNotSpec) != 0)
	{
		DbDatum form("format");
		const char *tmp = conf.format.in();
		form << tmp;
		db_d.push_back(form);
		prop_to_update++;
	}

	if (strlen(conf.format) == 0)
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
				tmp_sh[0] = *p_data;
			else
			{
				value.sh_seq = new Tango::DevVarShortArray(data_size);
				value.sh_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.sh_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
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
				tmp_lo[0] = *p_data;
			else
			{
				value.lg_seq = new Tango::DevVarLongArray(data_size);
				value.lg_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.lg_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
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
				tmp_fl[0] = *p_data;
			else
			{
				value.fl_seq = new Tango::DevVarFloatArray(data_size);
				value.fl_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.fl_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,p_data,release);
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
				tmp_db[0] = *p_data;
			else
			{
				value.db_seq = new Tango::DevVarDoubleArray(data_size);
				value.db_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.db_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
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
				tmp_str[0] = *p_data;
			else
			{
				value.str_seq = new Tango::DevVarStringArray(data_size);
				value.str_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.str_seq)[k] = CORBA::string_dup(p_data[k]);
			}	
			if (release == true)
				delete p_data;
		}
		else
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
				tmp_ush[0] = *p_data;
			else
			{
				value.ush_seq = new Tango::DevVarUShortArray(data_size);
				value.ush_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ush_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,p_data,release);
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
				tmp_boo[0] = *p_data;
			else
			{
				value.boo_seq = new Tango::DevVarBooleanArray(data_size);
				value.boo_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.boo_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,p_data,release);
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
				tmp_cha[0] = *p_data;
			else
			{
				value.cha_seq = new Tango::DevVarCharArray(data_size);
				value.cha_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.cha_seq)[k] = p_data[k];
			}	
			if (release == true)
				delete p_data;
		}
		else
			value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,p_data,release);
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevShort *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevShort *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevLong *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevLong *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevFloat *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevFloat *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevDouble *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevDouble *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevString *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevString *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevUShort *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevUShort *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
	//fire_quality_event();
}

#ifdef WIN32
void Attribute::set_value_date_quality(Tango::DevUChar *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
}
#else
void Attribute::set_value_date_quality(Tango::DevUChar *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
	//fire_quality_event();
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
#ifdef WIN32
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

/*
     The old Version of E. Taurel
	  Changed 15.12.05 JM
//
// If the attribute quality factor is set to INVALID, don't check anything
//

	if (quality == Tango::ATTR_INVALID)
	{
		return returned;
	}
*/

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
		if ((bs.test(Attribute::min_warn) == true) || 
	    	    (bs.test(Attribute::max_warn) == true))
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
	
//
// Check the min alarm if defined
//

	long i;
	if (alarm_conf.test(min_level) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.sh_seq)[i] <= min_alarm.sh)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(min_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_LONG:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.lg_seq)[i] <= min_alarm.lg)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(min_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_DOUBLE:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.db_seq)[i] <= min_alarm.db)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(min_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_FLOAT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.fl_seq)[i] <= min_alarm.fl)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(min_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_USHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.ush_seq)[i] <= min_alarm.ush)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(min_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_UCHAR:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.cha_seq)[i] <= min_alarm.uch)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(min_level);
					returned = true;
				}
			}
			break;
		}
	}
	
//
// Check the max alarm if defined
//

	if (alarm_conf.test(max_level) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.sh_seq)[i] >= max_alarm.sh)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(max_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_LONG:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.lg_seq)[i] >= max_alarm.lg)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(max_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_DOUBLE:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.db_seq)[i] >= max_alarm.db)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(max_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_FLOAT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.fl_seq)[i] >= max_alarm.fl)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(max_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_USHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.ush_seq)[i] >= max_alarm.ush)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(max_level);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_UCHAR:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.cha_seq)[i] >= max_alarm.uch)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(max_level);
					returned = true;
				}
			}
			break;
		}
	}
	
	return returned;
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
	
//
// Check the min alarm if defined
//

	long i;
	if (alarm_conf.test(min_warn) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.sh_seq)[i] <= min_warning.sh)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(min_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_LONG:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.lg_seq)[i] <= min_warning.lg)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(min_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_DOUBLE:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.db_seq)[i] <= min_warning.db)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(min_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_FLOAT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.fl_seq)[i] <= min_warning.fl)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(min_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_USHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.ush_seq)[i] <= min_warning.ush)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(min_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_UCHAR:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.cha_seq)[i] <= min_warning.uch)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(min_warn);
					returned = true;
				}
			}
			break;
		}
	}
	
//
// Check the max alarm if defined
//

	if (alarm_conf.test(max_warn) == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.sh_seq)[i] >= max_warning.sh)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(max_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_LONG:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.lg_seq)[i] >= max_warning.lg)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(max_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_DOUBLE:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.db_seq)[i] >= max_warning.db)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(max_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_FLOAT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.fl_seq)[i] >= max_warning.fl)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(max_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_USHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.ush_seq)[i] >= max_warning.ush)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(max_warn);
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_UCHAR:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.cha_seq)[i] >= max_warning.uch)
				{
					quality = Tango::ATTR_WARNING;
					alarm.set(max_warn);
					returned = true;
				}
			}
			break;
		}
	}
	
	return returned;
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
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::add_write_value
// 
// description : 	These methods add the associated writable attribute
//			value to the read attribute buffer and create a
//			sequence from the attribute internal buffer
//
// in :			val : The associated write attribute value
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
		tmp_str[1] = (*val_ptr)[0];	
		value.str_seq = new Tango::DevVarStringArray(data_size + 1,data_size + 1,tmp_str,false);	 
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


//+-------------------------------------------------------------------------
//
// method : 		Attribute::Attribute_2_AttributeValue
// 
// description : 	Build an AttributeValue_3 object from the Attribute
//			object content
//
// in :			ptr : Pointer to the AttributeValue_3 object to
//			      be filled in
//			dev : The device to which the attribute belongs to
//
//--------------------------------------------------------------------------


void Attribute::Attribute_2_AttributeValue(Tango::AttributeValue_3 *ptr,
					   Tango::DeviceImpl *dev)
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
		
		CORBA::Any &a = ptr->value;
		long	seq_length = value.sh_seq->length();
			
		switch (data_type)
		{
		case Tango::DEV_SHORT :
			sh_tmp_ptr = get_short_value()->get_buffer();
			sh_seq = new Tango::DevVarShortArray(seq_length,seq_length,sh_tmp_ptr,false);
			a <<= sh_seq;
			break;

		case Tango::DEV_LONG :
			lo_tmp_ptr = get_long_value()->get_buffer();
			lo_seq = new Tango::DevVarLongArray(seq_length,seq_length,lo_tmp_ptr,false);
			a <<= lo_seq;
			break;

		case Tango::DEV_DOUBLE :
			db_tmp_ptr = get_double_value()->get_buffer();
			db_seq = new Tango::DevVarDoubleArray(seq_length,seq_length,db_tmp_ptr,false);
			//db_seq = new Tango::DevVarDoubleArray(data_size,data_size,db_tmp_ptr,false);
			a <<= db_seq;
			break;

		case Tango::DEV_STRING :
			str_tmp_ptr = get_string_value()->get_buffer();
			str_seq = new Tango::DevVarStringArray(seq_length,seq_length,str_tmp_ptr,false);
			a <<= str_seq;
			break;

		case Tango::DEV_FLOAT : 		
			fl_tmp_ptr = get_float_value()->get_buffer();
			fl_seq = new Tango::DevVarFloatArray(seq_length,seq_length,fl_tmp_ptr,false);
			a <<= fl_seq;
			break;

		case Tango::DEV_BOOLEAN :
			bo_tmp_ptr = get_boolean_value()->get_buffer();
			bo_seq = new Tango::DevVarBooleanArray(seq_length,seq_length,bo_tmp_ptr,false);
			a <<= bo_seq;
			break;

		case Tango::DEV_USHORT :
			ush_tmp_ptr = get_ushort_value()->get_buffer();
			ush_seq = new Tango::DevVarUShortArray(seq_length,seq_length,ush_tmp_ptr,false);
			a <<= ush_seq;
			break;

		case Tango::DEV_UCHAR :
			uch_tmp_ptr = get_uchar_value()->get_buffer();
			uch_seq = new Tango::DevVarUCharArray(seq_length,seq_length,uch_tmp_ptr,false);
			a <<= uch_seq;
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




//+-------------------------------------------------------------------------
//
// method : 		Attribute::fire_change_event
// 
// description : 	Fire a change change event for the attribute value.
//
//--------------------------------------------------------------------------


void Attribute::fire_change_event(bool detect, DevFailed *except)
{
//
// Check if it is needed to send an event
//

	time_t now;
	int change_subscription;

	now = time(NULL);
	change_subscription = (int)now - ext->event_change_subscription;

	if (change_subscription > EVENT_RESUBSCRIBE_PERIOD)
		return;

//
// Get the event supplier, and simply return if not created
//

	EventSupplier *event_supplier;
	Tango::Util *tg = Util::instance();
	event_supplier = tg->get_event_supplier();
	if (event_supplier == NULL)
		return;

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

	//
	// Build one AttributeValue_3 object
	//
	
	Tango::AttributeValue_3 *send_attr;
	try
	{	
		send_attr = new Tango::AttributeValue_3;
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Attribute::fire_change_event");
	}
	
	Attribute_2_AttributeValue(send_attr,ext->dev);

// 
// Fire event
//
	if ( detect == true )
	{
		event_supplier->detect_and_push_change_event(ext->dev,
						 											*send_attr,
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
			ext->prev_change_event.err = true;
		else
		{
			if (ext->prev_change_event.quality !=  send_attr->quality)
				{
				quality_change = true;
				}
			ext->prev_change_event.value = send_attr->value;
			ext->prev_change_event.quality = send_attr->quality;
			ext->prev_change_event.err = false;
		}
	
		string domain_name = ext->dev->get_name() + "/" + name;
		
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

		event_supplier->push_event(ext->dev,
											"change",
											filterable_names,
											filterable_data,
											filterable_names_lg,
											filterable_data_lg,
											*send_attr,
											name,
											except);	
	}
							     
	delete send_attr;
	
	// delete the data values allocated in the attribute
	delete_seq();
	value_flag == false;
}



//+-------------------------------------------------------------------------
//
// method : 		Attribute::fire_quality_event
// 
// description : 	Send a quality change event
//
//--------------------------------------------------------------------------


void Attribute::fire_quality_event()
{
//
// Get the event supplier, and simply return if not created
//

	EventSupplier *event_supplier;
	Tango::Util *tg = Util::instance();
	event_supplier = tg->get_event_supplier();
	if (event_supplier == NULL)
		return;

//
// Check if it is needed to send an event
//

	time_t now;
	int quality_subscription;

	now = time(NULL);
	quality_subscription = (int)now - ext->event_quality_subscription;

	if (quality_subscription > EVENT_RESUBSCRIBE_PERIOD)
		return;
		
//
// Check that the attribute value has been set
//

	if (quality != Tango::ATTR_INVALID)
	{
		if (value_flag == false)
		{
			TangoSys_OMemStream o;

			o << "Value for attribute ";
			o << name;
			o << " has not been updated. Can't send quality change event\n";
			o << "Do you set the attribute value (using set_value(...) method) before calling set_quality()?" << ends;

			Except::throw_exception((const char *)"API_AttrValueNotSet",o.str(),
				        	(const char *)"Attribute::fire_quality_event");
		}
	}

//
// Retrieve device object if not already done
//

	if (ext->dev == NULL)
		ext->dev = tg->get_device_by_name(ext->d_name);


//
// Build one AttributeValue_3 object
//
	
	Tango::AttributeValue_3 *send_attr;
	try
	{	
		send_attr = new Tango::AttributeValue_3;
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"Attribute::fire_quality_event");
	}
	
	Attribute_2_AttributeValue(send_attr,ext->dev);

// 
// Fire event
//

	event_supplier->detect_and_push_quality_change_event(ext->dev,
							     *send_attr,
							     *this,
							     name,
							     NULL);
							     
	delete send_attr;
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
		fire_change_event (false, NULL);
		//fire_quality_event();
		
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
