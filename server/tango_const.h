//=============================================================================
//
// file :               Tango_const.h
//
// description :        Include for Tango system constant definition  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.17  2006/05/23 12:02:06  jensmeyer
// Added a reconnection delay of 1000ms not to send to many unnecessary
// requestes to the database. Modified Connection::reconnect().
//
// Revision 3.16  2006/05/18 14:29:12  jensmeyer
// Changed the handling of the event period for periodic and archiving events.
// The event period is now stored as the configured value and no longer
// multilplied by the factor DELTA_PERIODIC.
// The calculation of the precision to fire periodic events has moved from
// the attribbute to the push methods in the event supplier.
//
// For event periods < 5000 ms a precision of 2% was kept, but for
// times < 5000 ms a fixed precision of 100 ms was added.
// This avoids periodic events with a long event period to be fired to early.
//
// Revision 3.15  2006/05/18 08:51:56  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.14  2005/07/28 07:34:41  taurel
// - Fix some incompatibility between 5.1 and 5.2
//
// Revision 3.13  2005/07/12 08:51:27  taurel
// - Just update TgLibVers define
//
// Revision 3.12  2005/06/29 08:31:19  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.11  2005/03/14 09:49:29  taurel
// - Fix some bugs in filedatabase (Change in the data transferred between client and
//   server).
// - Fix bug in event re-connection
// - Add event support even for device server started with the -file command line option
//
// Revision 3.10  2005/01/13 09:39:21  taurel
// - Modified to Tango release 5.1.0
//
// Revision 3.9  2005/01/13 08:30:36  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.8.2.6  2004/11/26 13:53:21  taurel
// - Fix bug if exception thrown during Util class construction
// - Change attribute label and format default values
//
// Revision 3.8.2.5  2004/11/22 14:14:46  taurel
// - Added methods for the AttrDesc class for the new attribute config
//   parameters.
// - Some minor changes
//
// Revision 3.8.2.4  2004/11/04 09:46:39  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.8.2.3  2004/10/22 11:26:33  taurel
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
// Revision 3.8.2.2  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.8.2.1  2004/09/15 06:47:17  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.8  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.7  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.6  2003/08/22 12:52:55  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.5  2003/08/21 07:24:37  taurel
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
// Revision 3.4  2003/07/03 07:40:51  taurel
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
// Revision 3.3.2.9  2004/06/01 14:57:58  taurel
// - Now it's release 4.3
//
// Revision 3.3.2.8  2004/05/18 10:56:39  taurel
// - Now, it's release 4.2.0 (TgLibVers)
//
// Revision 3.3.2.7  2004/04/02 14:58:18  taurel
// Changes for release 4.1
// - Change the event.h inclusion place in tango.h
// - Fix bugs in event.cpp file and add a clean way to shutdown event system
// - Now support attribute min,max,min_alarm and max_alarm defined in scientific notation for long attribute
// - Added debian30 support in Make.rules
//
// Revision 3.3.2.6  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.3.2.5  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.3.2.4  2003/11/28 17:08:44  taurel
// Changes to handle tango lib build as a Windows DLL and used in a device
// server
//
// Revision 3.3.2.2  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.3.2.1  2003/09/18 14:11:06  taurel
// - Add client/group.h and client/group.cpp in Makefile
// - Update Tango release number in Makefiles and in TgLibVers string
//
// Revision 3.3  2003/06/24 07:31:02  taurel
// CHange release number to 3.0.2
//
// Revision 3.2  2003/05/16 08:46:16  taurel
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
// Revision 3.1  2003/04/02 12:26:04  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
// Revision 3.0.2.1  2003/11/21 11:30:21  andy_gotz
// changed string references in subscribe_event() calls to const's. Added
// filter parameter but not interpreting it yet. Made event type an
// enum. Fixed problem with resubscription. Minor memory leak in clients
// still persists.
//
// Revision 3.0  2003/03/25 16:47:52  taurel
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
//=============================================================================

#ifndef _TANGO_CONST_H
#define _TANGO_CONST_H

