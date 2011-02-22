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
// Revision 3.4.2.2  2004/10/22 11:25:00  taurel
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
// Revision 3.4.2.1  2004/08/19 07:44:03  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.4  2003/08/21 07:23:46  taurel
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
// Revision 3.3  2003/07/03 07:40:51  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.2  2003/05/16 08:46:15  taurel
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
//
// Revision 3.1  2003/04/08 15:06:16  taurel
// - Names of read/written attributes are now stored in blackbox
// - Admin device RestartServer command is now done in its own thread with
//   changing CORBA POA manager state
//
// Revision 3.0  2003/03/25 16:41:49  taurel
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
// Revision 2.8  2002/12/16 12:06:21  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
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

#include <tango.h>
#ifdef WIN32
#include <winsock.h>
#endif
#include <time.h>
#include <omniORB4/omniInterceptors.h>

namespace Tango
{

CORBA::Boolean get_client_addr(omni::omniInterceptors::serverReceiveRequest_T::info_T &);

class client_addr: public omni_thread::value_t
{
public:
	client_addr(const char *addr) {strcpy(client_ip,addr);}
	~client_addr() {};

	char	client_ip[80];
};

//=============================================================================
//
//			The BlackBoxElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================

#define 	DEFAULT_ATTR_NB		10

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
	Op_Read_Attr_history_2,
	Op_Read_Attr_3,
	Op_Write_Attr_3,
	Op_Read_Attr_history_3,
	Op_Info_3,
	Op_Get_Attr_Config_3,
	Op_Set_Attr_Config_3
};

class BlackBoxElt
{
public:
	BlackBoxElt();
	~BlackBoxElt();

	BlackBoxElt_ReqType		req_type;
	BlackBoxElt_AttrType		attr_type;
	BlackBoxElt_OpType		op_type;
	string				cmd_name;
	vector<string>			attr_names;
	struct timeval			when;
	char				host_ip_str[80];
	DevSource			source;	
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

	void insert_corba_attr(BlackBoxElt_AttrType);
	void insert_cmd(const char *,long vers=1,DevSource=Tango::DEV);
	void insert_attr(const Tango::DevVarStringArray &,long vers=1,DevSource=Tango::DEV);
	void insert_attr(const Tango::AttributeValueList &,long vers=1);
	void insert_op(BlackBoxElt_OpType);
		
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
	
	omni_mutex		sync;
	
	string			elt_str;
};

} // End of Tango namespace

#endif /* _BLACKBOX_ */
