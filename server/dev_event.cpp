static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :		dev_event.cpp
//
// description :	C++ source code for the DeviceImpl
//			class methods related to manually firing events.
//
// project :		TANGO
//
// author(s) :		J.Meyer + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
// Revision 3.9  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.8  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.7  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.6  2008/09/04 13:19:30  taurel
// - Add push_event() feature for attribute of the DevEncoded data type
//
// Revision 3.5  2007/04/20 14:40:28  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.4  2007/04/16 14:56:36  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.3  2007/03/06 08:18:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.2  2006/07/04 13:12:17  jensmeyer
// Corrected time include files for Windows.
//
// Revision 3.1  2006/06/06 10:44:05  taurel
// - Modified the DeviceImpl::push_event() method used to send user event.
// It is now coherent with the other events type (change and archhive)
//
//-============================================================================

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

#ifdef TANGO_HAS_LOG4TANGO
#include <logging.h>
#endif

namespace Tango
{

//////////////////// Push user event methods!!! //////////////////////////////////////


//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_event
// 
// description :	Push a user event to the Notification service
//			Should be used to push user events for the state and status
//			attributes as well as pushing an exception as change event.
//
// argument: in :	attr_name : name of the attribute
//			filt_names : The filterable fields name
//			filt_vals : The filterable fields value (as double)
//			*except   : Tango exception to be pushed as a user event for the attribute.
//
//--------------------------------------------------------------------------

void DeviceImpl::push_event(string attr_name, vector<string> &filt_names,vector<double> &filt_vals,DevFailed *except)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);
	
	// search the attribute from the attribute list		
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	attr.fire_event (filt_names,filt_vals,except);
}

//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_event
// 
// description :	Push an attribute change event with valid data to the notification service
//
// argument: in :	attr_name : name of the attribute
//			filt_names : The filterable fields name
//			filt_vals : The filterable fields value (as double)
//			*p_data	 : pointer to attribute data
//			x : The attribute x length. Default value is 1
//			y : The attribute y length. Default value is 0
//			release   : The release flag. If true, memory pointed to by p_data will be 
// 				    freed after being send to the client. Default value is false.
//--------------------------------------------------------------------------

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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


void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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


