//=============================================================================
//
// file :               PollRing.h
//
// description :        Include for the PollRing object. This class implements
//                      the polling ring buffer. Command result or attribute
//			values are stored in this buffer manages as a ring
//			buffer.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
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
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _POLLRING_H
#define _POLLRING_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The RingElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================


class RingElt
{
public:
	RingElt();

	CORBA::Any			*cmd_result;
	Tango::AttributeValueList	*attr_value;
	Tango::DevFailed		*except;
	struct timeval			when;
};

inline bool operator<(const RingElt &l,const RingElt &r)
{
	return true;
}

inline bool operator==(const RingElt &l,const RingElt &r)
{
	return true;
}

//=============================================================================
//
//			The PollRing class
//
// description :	Class to implement the ring buffer itself. This is mainly
//			a vector of RingElt managed as a circular buffer
//
//=============================================================================

class PollRing
{
public:
	PollRing();
	PollRing(long);
	~PollRing();

	void insert_data(CORBA::Any *,struct timeval &);
	void insert_data(Tango::AttributeValueList *,struct timeval &);
	void insert_except(Tango::DevFailed *,struct timeval &);
	
	void get_delta_t(vector<double> &,long nb);
	struct timeval get_last_insert_date();
	bool is_empty() {if (nb_elt == 0) return true;else return false;}
	bool is_last_an_error();
	Tango::DevFailed *get_last_except();
	CORBA::Any *get_last_cmd_result();
	Tango::AttributeValue &get_last_attr_value();
	long get_nb_elt() {return nb_elt;}
	
	void get_cmd_history(long,Tango::DevCmdHistoryList *);
	void get_attr_history(long,Tango::DevAttrHistoryList *,long);
	
private:
	void inc_indexes();
	
	vector<RingElt>		ring;
	long			insert_elt;
	long			nb_elt;
	long			max_elt;
	bool			cmd_init;
	bool			cmd_need_copy;	
};

} // End of Tango namespace

#endif /* _POLLRING_ */