namespace Tango
{

//
// Some general interest define
//

#define		TgLibVers			"5.5.0"

#define		DevVersion			3
#define		DefaultMaxSeq			20
#define		DefaultBlackBoxDepth		25
#define 	DefaultPollRingDepth		10

#define		InitialOutput			"Initial Output"
#define		DSDeviceDomain  		"dserver"
#define		DefaultDocUrl			"http://www.esrf.fr/tango"
#define		EnvVariable			"TANGO_HOST"
#define		DbObjName			"database"
#define		DescNotSet			"Uninitialised"
#define		ResNotDefined			"0"
#define         MessBoxTitle			"Tango Device Server"
#define		StatusNotSet			"Not initialised"

#define		DefaultWritAttrProp		false
#define		AllAttr 			"All attributes"
#define		AllAttr_3 			"All attributes_3"

#define		PollCommand			"command"
#define		PollAttribute			"attribute"

#define		MIN_POLL_PERIOD			20
#define		DELTA_T				1002000000
#define		MIN_DELTA_WORK			20000
#define		TIME_HEARTBEAT			2000
#define		POLL_LOOP_NB			500
#define		ONE_SECOND			1000000
#define		DISCARD_THRESHOLD		0.02

#define		DEFAULT_TIMEOUT_SEC		4
#define		DEFAULT_TIMEOUT			DEFAULT_TIMEOUT_SEC * 1000
#define		DEFAULT_POLL_OLD_FACTOR		4

#define		TG_IMP_MINOR_TO			10
#define		TG_IMP_MINOR_DEVFAILED		11
#define		TG_IMP_MINOR_NON_DEVFAILED	12

#define		TANGO_PY_MOD_NAME		"_PyTango.pyd"

//
// Event related define
//

#define 	EVENT_HEARTBEAT_PERIOD 		10
#define 	EVENT_RESUBSCRIBE_PERIOD 	600
#define		DEFAULT_EVENT_PERIOD		1000
#define		DELTA_PERIODIC				0.98  // Using a delta of 2% only for times < 5000 ms
#define     DELTA_PERIODIC_LONG		100   // For times > 5000ms only keep a delta of 100ms
#define		HEARTBEAT			"Event heartbeat"

//
// Client timeout as defined by omniORB4.0.0
//

#define		CLNT_TIMEOUT_STR		"3000"
#define		CLNT_TIMEOUT			3000

//
// Time to wait before trying to reconnect after
// a connevtion failure
//
#define 		RECONNECTION_DELAY		1000   //ms. Only try to reconnect every second

//
// Max transfer size 16 MBytes (in byte). Needed by omniORB
//

#define		MAX_TRANSFER_SIZE		"16777216"

//
// Tango name length
//

#define		MaxServerNameLength		255

//
// A short inline function to hide the CORBA::string_dup function
//

inline char * string_dup(char *s) {return CORBA::string_dup(s);}
inline char * string_dup(const char *s) {return CORBA::string_dup(s);}

//
// Many, many typedef
//

typedef	const char *	ConstDevString;	// Pseudo Tango type to ease POGO job
typedef DevVarCharArray	DevVarUCharArray;

class DeviceImpl;

typedef bool (DeviceImpl::*StateMethPtr)(const CORBA::Any &);

typedef void (DeviceImpl::*CmdMethPtr)();

typedef void (DeviceImpl::*CmdMethPtr_Bo)(DevBoolean);
typedef void (DeviceImpl::*CmdMethPtr_Sh)(DevShort);
typedef void (DeviceImpl::*CmdMethPtr_Lg)(DevLong);
typedef void (DeviceImpl::*CmdMethPtr_Fl)(DevFloat);
typedef void (DeviceImpl::*CmdMethPtr_Db)(DevDouble);
typedef void (DeviceImpl::*CmdMethPtr_US)(DevUShort);
typedef void (DeviceImpl::*CmdMethPtr_UL)(DevULong);
typedef void (DeviceImpl::*CmdMethPtr_Str)(DevString);
typedef void (DeviceImpl::*CmdMethPtr_ChA)(const DevVarCharArray *);
typedef void (DeviceImpl::*CmdMethPtr_ShA)(const DevVarShortArray *);
typedef void (DeviceImpl::*CmdMethPtr_LgA)(const DevVarLongArray *);
typedef void (DeviceImpl::*CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef void (DeviceImpl::*CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef void (DeviceImpl::*CmdMethPtr_USA)(const DevVarUShortArray *);
typedef void (DeviceImpl::*CmdMethPtr_ULA)(const DevVarULongArray *);
typedef void (DeviceImpl::*CmdMethPtr_StrA)(const DevVarStringArray *);
typedef void (DeviceImpl::*CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef void (DeviceImpl::*CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef void (DeviceImpl::*CmdMethPtr_Sta)(DevState);

typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr)();
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr)();
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr)();
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr)();
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr)();
typedef DevUShort (DeviceImpl::*US_CmdMethPtr)();
typedef DevULong (DeviceImpl::*UL_CmdMethPtr)();
typedef DevString (DeviceImpl::*Str_CmdMethPtr)();
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr)();
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr)();
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr)();
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr)();
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr)();
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr)();
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr)();
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr)();
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr)();
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr)();
typedef DevState (DeviceImpl::*Sta_CmdMethPtr)();

typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Bo)(DevBoolean);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Sh)(DevShort);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Lg)(DevLong);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Fl)(DevFloat);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Db)(DevDouble);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_US)(DevUShort);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_UL)(DevULong);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Str)(DevString);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Sta)(DevState);

typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Bo)(DevBoolean);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Sh)(DevShort);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Lg)(DevLong);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Fl)(DevFloat);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Db)(DevDouble);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_US)(DevUShort);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_UL)(DevULong);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Str)(DevString);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Sta)(DevState);

typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Bo)(DevBoolean);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Sh)(DevShort);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Lg)(DevLong);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Fl)(DevFloat);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Db)(DevDouble);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_US)(DevUShort);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_UL)(DevULong);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Str)(DevString);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Sta)(DevState);

typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Bo)(DevBoolean);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Sh)(DevShort);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Lg)(DevLong);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Fl)(DevFloat);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Db)(DevDouble);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_US)(DevUShort);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_UL)(DevULong);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Str)(DevString);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Sta)(DevState);

typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Bo)(DevBoolean);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Sh)(DevShort);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Lg)(DevLong);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Fl)(DevFloat);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Db)(DevDouble);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_US)(DevUShort);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_UL)(DevULong);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Str)(DevString);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Sta)(DevState);

typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Bo)(DevBoolean);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Sh)(DevShort);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Lg)(DevLong);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Fl)(DevFloat);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Db)(DevDouble);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_US)(DevUShort);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_UL)(DevULong);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Str)(DevString);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Sta)(DevState);

typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Bo)(DevBoolean);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Sh)(DevShort);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Lg)(DevLong);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Fl)(DevFloat);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Db)(DevDouble);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_US)(DevUShort);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_UL)(DevULong);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Str)(DevString);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Sta)(DevState);

typedef DevString (DeviceImpl::*Str_CmdMethPtr_Bo)(DevBoolean);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Sh)(DevShort);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Lg)(DevLong);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Fl)(DevFloat);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Db)(DevDouble);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_US)(DevUShort);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_UL)(DevULong);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Str)(DevString);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Sta)(DevState);

typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Sh)(DevShort);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Lg)(DevLong);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Db)(DevDouble);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_US)(DevUShort);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_UL)(DevULong);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Str)(DevString);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Sta)(DevState);

typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Sh)(DevShort);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Lg)(DevLong);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Db)(DevDouble);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_US)(DevUShort);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_UL)(DevULong);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Str)(DevString);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Sta)(DevState);

typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Sh)(DevShort);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Lg)(DevLong);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Db)(DevDouble);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_US)(DevUShort);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_UL)(DevULong);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Str)(DevString);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Sta)(DevState);

typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Sh)(DevShort);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Lg)(DevLong);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Db)(DevDouble);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_US)(DevUShort);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_UL)(DevULong);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Str)(DevString);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Sta)(DevState);

typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Sh)(DevShort);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Lg)(DevLong);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Db)(DevDouble);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_US)(DevUShort);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_UL)(DevULong);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Str)(DevString);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Sta)(DevState);

typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Sh)(DevShort);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Lg)(DevLong);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Db)(DevDouble);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_US)(DevUShort);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_UL)(DevULong);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Str)(DevString);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Sta)(DevState);

typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Sh)(DevShort);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Lg)(DevLong);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Db)(DevDouble);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_US)(DevUShort);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_UL)(DevULong);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Str)(DevString);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Sta)(DevState);

typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Sh)(DevShort);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Lg)(DevLong);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Db)(DevDouble);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_US)(DevUShort);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_UL)(DevULong);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Str)(DevString);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Sta)(DevState);

typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Sh)(DevShort);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Lg)(DevLong);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Db)(DevDouble);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_US)(DevUShort);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_UL)(DevULong);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Str)(DevString);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Sta)(DevState);

typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Sh)(DevShort);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Lg)(DevLong);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Db)(DevDouble);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_US)(DevUShort);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_UL)(DevULong);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Str)(DevString);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Sta)(DevState);

typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Bo)(DevBoolean);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Sh)(DevShort);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Lg)(DevLong);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Fl)(DevFloat);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Db)(DevDouble);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_US)(DevUShort);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_UL)(DevULong);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Str)(DevString);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Sta)(DevState);


//
// Some enum
//

enum CmdArgType {
	DEV_VOID = 0,
	DEV_BOOLEAN,
	DEV_SHORT,
	DEV_LONG,
	DEV_FLOAT,
	DEV_DOUBLE,
	DEV_USHORT,
	DEV_ULONG,
	DEV_STRING,
	DEVVAR_CHARARRAY,
	DEVVAR_SHORTARRAY,
	DEVVAR_LONGARRAY,
	DEVVAR_FLOATARRAY,
	DEVVAR_DOUBLEARRAY,
	DEVVAR_USHORTARRAY,
	DEVVAR_ULONGARRAY,
	DEVVAR_STRINGARRAY,
	DEVVAR_LONGSTRINGARRAY,
	DEVVAR_DOUBLESTRINGARRAY,
	DEV_STATE,
	CONST_DEV_STRING,
	DEVVAR_BOOLEANARRAY,
	DEV_UCHAR
};

enum MessBoxType {
        STOP = 0,
        INFO
};

enum PollObjType {
	POLL_CMD = 0,
	POLL_ATTR,
	EVENT_HEARTBEAT
};

enum PollCmdCode {
	POLL_ADD_OBJ = 0,
	POLL_REM_OBJ,
	POLL_START,
	POLL_STOP,
	POLL_UPD_PERIOD,
	POLL_REM_DEV,
	POLL_EXIT,
	POLL_REM_EXT_TRIG_OBJ,
	POLL_ADD_HEARTBEAT,
	POLL_REM_HEARTBEAT
};

enum SerialModel {
	BY_DEVICE = 0,
	BY_CLASS,
	BY_PROCESS,
	NO_SYNC
};

enum AttReqType {
	READ_REQ = 0,
	WRITE_REQ
};

//
// The polled device structure
//

typedef struct _PollDevice
{
	string		dev_name;
	vector<long>	ind_list;
}PollDevice;

#ifdef TANGO_HAS_LOG4TANGO
//
// Logging levels
//

enum LogLevel {
  	LOG_OFF = 0,
  	LOG_FATAL,
  	LOG_ERROR,
  	LOG_WARN,
  	LOG_INFO,
  	LOG_DEBUG
} ;

//
// Logging targets
//

enum LogTarget {
  	LOG_CONSOLE = 0,
  	LOG_FILE,
  	LOG_DEVICE
};

//
// Logging targets (as string)
//

#define kLogTargetConsole "console"
#define kLogTargetFile	  "file"
#define kLogTargetDevice  "device"

//
// Logging target [type/name] separator
//

#define kLogTargetSep "::"

//
// TANGO <rolling log files> threshold 
//