void DeviceImpl::push_event(string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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



void DeviceImpl::push_event(string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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



void DeviceImpl::push_event(string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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



void DeviceImpl::push_event(string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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



void DeviceImpl::push_event(string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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



void DeviceImpl::push_event(string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::push_event
// 
// description :	Push an attribute change event with data, time stamp and a quality
//			factor to the notification service.
//
// argument: in :	attr_name : name of the attribute
//			filt_names : The filterable fields name
//			filt_vals : The filterable fields value (as double)
//			t : the time stamp
//			qual : the attribute quality factor
//			*p_data	: pointer to attribute data
//			x : The attribute x length. Default value is 1
//			y : The attribute y length. Default value is 0
//		 	release : The release flag. If true, memory pointed to by p_data will be 
// 				  freed after being send to the client. Default value is false.
//--------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevShort *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevLong *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevLong64 *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevFloat *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevDouble *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevString *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevBoolean *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevUShort *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevUChar *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevULong *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevULong64 *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevState *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevEncoded *p_data,struct _timeb &t, Tango::AttrQuality qual,
			     long x,long y ,bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
			     Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size, struct _timeb &t, Tango::AttrQuality qual,
			     bool release)
#else
void DeviceImpl::push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,
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



//+-------------------------------------------------------------------------
// method :			DeviceImpl::set_change_event
//
// description :	Set a flag to indicate that the server pushes change events manually, without
// 					the polling to be started for the attribute.
// 					If the detect parameter is set to true, the criteria specified for the change 
// 					event are verified and the event is only pushed if they are fullfilled.
// 					If detect is set to false the event is fired without any value checking!
//
//  argument: in : implemented  : True when the server fires change events manually. 
//  					 detect       : Triggers the verification of the change event 
//											 properties when set to true.
//--------------------------------------------------------------------------

void DeviceImpl::set_change_event  (string attr_name, bool implemented, bool detect)
{
	// search the attribute from the attribute list		
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	attr.set_change_event (implemented, detect);	
}


//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_change_event
// 
// description :	Push an attribute change event to the Notification service
//						Should be used to push change events for the state and status
//						attributes as well as pushing an exception as change event.
//
// argument: in :	attr_name : name of the attribute
//						*except   : Tango exception to be pushed as a change event for the attribute.
//
//--------------------------------------------------------------------------

void DeviceImpl::push_change_event(string attr_name, DevFailed *except)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);
	
	// search the attribute from the attribute list		
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	attr.fire_change_event (except);
}

//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_change_event
// 
// description :	Push an attribute change event with valid data to the notification service
//
// argument: in :	attr_name : name of the attribute
//						*p_data	 : pointer to attribute data
//						x 			 : The attribute x length. Default value is 1
//					   y 			 : The attribute y length. Default value is 0
//					   release   : The release flag. If true, memory pointed to by p_data will be 
// 									freed after being send to the client. Default value is false.
//--------------------------------------------------------------------------

void DeviceImpl::push_change_event (string attr_name, Tango::DevShort *p_data, long x,long y ,bool release)
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


void DeviceImpl::push_change_event (string attr_name, Tango::DevLong *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event (string attr_name, Tango::DevLong64 *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event(string attr_name, Tango::DevFloat *p_data, long x, long y , bool release)
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



void DeviceImpl::push_change_event(string attr_name, Tango::DevDouble *p_data, long x, long y , bool release)
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



void DeviceImpl::push_change_event(string attr_name, Tango::DevString *p_data, long x, long y , bool release)
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



void DeviceImpl::push_change_event(string attr_name, Tango::DevBoolean *p_data, long x, long y , bool release)
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



void DeviceImpl::push_change_event(string attr_name, Tango::DevUShort *p_data, long x, long y , bool release)
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



void DeviceImpl::push_change_event(string attr_name, Tango::DevUChar *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event (string attr_name, Tango::DevULong *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event (string attr_name, Tango::DevULong64 *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event (string attr_name, Tango::DevState *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event (string attr_name, Tango::DevEncoded *p_data, long x, long y , bool release)
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

void DeviceImpl::push_change_event (string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data, long size, bool release)
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
//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_change_event
// 
// description :	Push an attribute change event with data, time stamp and a quality
//						factor to the notification service.
//
// argument: in :	attr_name : name of the attribute
//						t			 : the time stamp
//						qual      : the attribute quality factor
//						*p_data	 : pointer to attribute data
//						x 			 : The attribute x length. Default value is 1
//					   y 			 : The attribute y length. Default value is 0
//					   release   : The release flag. If true, memory pointed to by p_data will be 
// 									freed after being send to the client. Default value is false.
//--------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
void DeviceImpl::push_change_event (string attr_name, Tango::DevShort *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevShort *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevLong *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevLong *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevLong64 *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevLong64 *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevFloat *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevFloat *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevDouble *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevDouble *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevString *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevString *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevBoolean *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevBoolean *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevUShort *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevUShort *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevUChar *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevUChar *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevULong *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevULong *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevULong64 *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevULong64 *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevState *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevState *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevEncoded *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevEncoded *p_data, 
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
void DeviceImpl::push_change_event (string attr_name, Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  bool release)
#else
void DeviceImpl::push_change_event (string attr_name, Tango::DevString *p_str_data,Tango::DevUChar *p_data,long size, 
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



//+-------------------------------------------------------------------------
// method :			DeviceImpl::set_archive_event
//
// description :	Set a flag to indicate that the server pushes archive events manually, without
// 					the polling to be started for the attribute.
// 					If the detect parameter is set to true, the criteria specified for the archive 
// 					event are verified and the event is only pushed if they are fullfilled.
// 					If detect is set to false the event is fired without any value checking!
//
//  argument: in : implemented  : True when the server fires archive events manually. 
//  					 detect       : Triggers the verification of the archive event 
//											 properties when set to true.
//--------------------------------------------------------------------------

void DeviceImpl::set_archive_event  (string attr_name, bool implemented, bool detect)
{
	// search the attribute from the attribute list		
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	attr.set_archive_event (implemented, detect);	
}


//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_archive_event
// 
// description :	Push an attribute archive event to the Notification service
//						Should be used to push archive events for the state and status
//						attributes as well as pushing an exception as archive event.
//
// argument: in :	attr_name : name of the attribute
//						*except   : Tango exception to be pushed as a archive event for the attribute.
//
//--------------------------------------------------------------------------

void DeviceImpl::push_archive_event(string attr_name, DevFailed *except)
{
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);
	
	// search the attribute from the attribute list		
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr           = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	attr.fire_archive_event (except);
}

//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_archive_event
// 
// description :	Push an attribute archive event with valid data to the notification service
//
// argument: in :	attr_name : name of the attribute
//						*p_data	 : pointer to attribute data
//						x 			 : The attribute x length. Default value is 1
//					   y 			 : The attribute y length. Default value is 0
//					   release   : The release flag. If true, memory pointed to by p_data will be 
// 									freed after being send to the client. Default value is false.
//--------------------------------------------------------------------------

void DeviceImpl::push_archive_event (string attr_name, Tango::DevShort *p_data, long x,long y ,bool release)
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


void DeviceImpl::push_archive_event (string attr_name, Tango::DevLong *p_data, long x, long y , bool release)
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

void DeviceImpl::push_archive_event (string attr_name, Tango::DevLong64 *p_data, long x, long y , bool release)
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


void DeviceImpl::push_archive_event(string attr_name, Tango::DevFloat *p_data, long x, long y , bool release)
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



void DeviceImpl::push_archive_event(string attr_name, Tango::DevDouble *p_data, long x, long y , bool release)
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



void DeviceImpl::push_archive_event(string attr_name, Tango::DevString *p_data, long x, long y , bool release)
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



void DeviceImpl::push_archive_event(string attr_name, Tango::DevBoolean *p_data, long x, long y , bool release)
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



void DeviceImpl::push_archive_event(string attr_name, Tango::DevUShort *p_data, long x, long y , bool release)
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



void DeviceImpl::push_archive_event(string attr_name, Tango::DevUChar *p_data, long x, long y , bool release)
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

void DeviceImpl::push_archive_event (string attr_name, Tango::DevULong *p_data, long x, long y , bool release)
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevULong64 *p_data, long x, long y , bool release)
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevState *p_data, long x, long y , bool release)
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

void DeviceImpl::push_archive_event (string attr_name, Tango::DevEncoded *p_data, long x, long y , bool release)
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

void DeviceImpl::push_archive_event (string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data, long size, bool release)
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

//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_archive_event
// 
// description :	Push an attribute archive event with data, time stamp and a quality
//						factor to the notification service.
//
// argument: in :	attr_name : name of the attribute
//						t			 : the time stamp
//						qual      : the attribute quality factor
//						*p_data	 : pointer to attribute data
//						x 			 : The attribute x length. Default value is 1
//					   y 			 : The attribute y length. Default value is 0
//					   release   : The release flag. If true, memory pointed to by p_data will be 
// 									freed after being send to the client. Default value is false.
//--------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
void DeviceImpl::push_archive_event (string attr_name, Tango::DevShort *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevShort *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevLong *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevLong *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevLong64 *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevLong64 *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevFloat *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevFloat *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevDouble *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevDouble *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevString *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevString *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevBoolean *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevBoolean *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevUShort *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevUShort *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevUChar *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevUChar *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevULong *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevULong *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevULong64 *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevULong64 *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevState *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevState *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevEncoded *p_data, 
											  struct _timeb &t, Tango::AttrQuality qual,
											  long x,long y ,bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevEncoded *p_data, 
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
void DeviceImpl::push_archive_event (string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data,long size,
											  struct _timeb &t, Tango::AttrQuality qual,
											  bool release)
#else
void DeviceImpl::push_archive_event (string attr_name, Tango::DevString *p_str_data, Tango::DevUChar *p_data,long size, 
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



//+-------------------------------------------------------------------------
//
// method :			DeviceImpl::push_data_ready_event
// 
// description :	Push an attribute data ready event
//
// argument: in :	attr_name : name of the attribute
//					ctr : user counter (optional)
//
//--------------------------------------------------------------------------

void DeviceImpl::push_data_ready_event (const string &attr_name, Tango::DevLong ctr)
{

	Tango::Util *tg = Tango::Util::instance();
	EventSupplier *ev_supply = tg->get_event_supplier();
	if (ev_supply == NULL)
		tg->create_event_supplier();
	
	// get the tango synchroisation monitor
	Tango::AutoTangoMonitor synch(this);

	// search the attribute from the attribute list to check that it exist		
	Tango::MultiAttribute *attr_list = get_device_attr();
	Tango::Attribute &attr = attr_list->get_attr_by_name (attr_name.c_str());

	// push the event
	ev_supply->push_att_data_ready_event(this,attr_name,attr.get_data_type(),ctr);

}


} // End of Tango namespace
