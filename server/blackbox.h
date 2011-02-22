//=============================================================================
//
// file :               BlackBox.h
//
// description :        Include for the BlackBox object. This class implements
//                      the black box objects which keep tracks of all 
//			operation invoke on a device or attribute retrieved.
//			This black box is managed as a circular buffer
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:15:35  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:05  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:18  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:53  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:23  taurel
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
// Revision 2.2  2002/04/30 10:50:40  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:02  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:11  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:09  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:12  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:44  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:14  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _BLACKBOX_H
#define _BLACKBOX_H

//#include <OB/OCI_IIOP.h>

#include <tango.h>
#ifdef WIN32
#include <winsock.h>
#endif
#include <time.h>

namespace Tango
{

//=============================================================================
//
//			The BlackBoxElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================


enum BlackBoxElt_ReqType
{
	Req_Unknown,
	Req_Operation,
	Req_Attribute
};

enum BlackBoxElt_AttrType
{
	Attr_Unknown,
	Attr_Name,
	Attr_Description,
	Attr_State,
	Attr_Status,
	Attr_AdmName
};

enum BlackBoxElt_OpType
{
	Op_Unknown,
	Op_Command_inout,
	Op_BlackBox,
	Op_Ping,
	Op_Info,
	Op_Command_list,
	Op_Command,
	Op_Get_Attr_Config,
	Op_Set_Attr_Config,
	Op_Read_Attr,
	Op_Write_Attr,
	Op_Command_inout_2,
	Op_Command_list_2,
	Op_Command_2,
	Op_Get_Attr_Config_2,
	Op_Read_Attr_2,
	Op_Command_inout_history_2,
	Op_Read_Attr_history_2
};

class BlackBoxElt
{
public:
	BlackBoxElt();
	~BlackBoxElt();

	BlackBoxElt_ReqType		req_type;
	BlackBoxElt_AttrType		attr_type;
	BlackBoxElt_OpType		op_type;
	string				host;
	string				cmd_name;
	struct timeval			when;
//	OCI::IIOP::InetAddr_slice	*host_ip;	
};

inline bool operator<(const BlackBoxElt &l,const BlackBoxElt &r)
{
	return true;
}

inline bool operator==(const BlackBoxElt &l,const BlackBoxElt &r)
{
	return true;
}

//=============================================================================
//
//			The BlackBox class
//
// description :	Class to implement the black box itself. This is mainly
//			a vector of BlackBoxElt managed as a circular vector
//
//=============================================================================

class BlackBox
{
public:
	BlackBox();
	BlackBox(long);

	void insert_attr(BlackBoxElt_AttrType);
	void insert_cmd(const char *,long vers=1);
	void insert_op(BlackBoxElt_OpType);
	
	void insert_attr(BlackBoxElt_AttrType,const char *);
	void insert_cmd(const char *,const char *,long vers=1);
	void insert_op(BlackBoxElt_OpType,const char *);
	
	Tango::DevVarStringArray *read(long);
	
private:

	void inc_indexes();
	void get_client_host();
	void build_info_as_str(long);
	void date_ux_to_str(struct timeval &,char *);
	
	vector<BlackBoxElt>	box;
	long			insert_elt;
	long			nb_elt;
	long			max_elt;
	
	string			elt_str;
	
	CORBA::ORB_ptr		orb;
};

} // End of Tango namespace

#endif /* _BLACKBOX_ */