// Min RollingFileAppender threshold (~500kB)
const size_t kMinRollingThreshold = 500;
// Default RollingFileAppender threshold (~2MB)
const size_t kDefaultRollingThreshold = 2 * 1024;
// Max RollingFileAppender threshold (~20MB)
const size_t kMaxRollingThreshold = 20 * 1024;

#endif // TANGO_HAS_LOG4TANGO

//
// The command argument name
//

const char * const CmdArgTypeName[] = {
	"DevVoid",
	"DevBoolean",
	"DevShort",
	"DevLong",
	"DevFloat",
	"DevDouble",
	"DevUShort",
	"DevULong",
	"DevString",
	"DevVarCharArray",
	"DevVarShortArray",
	"DevVarLongArray",
	"DevVarFloatArray",
	"DevVarDoubleArray",
	"DevVarUShortArray",
	"DevVarULongArray",
	"DevVarStringArray",
	"DevVarLongStringArray",
	"DevVarDoubleStringArray",
	"DevState",
	"ConstDevString",
	"DevVarBooleanArray",
	"DevUChar"
};

//
// The state name
//

const char * const DevStateName[] = {
	"ON",
	"OFF",
	"CLOSE",
	"OPEN",
	"INSERT",
	"EXTRACT",
	"MOVING",
	"STANDBY",
	"FAULT",
	"INIT",
	"RUNNING",
	"ALARM",
	"DISABLE",
	"UNKNOWN"
};

enum EventType {
	CHANGE_EVENT=0,
	QUALITY_EVENT,
	PERIODIC_EVENT,
	ARCHIVE_EVENT,
	USER_EVENT
};

enum KeepAliveCmdCode {
	EXIT_TH = 0
};


//
// The optional attribute properties
//

#define		AlrmValueNotSpec	"Not specified"
#define		AssocWritNotSpec	"None"
#define		LabelNotSpec	  	"No label"
#define		DescNotSpec		"No description"
#define		UnitNotSpec		"No unit"
#define		StdUnitNotSpec		"No standard unit"
#define		DispUnitNotSpec		"No display unit"
#define		FormatNotSpec		"%6.2f"

#define		NotANumber		"NaN"

#define		MemNotUsed		"Not used yet"
#define		MemAttrPropName		"__value"

typedef struct _OptAttrProp
{
	const char *name;
	const char *default_value;
}OptAttrProp;


/*
 * List of strings used by the API as the DevError reason field.
 * This list is given here only for API writers to re-use (if possible)
 * strings already used.
 *
 *
 * API_MemoryAllocation		 
 * API_CommandNotFound 		 
 * API_IncompatibleCmdArgumentType	 
 * API_BlackBoxArgument		 
 * API_BlackBoxEmpty			 
 * API_CannotOpenFile  		 
 * API_SignalOutOfRange		 
 * API_CantInstallSignal		 
 * API_CommandNotAllowed		 
 * API_CantRetrieveClassList		 
 * API_ClassNotFound			 
 * API_InitMethodNotFound		 
 * API_JavaRuntimeSecurityException	 
 * API_InitThrowsException		 
 * API_InitNotPublic			 
 * API_DeviceNotFound  		 
 * API_CmdArgumentTypeNotSupported	 
 * API_MethodNotFound  		 
 * API_OverloadingNotSupported 	 
 * API_MethodArgument  		 
 * API_AttrWrongDefined		 
 * API_AttrOptProp			 
 * API_AttrNotFound			 
 * API_AttrValueNotSet 		 
 * API_AttrNoAlarm			 
 * API_AttrNotWritable 		 
 * API_IncompatibleAttrDataType	 
 * API_WAttrOutsideLimit		 
 * API_NtDebugWindowError		 
 * API_CantCreateClassPoa		 
 * API_CantGetDevObjectId		 
 * API_CantActivatePOAManager
 * API_DatabaseAccess
 * API_CorbaSysException 
 * API_BadConfigurationProperty 
 * API_AttrIncorrectDataNumber
 * API_NotSupportedFeature
 * API_AttrNotAllowed
 * API_AttrWrongMemValue
 * API_WizardConfError
 * API_EventSupplierNotConstructed
 
 */

} // End of Tango namespace

		
#endif /* TANGO_CONST_H */
