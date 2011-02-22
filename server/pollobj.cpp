static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               PollObj.cpp
//
// description :        C++ source code for the PollObj class.
//			This class is used to store all data specific to one
//			polled object and which does not need to be stored 
//			in the ring buffer
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/03/25 16:44:08  taurel
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
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:33  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:23  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:11  taurel
// See Tango WEB pages for list of changes
//
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
#include <pollobj.h>
#include <pollring.h>


#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		PollObj::PollObj
// 
// description : 	Two constructors for the PollObj class. The first one
//			constructs a PollObj instance with the default polling
//			ring depth
//			The second one create a PollObj instance with a 
//			specified polling ring depth
//
// argument : in : 	- d : The device pointer
//			- ty : The polled object type
//			- na : The polled object name
//			- user_upd : The polling update period (in mS)
//			- r_depth : The polling ring depth
//
//--------------------------------------------------------------------------

PollObj::PollObj(DeviceImpl *d,PollObjType ty,const string &na,long user_upd)
:dev(d),type(ty),name(na),ring()
{
	needed_time.tv_sec = 0;
	needed_time.tv_usec = 0;
	if (user_upd < 1000)
	{
		upd.tv_usec = user_upd * 1000;
		upd.tv_sec = 0;
	}
	else
	{
		upd.tv_sec = user_upd / 1000;
		upd.tv_usec = (user_upd - (upd.tv_sec * 1000)) * 1000;
	}
	max_delta_t = (double)(user_upd / 1000.0) * dev->get_poll_old_factor();
}

PollObj::PollObj(DeviceImpl *d,PollObjType ty,const string &na,
		 long user_upd,long r_depth)
:dev(d),type(ty),name(na),ring(r_depth)
{
	needed_time.tv_sec = 0;
	needed_time.tv_usec = 0;
	if (user_upd < 1000)
	{
		upd.tv_usec = user_upd * 1000;
		upd.tv_sec = 0;
	}
	else
	{
		upd.tv_sec = user_upd / 1000;
		upd.tv_usec = (user_upd - (upd.tv_sec * 1000)) * 1000;
	}	
	max_delta_t = (double)(user_upd / 1000.0) * dev->get_poll_old_factor();	
}


//+-------------------------------------------------------------------------
//
// method : 		PollObj::insert_data
// 
// description : 	These methods insert a new element in the object ring
//			buffer when its real data
//
// argument : in : 	- res : The Any returned by the command
//			- when : The date when data was read
//			- needed : The time needed to exceute command/attribute
//				   reading
//
//--------------------------------------------------------------------------

void PollObj::insert_data(CORBA::Any *res,
			  struct timeval &when,
			  struct timeval &needed)
{
	omni_mutex_lock(*this);
	
	ring.insert_data(res,when);
	needed_time = needed;
}

void PollObj::insert_data(Tango::AttributeValueList *res,
			  struct timeval &when,
			  struct timeval &needed)
{
	omni_mutex_lock(*this);
	
	ring.insert_data(res,when);
	needed_time = needed;
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::insert_except
// 
// description : 	This method insert a new element in the ring buffer
//			when this element is an exception
//
// argument : in : 	- res : The DevFailed exception
//			- when : The date when the exception was thrown
//			- needed : The time needed for the command/attribute
//				   reading
//
//--------------------------------------------------------------------------

void PollObj::insert_except(Tango::DevFailed *res,
			    struct timeval &when,
			    struct timeval &needed)
{
	omni_mutex_lock(*this);
	
	ring.insert_except(res,when);
	needed_time = needed;
}


//-------------------------------------------------------------------------
//
// method : 		PollObj::get_last_insert_date
// 
// description : 	This method returns the date stored with the most 
//			recent record in the ring buffer (as a double in Sec)
//
//--------------------------------------------------------------------------

double PollObj::get_last_insert_date_i()
{	
	struct timeval last = ring.get_last_insert_date();
	double last_d = (double)last.tv_sec + ((double)last.tv_usec / 1000000);
	return last_d;
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_last_cmd_result
// 
// description : 	This method returns the last data stored in ring
//			for a polled command or throw an exception if the
//			command failed when it was executed
//
//--------------------------------------------------------------------------

CORBA::Any *PollObj::get_last_cmd_result()
{
	omni_mutex_lock(*this);
	
	return ring.get_last_cmd_result();
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_last_attr_value
// 
// description : 	This method returns the last data stored in ring
//			for a polled attribute or throw an exception if the
//			read attribuite operation failed when it was executed
//
//--------------------------------------------------------------------------

Tango::AttributeValue &PollObj::get_last_attr_value(bool lock)
{
	if (lock == true)
		omni_mutex_lock(*this);
	
	return ring.get_last_attr_value();
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::update_upd
// 
// description : 	This method update the polling update period
//
// argument : in :	- new_upd : The new update period (in mS)
//
//--------------------------------------------------------------------------

void PollObj::update_upd(long new_upd)
{
	if (new_upd < 1000)
	{
		upd.tv_usec = new_upd * 1000;
		upd.tv_sec = 0;
	}
	else
	{
		upd.tv_sec = new_upd / 1000;
		upd.tv_usec = (new_upd - (upd.tv_sec * 1000)) * 1000;
	}
	max_delta_t = (double)(new_upd / 1000.0) * dev->get_poll_old_factor();	
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_cmd_history
// 
// description : 	This method get command history from the ring buffer
//
// argument : in :	- n : recodr number
//			- ptr : Pointer to the sequence where command result
//				should be stored
//
//--------------------------------------------------------------------------

void PollObj::get_cmd_history(long n,Tango::DevCmdHistoryList *ptr)
{
	omni_mutex_lock(*this);
	
	ring.get_cmd_history(n,ptr);	
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_attr_history
// 
// description : 	This method get command history from the ring buffer
//
// argument : in :	- n : recodr number
//			- ptr : Pointer to the sequence where command result
//				should be stored
//			- type : The attribute type
//
//--------------------------------------------------------------------------

void PollObj::get_attr_history(long n,Tango::DevAttrHistoryList *ptr,long attr_type)
{
	omni_mutex_lock(*this);
	
	ring.get_attr_history(n,ptr,attr_type);	
}

} // End of Tango namespace
