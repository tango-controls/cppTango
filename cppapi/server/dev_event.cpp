//+==================================================================================================================
//
// file :		dev_event.cpp
//
// description :	C++ source code for the DeviceImpl class methods related to manually firing events.
//
// project :		TANGO
//
// author(s) :		J.Meyer + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//
//-=================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventsupplier.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#include <logging.h>

namespace Tango
{

//////////////////// Push user event methods!!! //////////////////////////////////////


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_event
//
// description :
//		Push a user event to the Notification service. Should be used to push user events for the state and status
//		attributes as well as pushing an exception as change event.
//
// args :
//  	in :
//			- attr_name : name of the attribute
//			- filt_names : The filterable fields name
//			- filt_vals : The filterable fields value (as double)
//			- except   : Tango exception to be pushed as a user event for the attribute.
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_event(std::string attr_name, std::vector<std::string> &filt_names,std::vector<double> &filt_vals,DevFailed *except)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	attr.fire_event (filt_names,filt_vals,except);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_event
//
// description :
//		Push an attribute change event with valid data to the notification service
//
// args :
//  	in :
//			- attr_name : name of the attribute
//			- filt_names : The filterable fields name
//			- filt_vals : The filterable fields value (as double)
//			- p_data	 : pointer to attribute data
//			- x : The attribute x length. Default value is 1
//			- y : The attribute y length. Default value is 0
//			- release   : The release flag. If true, memory pointed to by p_data will be freed after being send to the
//						  client. Default value is false.
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevShort *p_data, long x,long y ,bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}


void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevLong *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevLong64 *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}


void DeviceImpl::push_event(std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			    Tango::DevFloat *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}



void DeviceImpl::push_event(std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			    Tango::DevDouble *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}



void DeviceImpl::push_event(std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			    Tango::DevString *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}



void DeviceImpl::push_event(std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			    Tango::DevBoolean *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}



void DeviceImpl::push_event(std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			    Tango::DevUShort *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}



void DeviceImpl::push_event(std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			    Tango::DevUChar *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevULong *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevULong64 *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevState *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevEncoded *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevString *p_str, Tango::DevUChar *p_data, long size , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_str,p_data, size, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_event
//
// description :
//		Push an attribute change event with data, time stamp and a quality factor to the notification service.
//
// args :
// 		in :
//			- attr_name : name of the attribute
//			- filt_names : The filterable fields name
//			- filt_vals : The filterable fields value (as double)
//			- t : the time stamp
//			- qual : the attribute quality factor
//			- p_data	: pointer to attribute data
//			- x : The attribute x length. Default value is 1
//			- y : The attribute y length. Default value is 0
//		 	- release : The release flag. If true, memory pointed to by p_data will be freed after being send to the
//						client. Default value is false.
//
//----------------------------------------------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevShort *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevShort *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevLong *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevLong *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevLong64 *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevLong64 *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevFloat *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevFloat *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevDouble *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevDouble *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevString *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevString *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevBoolean *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevBoolean *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevUShort *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevUShort *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevUChar *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevUChar *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevULong *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevULong *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevULong64 *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevULong64 *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevState *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevState *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevEncoded *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevEncoded *p_data,struct timeval &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size, struct _timeb &t, Tango::AttrQuality qual,
			     bool release)
#else
void DeviceImpl::push_event (std::string attr_name,std::vector<std::string> &filt_names,std::vector<double> &filt_vals,
			     Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size, struct timeval &t, Tango::AttrQuality qual,
			     bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_str_data,p_data, size,t, qual, release);
	// push the event
	attr.fire_event(filt_names,filt_vals);
}

//////////////////// Push change event methods!!! //////////////////////////////////////



//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::set_change_event
//
// description :
//		Set a flag to indicate that the server pushes change events manually, without the polling to be started for
//		the attribute. If the detect parameter is set to true, the criteria specified for the change event are
//		verified and the event is only pushed if they are fulfilled. If detect is set to false the event is fired
//		without any value checking!
//
//	args :
//		in :
//  		- implemented  : True when the server fires change events manually.
//  		- detect : Triggers the verification of the change event properties when set to true.
//
//-----------------------------------------------------------------------------------------------------------------

void DeviceImpl::set_change_event  (std::string attr_name, bool implemented, bool detect)
{
	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	attr.set_change_event (implemented, detect);
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_change_event
//
// description :
//		Push an attribute change event to the Notification service. Should be used to push change events for the
//		state and status attributes as well as pushing an exception as change event.
//
// args :
//		in :
// 			- attr_name : name of the attribute
//			- except   : Tango exception to be pushed as a change event for the attribute.
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_change_event(std::string attr_name, DevFailed *except)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	attr.fire_change_event (except);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_change_event
//
// description :
//		Push an attribute change event with valid data to the notification service
//
// args:
//		in :
//			- attr_name : name of the attribute
//			- p_data : pointer to attribute data
//			- x : The attribute x length. Default value is 1
//			- y : The attribute y length. Default value is 0
//			- release : The release flag. If true, memory pointed to by p_data will be
// 						freed after being send to the client. Default value is false.
//
//-----------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevShort *p_data, long x,long y ,bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}


void DeviceImpl::push_change_event (std::string attr_name, Tango::DevLong *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevLong64 *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event(std::string attr_name, Tango::DevFloat *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}



void DeviceImpl::push_change_event(std::string attr_name, Tango::DevDouble *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}



void DeviceImpl::push_change_event(std::string attr_name, Tango::DevString *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}



void DeviceImpl::push_change_event(std::string attr_name, Tango::DevBoolean *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}



void DeviceImpl::push_change_event(std::string attr_name, Tango::DevUShort *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}



void DeviceImpl::push_change_event(std::string attr_name, Tango::DevUChar *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevULong *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevULong64 *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevState *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevEncoded *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_change_event();
}

void DeviceImpl::push_change_event (std::string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data, long size, bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_str_data, p_data, size, release);
	// push the event
	attr.fire_change_event();
}
//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_change_event
//
// description :
//		Push an attribute change event with data, time stamp and a quality factor to the notification service.
//
// args:
//		in :
//			- attr_name : name of the attribute
//			- t	: the time stamp
//			- qual : the attribute quality factor
//			- p_data : pointer to attribute data
//			- x : The attribute x length. Default value is 1
//			- y : The attribute y length. Default value is 0
//			- release  : The release flag. If true, memory pointed to by p_data will be
// 						 freed after being send to the client. Default value is false.
//
//-----------------------------------------------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevShort *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevShort *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevLong *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevLong *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}


#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevLong64 *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevLong64 *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevFloat *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevFloat *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevDouble *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevDouble *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevString *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevString *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevBoolean *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevBoolean *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevUShort *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevUShort *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevUChar *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevUChar *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevULong *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevULong *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevULong64 *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevULong64 *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevState *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevState *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevEncoded *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevEncoded *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_change_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size,
											  struct _timeb &t, Tango::AttrQuality qual,
											  bool release)
#else
void DeviceImpl::push_change_event (std::string attr_name, Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size,
												struct timeval &t, Tango::AttrQuality qual,
												bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_str_data,p_data, size, t, qual, release);
	// push the event
	attr.fire_change_event();
}


//////////////////// Push change archive methods!!! //////////////////////////////////////



//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::set_archive_event
//
// description :
//		Set a flag to indicate that the server pushes archive events manually, without the polling to be started
//		for the attribute. If the detect parameter is set to true, the criteria specified for the archive
// 		event are verified and the event is only pushed if they are fulfilled. If detect is set to false the event
//		is fired without any value checking!
//
// args :
//  	in :
//			- implemented  : True when the server fires archive events manually.
//  		- detect       : Triggers the verification of the archive event properties when set to true.
//
//----------------------------------------------------------------------------------------------------------------

void DeviceImpl::set_archive_event  (std::string attr_name, bool implemented, bool detect)
{
	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	attr.set_archive_event (implemented, detect);
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_archive_event
//
// description :
//		Push an attribute archive event to the Notification service. Should be used to push archive events for the
//		state and status attributes as well as pushing an exception as archive event.
//
// args :
//		in :
// 			- attr_name : name of the attribute
//			- except   : Tango exception to be pushed as a archive event for the attribute.
//
//-----------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_archive_event(std::string attr_name, DevFailed *except)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	attr.fire_archive_event (except);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_archive_event
//
// description :
//		Push an attribute archive event with valid data to the notification service
//
// args :
//		in :
// 			- attr_name : name of the attribute
//			- p_data : pointer to attribute data
//			- x : The attribute x length. Default value is 1
//			- y : The attribute y length. Default value is 0
//			- release  : The release flag. If true, memory pointed to by p_data will be
// 						 freed after being send to the client. Default value is false.
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevShort *p_data, long x,long y ,bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}


void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevLong *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}

void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevLong64 *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}


void DeviceImpl::push_archive_event(std::string attr_name, Tango::DevFloat *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}



void DeviceImpl::push_archive_event(std::string attr_name, Tango::DevDouble *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}



void DeviceImpl::push_archive_event(std::string attr_name, Tango::DevString *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}



void DeviceImpl::push_archive_event(std::string attr_name, Tango::DevBoolean *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}



void DeviceImpl::push_archive_event(std::string attr_name, Tango::DevUShort *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}



void DeviceImpl::push_archive_event(std::string attr_name, Tango::DevUChar *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}

void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevULong *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevULong64 *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevState *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}

void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevEncoded *p_data, long x, long y , bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_data, x, y, release);
	// push the event
	attr.fire_archive_event();
}

void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data, long size, bool release)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value (p_str_data,p_data, size, release);
	// push the event
	attr.fire_archive_event();
}

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_archive_event
//
// description :
//		Push an attribute archive event with data, time stamp and a quality factor to the notification service.
//
//	args :
//		in :
// 			- attr_name : name of the attribute
//			- t	: the time stamp
//			- qual  : the attribute quality factor
//			- p_data : pointer to attribute data
//			- x : The attribute x length. Default value is 1
//			- y : The attribute y length. Default value is 0
//			- release   : The release flag. If true, memory pointed to by p_data will be
// 						  freed after being send to the client. Default value is false.
//
//-----------------------------------------------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevShort *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevShort *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevLong *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevLong *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevLong64 *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevLong64 *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}


#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevFloat *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevFloat *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevDouble *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevDouble *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevString *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevString *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevBoolean *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevBoolean *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevUShort *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevUShort *p_data,
											  struct timeval &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevUChar *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevUChar *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevULong *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevULong *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevULong64 *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevULong64 *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevState *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevState *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevEncoded *p_data,
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevEncoded *p_data,
												struct timeval &t, Tango::AttrQuality qual,
												long x,long y ,bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_data, t, qual, x, y, release);
	// push the event
	attr.fire_archive_event();
}

#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data,long size,
											  struct _timeb &t, Tango::AttrQuality qual,
											  bool release)
#else
void DeviceImpl::push_archive_event (std::string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data,long size,
												struct timeval &t, Tango::AttrQuality qual,
												bool release)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// set the attribute value
	attr.set_value_date_quality (p_str_data, p_data, size, t, qual, release);
	// push the event
	attr.fire_archive_event();
}


//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::set_data_ready_event
//
// description :
//		Set a flag to indicate that the server pushes data ready events.
//
// args :
//  	in :
//			- attr_name  : The attribute name
//  		- implemented  : True when the server fires change events manually.
//
//----------------------------------------------------------------------------------------------------------------

void DeviceImpl::set_data_ready_event  (std::string attr_name, bool implemented)
{
	// search the attribute from the attribute list
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	attr.set_data_ready_event (implemented);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_data_ready_event
//
// description :
//		Push an attribute data ready event
//
// args:
//		in :
//			- attr_name : name of the attribute
//			- ctr : user counter (optional)
//
//----------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_data_ready_event (const std::string &attr_name, Tango::DevLong ctr)
{

	Tango::Util *tg = Tango::Util::instance();

// get the tango synchronisation monitor

	Tango::AutoTangoMonitor synch(this);

// search the attribute from the attribute list to check that it exist

    Tango::MultiAttribute *attr_list = get_device_attr();
    Tango::Attribute &attr = attr_list->get_attr_by_name (attr_name.c_str());

//
// Get event suppliers
//

    EventSupplier *event_supplier_nd = NULL;
    EventSupplier *event_supplier_zmq = NULL;

	if (attr.use_notifd_event() == true)
		event_supplier_nd = tg->get_notifd_event_supplier();
	if (attr.use_zmq_event() == true)
		event_supplier_zmq = tg->get_zmq_event_supplier();

	if ((event_supplier_nd == NULL) && (event_supplier_zmq == NULL))
		return;

//
// Push the event
//

    if (event_supplier_nd != NULL)
    {
        event_supplier_nd->push_att_data_ready_event(this,attr_name,attr.get_data_type(),ctr);
    }

    if (event_supplier_zmq != NULL)
    {
        event_supplier_zmq->push_att_data_ready_event(this,attr_name,attr.get_data_type(),ctr);
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_pipe_event
//
// description :
//		Push a pipe event.
//
// args :
//		in :
// 			- pipe_name : name of the pipe
//			- except   : Tango exception to be pushed as event for the pipe.
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_pipe_event(const std::string &pipe_name, DevFailed *except)
{
	// get the tango synchronisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the pipe from the attribute list
	Tango::Pipe &pi = get_device_class()->get_pipe_by_name(pipe_name,device_name_lower);

	// push the event
	pi.fire_event(this,except);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_pipe_event
//
// description :
//		Push a pipe event with data
//
// args:
//		in :
//			- pipe_name : name of the pipe
//			- p_data : pointer to pipe data
//
//-----------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_pipe_event (const std::string &pipe_name,Tango::DevicePipeBlob *p_data,bool reuse_it)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the pipe from the pipe list
	Tango::Pipe &pi = get_device_class()->get_pipe_by_name(pipe_name,device_name_lower);

	// push the event
	pi.fire_event(this,p_data,reuse_it);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_pipe_event
//
// description :
//		Push a pipe event with data and timestamp
//
// args:
//		in :
//			- pipe_name : name of the pipe
//			- p_data : pointer to pipe data
//			- t : timestamp
//
//-----------------------------------------------------------------------------------------------------------------

#ifdef _TG_WINDOWS_
void DeviceImpl::push_pipe_event (const std::string &pipe_name, Tango::DevicePipeBlob *p_data,struct _timeb &t,bool reuse_it)
#else
void DeviceImpl::push_pipe_event (const std::string &pipe_name, Tango::DevicePipeBlob *p_data,struct timeval &tv,bool reuse_it)
#endif
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the pipe from the pipe list
	Tango::Pipe &pi = get_device_class()->get_pipe_by_name (pipe_name,device_name_lower);

#ifdef _TG_WINDOWS_
	struct timeval tv;
	tv.tv_sec = (unsigned long)t.time;
	tv.tv_usec = (long)t.millitm * 1000;
#endif // _TG_WINDOWS_

	// push the event
	pi.fire_event(this,p_data,tv,reuse_it);
}


} // End of Tango namespace
