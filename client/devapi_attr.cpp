static const char *RcsId = "$Id$\n$Name$";

//
// devapi_attrib.cpp 	- C++ source code file for TANGO devapi class DeviceAttribute
//
// programmer(s) 	- Andy Gotz (goetz@esrf.fr)
//
// original 		- February 2002
//
// log			- $Log$
// log			- Revision 3.9  2005/05/09 15:27:44  taurel
// log			- - Added DeviceData and DeviceAttribute get_type() method
// log			-
// log			- Revision 3.8  2005/05/04 11:52:18  taurel
// log			- - Changes for 32<-->64 bits data exchange
// log			- - Fix a bug in the DeviceAttribute::has_failed() method (devapi.h file)
// log			-
// log			- Revision 3.7  2005/01/13 08:36:36  taurel
// log			- - Merge trunk with Release_5_0 from brach Release_5_branch
// log			-
// log			- Revision 3.6.2.3  2004/11/26 13:44:06  taurel
// log			- - Fix some bug in method for printing one attribute config object
// log			- - DeviceData and DeviceAttribute default mode is to throw exception when trying to extract something from an empty instance
// log			- - Fix two small memory leaks in filedatabase
// log			-
// log			- Revision 3.6.2.2  2004/10/22 11:23:17  taurel
// log			- Added warning alarm
// log			- Change attribute config. It now includes alarm and event parameters
// log			- Array attribute property now supported
// log			- subscribe_event throws exception for change event if they are not correctly configured
// log			- Change in the polling thread: The event heartbeat has its own work in the work list
// log			- Also add some event_unregister
// log			- Fix order in which classes are destructed
// log			- Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// log			- Change in the CORBA info call for the device type
// log			-
// log			- Revision 3.6.2.1  2004/09/15 06:44:43  taurel
// log			- - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// log			- - It is also possible to read state and status as attributes
// log			- - Fix bug in Database::get_class_property() method (missing ends insertion)
// log			- - Fix bug in admin device DevRestart command (device name case problem)
// log			-
// log			- Revision 3.6  2004/07/07 08:39:56  taurel
// log			-
// log			- - Fisrt commit after merge between Trunk and release 4 branch
// log			- - Add EventData copy ctor, asiignement operator and dtor
// log			- - Add Database and DeviceProxy::get_alias() method
// log			- - Add AttributeProxy ctor from "device_alias/attribute_name"
// log			- - Exception thrown when subscribing two times for exactly yhe same event
// log			-
// log			- Revision 3.5  2003/08/21 07:22:01  taurel
// log			- - End of the implementation of the new way to transfer data for read and
// log			-   write attributes (better use of exception)
// log			- - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// log			- - Added DeviceAttribute ctors from "const char *"
// log			- - Enable writing of spectrum and image attributes
// log			- - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
// log			-   attribute writing
// log			- - Attribute date automatically set in case of attribute quality factor set to INVALID
// log			- - Change in the polling thread discarding element algo. to support case of polling
// log			-   several cmd/atts at the same polling period with cmd/attr having a long response time
// log			- - Take cmd/attr execution time into account in the "Data not updated since" polling
// log			-   status string
// log			- - Split "str().c_str()" code in two lines of code. It was the reason of some problem
// log			-   on Windows device server
// log			- - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
// log			-   to send trigger to the polling thread
// log			-
// log			- Revision 3.4  2003/07/03 07:37:56  taurel
// log			- - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// log			- - Handle this new IDL release in DeviceProxy class
// log			- - New exception methods in DeviceAttribute class
// log			- - New way to get data out of DeviceAttribute object
// log			- - Fix bugs in DeviceProxy copy constructor and assignement operator
// log			- - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// log			- - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// log			- - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// log			- - Add a server_cleanup() method in the Util class
// log			- - Win32 : Update debug menu in the server graphical window to support logging feature
// log			- - Win32 : Display library CVS tag in the "Help->About" sub-window
// log			-
// log			- Revision 3.3  2003/06/30 08:28:41  nleclercq
// log			- Added support for the so-called "Tango Groups"
// log			-
// log			- Revision 3.2.2.2  2003/12/11 11:47:40  taurel
// log			- Added CHANGING attribute quality factor
// log			-
// log			- Revision 3.2.2.1  2003/09/18 14:07:41  taurel
// log			- Fixes some bugs:
// log			-  - Bug fix in DeviceProxy copy constructor and assignement operator
// log			-  - Change the way how DeviceProxy::write_attribute() is coded
// log			-  - Added DeviceAttribute ctors from "const char *"
// log			-  - Split "str().c_str()" in two lines of code. It was the reason of some
// log			-    problems using Windows VC6
// log			-
// log			- Revision 3.2  2003/05/28 14:42:56  taurel
// log			- Add (conditionaly) autoconf generated include file
// log			-
// log			- Revision 3.1  2003/04/03 15:21:52  taurel
// log			- Added methods to print DeviceData, DeviceAttribute, DeviceDataHistory
// log			- and DeviceAttributeHistory instance
// log			-
// log			- Revision 3.0  2003/03/25 16:30:49  taurel
// log			- Change revision number to 3.0 before release 3.0.0 of Tango lib
// log			-
// log			- Revision 2.5  2003/03/20 08:54:54  taurel
// log			- Updated to support asynchronous calls
// log			-
// log			- Revision 2.4  2003/01/09 12:00:33  taurel
// log			- - Ported to gcc 3.2
// log			- - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// log			- - Replace some ORB * by ORB_ptr
// log			- - Use CORBA::ORB::is_nil() instead of comparing to NULL
// log			-
// log			- Revision 2.3  2002/12/16 11:58:36  taurel
// log			- - Change the underlying ORB fom ORBacus to omniORB
// log			- - New method get_device_list() in Util class
// log			- - Util::get_class_list() takes DServer device into account
// log			- - Util::get_device_by_name() takes DSErver device into account
// log			- - Util::get_device_list_by_class() takes DServer device into account
// log			- - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
// log			-
// log			- Revision 2.2  2002/10/14 09:32:43  taurel
// log			- Fix bugs in devapi_base.cpp file :
// log			- - In read_attribute and read_attributes method of the DeviceProxy class
// log			-   Do not create sequence the same way if the call is local or remote.
// log			- - Add reconnection in the Connection::set_timeout_millis method
// log			- - Add flags to the Connection::set_timeout_millis method
// log			- - Fix bug in the DeviceProxy constructor when device is not marked as exported
// log			-   in the database. The constructor was not stateless in this case.
// log			-
// log			- Revision 2.1  2002/08/12 12:43:24  taurel
// log			- Fix bug in DeviceProxy::write_attributes method when writing several
// log			- attributes in one call. (File devapi_base.cpp)
// log			-
// log			- Revision 2.0  2002/06/28 13:43:08  taurel
// log			- Lot of changes since last releases :
// log			- 	- Database object managed as a singleton per control system
// log			- 	- Support all tango device naming syntax (using TANGO_HOST env.
// log			-  	  variable, without env variable and non database device)
// log			- 	- No more copy during read_attribute and command_inout
// log			- 	- Added some missing methods
// log			- 	- Build an exception class hierarchy
// log			- 	- Added correct management of device time-out
// log			- 	- Support all Tango device interface release 2 features
// log			- 	  (data/attribute comming from polling buffer, polling related methods,
// log			- 	   command/attribute result history)
// log			-
// log			- Revision 1.6  2002/04/29 12:11:25  goetz
// log			- New change in Database::delete_device_attribute_property. The fix done in the previous release was not enough
// log			-
// log			- Revision 1.5  2002/04/29 05:43:09  goetz
// log			- Check in by ET. Fix bug (i=i+n_props+1) in Database:delete_device_attribute_property, delete_class_attribute_property and put_class_attribute_property
// log			-
// log			- Revision 1.4  2002/03/18 07:20:56  goetz
// log			- new DeviceProxy() stateless now; added inline method name()
// log			-
// log			- Revision 1.3  2002/03/01 15:48:06  goetz
// log			- added get_attribute_list() method
// log			-
// log			- Revision 1.2  2002/02/28 17:00:52  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.1  2002/02/18 20:42:59  goetz
// log			- supports attributes, added lots of new methods, changed some old ones
// log			-
//
// version 		- $Version$
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
                                                      
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - default constructor to create DeviceAttribute 
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute()
{
	name = "Name not set";
	dim_x = 0;
	dim_y = 0;
	time.tv_sec = 0;
	time.tv_usec = 0;
	time.tv_nsec = 0;
	quality = Tango::ATTR_INVALID;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const DeviceAttribute & source) 
{
	name = source.name;
	exceptions_flags = source.exceptions_flags;
	dim_x = source.dim_x;
	dim_y = source.dim_y;
	quality = source.quality;
	time = source.time;

	DeviceAttribute &nc_source = const_cast<DeviceAttribute &>(source);
	if (nc_source.LongSeq.operator->() != NULL)	
		LongSeq = nc_source.LongSeq._retn();
	if (nc_source.ShortSeq.operator->() != NULL)	
		ShortSeq = nc_source.ShortSeq._retn();	
	if (nc_source.DoubleSeq.operator->() != NULL)	
		DoubleSeq = nc_source.DoubleSeq._retn();
	if (nc_source.StringSeq.operator->() != NULL)	
		StringSeq = nc_source.StringSeq._retn();
	if (nc_source.FloatSeq.operator->() != NULL)	
		FloatSeq = nc_source.FloatSeq._retn();
	if (nc_source.BooleanSeq.operator->() != NULL)	
		BooleanSeq = nc_source.BooleanSeq._retn();	
	if (nc_source.UShortSeq.operator->() != NULL)	
		UShortSeq = nc_source.UShortSeq._retn();
	if (nc_source.UCharSeq.operator->() != NULL)	
		UCharSeq = nc_source.UCharSeq._retn();
		
	d_state = source.d_state;
	d_state_filled = source.d_state_filled;
		
				
	if (source.ext != NULL)
	{
		ext = new DeviceAttributeExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::get_x_dimension - Get attribute data transfer dimension
//
//-----------------------------------------------------------------------------

AttributeDimension DeviceAttribute::get_r_dimension()
{
	AttributeDimension d;
	d.dim_x = dim_x;
	d.dim_y = dim_y;
	
	return d;
}

long DeviceAttribute::get_nb_read()
{
	if (dim_y == 0)
		return dim_x;
	else
		return dim_x * dim_y;
}

AttributeDimension DeviceAttribute::get_w_dimension()
{
	AttributeDimension d;
	d.dim_x = ext->w_dim_x;
	d.dim_y = ext->w_dim_y;
	
	return d;
}

long DeviceAttribute::get_nb_written()
{
	if (ext->w_dim_y == 0)
		return ext->w_dim_x;
	else
		return ext->w_dim_x * ext->w_dim_y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceAttribute & DeviceAttribute::operator=(const DeviceAttribute &rval)
{
	name = rval.name;
	exceptions_flags = rval.exceptions_flags;
	dim_x = rval.dim_x;
	dim_y = rval.dim_y;
	quality = rval.quality;
	time = rval.time;

	DeviceAttribute &nc_rval = const_cast<DeviceAttribute &>(rval);
	if (nc_rval.LongSeq.operator->() != NULL)	
		LongSeq = nc_rval.LongSeq._retn();
	if (nc_rval.ShortSeq.operator->() != NULL)	
		ShortSeq = nc_rval.ShortSeq._retn();	
	if (nc_rval.DoubleSeq.operator->() != NULL)	
		DoubleSeq = nc_rval.DoubleSeq._retn();
	if (nc_rval.StringSeq.operator->() != NULL)	
		StringSeq = nc_rval.StringSeq._retn();
	if (nc_rval.FloatSeq.operator->() != NULL)	
		FloatSeq = nc_rval.FloatSeq._retn();
	if (nc_rval.BooleanSeq.operator->() != NULL)	
		BooleanSeq = nc_rval.BooleanSeq._retn();	
	if (nc_rval.UShortSeq.operator->() != NULL)	
		UShortSeq = nc_rval.UShortSeq._retn();
	if (nc_rval.UCharSeq.operator->() != NULL)	
		UCharSeq = nc_rval.UCharSeq._retn();
		
	d_state = rval.d_state;
	d_state_filled = rval.d_state_filled;
			
	if (ext != NULL)
		delete ext;
	if (rval.ext != NULL)
	{
		ext = new DeviceAttributeExt();
		*ext = *(rval.ext);
	}
	else
		ext = NULL;
					
	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string &new_name, short datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(1);
	ShortSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, short datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(1);
	ShortSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for long
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, long datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	LongSeq  = new(DevVarLongArray);
	LongSeq->length(1);
	LongSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, long datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	LongSeq  = new(DevVarLongArray);
	LongSeq->length(1);
	LongSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for double
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, double datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	DoubleSeq  = new(DevVarDoubleArray);
	DoubleSeq->length(1);
	DoubleSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, double datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	DoubleSeq  = new(DevVarDoubleArray);
	DoubleSeq->length(1);
	DoubleSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for string
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, string& datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum.c_str());
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, string& datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum.c_str());
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for float
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, float datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	FloatSeq  = new(DevVarFloatArray);
	FloatSeq->length(1);
	FloatSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, float datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	FloatSeq  = new(DevVarFloatArray);
	FloatSeq->length(1);
	FloatSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for boolean
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, bool datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	BooleanSeq  = new(DevVarBooleanArray);
	BooleanSeq->length(1);
	BooleanSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, bool datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	BooleanSeq  = new(DevVarBooleanArray);
	BooleanSeq->length(1);
	BooleanSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for unsigned short (DevUShort)
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, unsigned short datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UShortSeq = new(DevVarUShortArray);
	UShortSeq->length(1);
	UShortSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, unsigned short datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UShortSeq = new(DevVarUShortArray);
	UShortSeq->length(1);
	UShortSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for unsigned char (DevUChar)
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, unsigned char datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UCharSeq  = new(DevVarCharArray);
	UCharSeq->length(1);
	UCharSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, unsigned char datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UCharSeq  = new(DevVarCharArray);
	UCharSeq->length(1);
	UCharSeq[0] = datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<short> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<short> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<short> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<short> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of long
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<long> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<long> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<long> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<long> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of double
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<double> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<double> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<double> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<double> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of string
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<string> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<string> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<string> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<string> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of float
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<float> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<float> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<float> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<float> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of bool
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<bool> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	BooleanSeq = new(DevVarBooleanArray);
	BooleanSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<bool> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	BooleanSeq = new(DevVarBooleanArray);
 	BooleanSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<bool> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	BooleanSeq = new(DevVarBooleanArray);
	BooleanSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<bool> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	BooleanSeq = new(DevVarBooleanArray);
	BooleanSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of unsigned short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned short> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned short> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned short> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned short> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of unsigned char
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned char> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned char> &datum)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned char> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned char> &datum,int x,int y)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	d_state_filled = false;
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
	ext = new DeviceAttributeExt();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::~DeviceAttribute() - destructor to destroy DeviceAttribute
//
//-----------------------------------------------------------------------------

DeviceAttribute::~DeviceAttribute()
{
	if (ext != NULL)
		delete ext;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::is_empty() - returns true or false if datum is empty
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::is_empty()
{
	if (LongSeq.operator->() != NULL)
	{	
		if (LongSeq->length() != 0)
			return false;
	}
	
	if (ShortSeq.operator->() != NULL)
	{	
		if (ShortSeq->length() != 0)
			return false;
	}		
		
	if (DoubleSeq.operator->() != NULL)
	{	
		if (DoubleSeq->length() != 0)
			return false;
	}		
		
	if (StringSeq.operator->() != NULL)
	{	
		if (StringSeq->length() != 0)
			return false;
	}
	
	if (FloatSeq.operator->() != NULL)
	{	
		if (FloatSeq->length() != 0)
			return false;
	}
	
	if (BooleanSeq.operator->() != NULL)
	{	
		if (BooleanSeq->length() != 0)
			return false;
	}		
		
	if (UShortSeq.operator->() != NULL)
	{	
		if (UShortSeq->length() != 0)
			return false;
	}		
		
	if (UCharSeq.operator->() != NULL)
	{	
		if (UCharSeq->length() != 0)
			return false;
	}		

	if (d_state_filled == true)
		return false;
		
	if (exceptions_flags.test(isempty_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"cannot extract, no data in DeviceAttribute object ",
					(const char*)"DeviceAttribute::is_empty");
	}		
	return true;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::get_type() - returns attribute data type
//
//-----------------------------------------------------------------------------

int DeviceAttribute::get_type()
{
	int data_type;
	
	if (is_empty() == true)
		return -1;
	else
	{
		if (LongSeq.operator->() != NULL)
			data_type = Tango::DEV_LONG;	
		else if (ShortSeq.operator->() != NULL)
			data_type = Tango::DEV_SHORT;	
		else if (DoubleSeq.operator->() != NULL)
			data_type = Tango::DEV_DOUBLE;	
		else if (FloatSeq.operator->() != NULL)
			data_type = Tango::DEV_FLOAT;
		else if (BooleanSeq.operator->() != NULL)
			data_type = Tango::DEV_BOOLEAN;	
		else if (UShortSeq.operator->() != NULL)
			data_type = Tango::DEV_USHORT;
		else if (UCharSeq.operator->() != NULL)
			data_type = Tango::DEV_UCHAR;	
		else if (StringSeq.operator->() != NULL)
			data_type = Tango::DEV_STRING;
		else
			data_type = Tango::DEV_STATE;	
	}
	
	return data_type;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevState &) - extract a DevState from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevState &datum)
{
	bool ret = true;

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}
		
	if (is_empty() == true)
		return false;
			
	datum = d_state;
	d_state_filled = false;
			
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(short &) - extract a short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (short &datum)
{
	bool ret = true;

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}
		
	if (is_empty() == true)
		return false;
			
	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
			datum = ShortSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a short",
					(const char*)"DeviceAttribute::operator>>");
		}
	}
			
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(short) - insert a short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (short datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        DevVarShortArray *short_vararr = new(DevVarShortArray);
        short_vararr->length(1);
        (*short_vararr)[0] = datum;
        ShortSeq = short_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                               
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(long &) - extract a long from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (long &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}

	bool empty_flag = false;
	if (exceptions_flags.test(isempty_flag))
		empty_flag = true;
		
	exceptions_flags.reset(isempty_flag);

	if (is_empty() == true)
	{
#ifdef TANGO_LONG32
		if (ext->long_from_64 == false)
		{
			if (empty_flag == true)
			{
				exceptions_flags.set(isempty_flag);
				ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"cannot extract, no data in DeviceAttribute object ",
					(const char*)"DeviceAttribute::is_empty");
			}				
			return false;
		}
#else
		if (empty_flag == true)
		{
			exceptions_flags.set(isempty_flag);
			ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
				(const char*)"cannot extract, no data in DeviceAttribute object ",
				(const char*)"DeviceAttribute::is_empty");
		}	
		return false;
#endif
	}
		
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
			datum = LongSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;
		if (exceptions_flags.test(wrongtype_flag))
		{
			TangoSys_OMemStream desc;

			if (ext->long_from_64 == true)
				desc << "Cannot extract, data sent by a 64 bits computer" << ends;
			else
				desc << "Cannot extract, data in DeviceAttribute object is not a long" << ends;
				
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					desc.str(),
					(const char*)"DeviceAttribute::operator>>");
		}
	}
			
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(long) - insert a long into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (long datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarLongArray *long_vararr = new(DevVarLongArray);
        long_vararr->length(1);
        (*long_vararr)[0] = datum;
        LongSeq = long_vararr; 
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(double &) - extract a double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (double &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
		
	if (is_empty() == true)
		return false;
		
	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
			datum = DoubleSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a double",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(double) - insert a double into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (double datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarDoubleArray *double_vararr = new(DevVarDoubleArray);
        double_vararr->length(1);
        (*double_vararr)[0] = datum;
        DoubleSeq = double_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();    	                                              
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(string &) - extract a string from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (string& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
		
	if (is_empty() == true)
		return false;
		
	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
			datum = StringSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a string",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(string &) - insert a string into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (string& datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarStringArray *string_vararr = new(DevVarStringArray);
        string_vararr->length(1);
        (*string_vararr)[0] = string_dup(datum.c_str());
        StringSeq = string_vararr;
	
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                 
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(float &) - extract a float from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (float &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
		
	if (is_empty() == true)
		return false;
		
	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
			datum = FloatSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a float",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(float) - insert a float into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (float datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarFloatArray *float_vararr = new(DevVarFloatArray);
        float_vararr->length(1);
        (*float_vararr)[0] = datum;
        FloatSeq = float_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();    	                                              
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(bool &) - extract a boolean from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (bool &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
		
	if (is_empty() == true)
		return false;
		
	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
			datum = BooleanSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a boolean",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(bool) - insert a boolean into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (bool datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarBooleanArray *bool_vararr = new(DevVarBooleanArray);
        bool_vararr->length(1);
        (*bool_vararr)[0] = datum;
        BooleanSeq = bool_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();    	                                              
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(unsigned short &) - extract a unsigned short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (unsigned short &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
		
	if (is_empty() == true)
		return false;
		
	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
			datum = UShortSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an unsigned short",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(unsigned short) - insert a unsigned short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (unsigned short datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarUShortArray *ush_vararr = new(DevVarUShortArray);
        ush_vararr->length(1);
        (*ush_vararr)[0] = datum;
        UShortSeq = ush_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();    	                                              
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(unsigned char &) - extract a unsigned char from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (unsigned char &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
		
	if (is_empty() == true)
		return false;
		
	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
			datum = UCharSeq[0];
		else
			ret = false;
	}
	else
	{
                ret = false;
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an unsigned char",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(unsigned char) - insert a unsigned char into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (unsigned char datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarCharArray *uch_vararr = new(DevVarCharArray);
        uch_vararr->length(1);
        (*uch_vararr)[0] = datum;
        UCharSeq = uch_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();    	                                              
}



//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<string>) - 
// insert a vector of string into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<string> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;

	if (StringSeq.operator->() == NULL)
	{
		DevVarStringArray *str_vararr = new(DevVarStringArray);
		StringSeq = str_vararr;
	}	
        StringSeq.inout() << datum;
	
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<string> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<string> &) - extract a vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<string>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
                	datum.resize(StringSeq->length());

                	for (unsigned int i=0; i<StringSeq->length(); i++)
                	{
                        	datum[i] = StringSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of string",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<short>) - 
// insert a vector of short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<short> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;

	if (ShortSeq.operator->() == NULL)
	{
		DevVarShortArray *short_vararr = new(DevVarShortArray);
		ShortSeq = short_vararr;
	}		
        ShortSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<short> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<short> &) - extract a vector<short> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<short>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
                	datum.resize(ShortSeq->length());

                	for (unsigned int i=0; i<ShortSeq->length(); i++)
                	{
                        	datum[i] = ShortSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of short",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<long>) - 
// insert a vector of long into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<long> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;

	if (LongSeq.operator->() == NULL)
	{
		DevVarLongArray *long_vararr = new(DevVarLongArray);
		LongSeq = long_vararr;
	}		
        LongSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<long> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<long> &) - extract a vector<long> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<long>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	

	bool empty_flag = false;
	if (exceptions_flags.test(isempty_flag))
		empty_flag = true;
		
	exceptions_flags.reset(isempty_flag);
		
	if (is_empty() == true)
	{
#ifdef TANGO_LONG32
		if (ext->long_from_64 == false)
		{
			if (empty_flag == true)
			{
				exceptions_flags.set(isempty_flag);
				ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"cannot extract, no data in DeviceAttribute object ",
					(const char*)"DeviceAttribute::is_empty");
			}				
			return false;
		}
#else
		if (empty_flag == true)
		{
			exceptions_flags.set(isempty_flag);
			ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
				(const char*)"cannot extract, no data in DeviceAttribute object ",
				(const char*)"DeviceAttribute::is_empty");
		}	
		return false;
#endif
	}
			
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
                	datum.resize(LongSeq->length());

                	for (unsigned int i=0; i<LongSeq->length(); i++)
                	{
                        	datum[i] = LongSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;

		if (exceptions_flags.test(wrongtype_flag))
		{
			TangoSys_OMemStream desc;

			if (ext->long_from_64 == true)
				desc << "Cannot extract, data sent by a 64 bits computer" << ends;
			else
				desc << "Cannot extract, data in DeviceAttribute object is not a long" << ends;
				
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					desc.str(),
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<double>) - 
// insert a vector of double into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<double> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;

	if (DoubleSeq.operator->() == NULL)
	{
		DevVarDoubleArray *double_vararr = new(DevVarDoubleArray);
		DoubleSeq = double_vararr;
	}		
        DoubleSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<double> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<double> &) - extract a vector<double> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<double>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
                	datum.resize(DoubleSeq->length());

                	for (unsigned int i=0; i<DoubleSeq->length(); i++)
                	{
                        	datum[i] = DoubleSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of double",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<float>) - 
// insert a vector of float into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<float> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;

	if (FloatSeq.operator->() == NULL)
	{
		DevVarFloatArray *float_vararr = new(DevVarFloatArray);
		FloatSeq = float_vararr;
	}		
        FloatSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<float> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<float> &) - extract a vector<float> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<float>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
		{
                	datum.resize(FloatSeq->length());

                	for (unsigned int i=0; i<FloatSeq->length(); i++)
                	{
                        	datum[i] = FloatSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of float",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<bool>) - 
// insert a vector of boolean into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<bool> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
	if (BooleanSeq.operator->() == NULL)
	{
		DevVarBooleanArray *bool_vararr = new(DevVarBooleanArray);
		BooleanSeq = bool_vararr;
	}	
        BooleanSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<bool> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<bool> &) - extract a vector<bool> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<bool>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
		{
                	datum.resize(BooleanSeq->length());

                	for (unsigned int i=0; i<BooleanSeq->length(); i++)
                	{
                        	datum[i] = BooleanSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of bool",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<unsigned short>) - 
// insert a vector of unsigned short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<unsigned short> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;

	if (UShortSeq.operator->() == NULL)
	{
		DevVarUShortArray *ushort_vararr = new(DevVarUShortArray);
		UShortSeq = ushort_vararr;
	}		
        UShortSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<unsigned short> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<unsigned short>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
		{
                	datum.resize(UShortSeq->length());

                	for (unsigned int i=0; i<UShortSeq->length(); i++)
                	{
                        	datum[i] = UShortSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of unsigned short",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<unsigned char>) - 
// insert a vector of unsigned char into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<unsigned char> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
	if (UCharSeq.operator->() == NULL)
	{
		DevVarUCharArray *uchar_vararr = new(DevVarUCharArray);
		UCharSeq = uchar_vararr;
	}	
        UCharSeq.inout() << datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();                                                   
}

void DeviceAttribute::insert(vector<unsigned char> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                         
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<unsigned char> &) - extract a vector<unsigne char> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<unsigned char>& datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
		{
                	datum.resize(UCharSeq->length());

                	for (unsigned int i=0; i<UCharSeq->length(); i++)
                	{
                        	datum[i] = UCharSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of unsigned char",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarShortArray *) - extract a sequence of
// short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarShortArray* &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			datum = ShortSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of short",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarLongArray *) - extract a sequence of
// long from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarLongArray* &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	bool empty_flag = false;
	if (exceptions_flags.test(isempty_flag))
		empty_flag = true;
		
	exceptions_flags.reset(isempty_flag);
		
	if (is_empty() == true)
	{
#ifdef TANGO_LONG32
		if (ext->long_from_64 == false)
		{
			if (empty_flag == true)
			{
				exceptions_flags.set(isempty_flag);
				ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"cannot extract, no data in DeviceAttribute object ",
					(const char*)"DeviceAttribute::is_empty");
			}				
			return false;
		}
#else
		if (empty_flag == true)
		{
			exceptions_flags.set(isempty_flag);
			ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
				(const char*)"cannot extract, no data in DeviceAttribute object ",
				(const char*)"DeviceAttribute::is_empty");
		}	
		return false;
#endif
	}
			
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
			datum = LongSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;

		if (exceptions_flags.test(wrongtype_flag))
		{
			TangoSys_OMemStream desc;

			if (ext->long_from_64 == true)
				desc << "Cannot extract, data sent by a 64 bits computer" << ends;
			else
				desc << "Cannot extract, data in DeviceAttribute object is not a long" << ends;
				
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					desc.str(),
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;


}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarDoubleArray *) - extract a sequence of
// double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarDoubleArray* &datum)
{
	bool ret = true;	

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
			datum = DoubleSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of double",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarStringArray *) - extract a sequence of
// string from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarStringArray* &datum)
{
	bool ret = true;
	
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
			datum = StringSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{	
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of string",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarDoubleArray *) - extract a sequence of
// double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarFloatArray* &datum)
{
	bool ret = true;	

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
		{
			datum = FloatSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of float",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarBooleanArray *) - extract a sequence of
// bool from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarBooleanArray* &datum)
{
	bool ret = true;	

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
		{
			datum = BooleanSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of boolean",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarUShortArray *) - extract a sequence of
// unsigned short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarUShortArray* &datum)
{
	bool ret = true;	

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
		{
			datum = UShortSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of unsigned short",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarCharArray *) - extract a sequence of
// unsigned char from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarCharArray* &datum)
{
	bool ret = true;	

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}	
	
	if (is_empty() == true)
		return false;
		
	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
		{
			datum = UCharSeq._retn();
		}			
		else
			ret = false;
	}
	else
	{
                ret = false;
		
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of unsigned char",
					(const char*)"DeviceAttribute::operator>>");
		}
	}		
	return ret;
}



//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarShortArray &) - 
// insert a DevVarShortArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarShortArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        ShortSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		ShortSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarShortArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarShortArray *) - 
// insert a DevVarShortArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarShortArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        ShortSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarShortArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarLongArray &) - 
// insert a DevVarLongArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarLongArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        LongSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		LongSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarLongArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarLongArray *) - 
// insert a DevVarLongArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarLongArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        LongSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarLongArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarDoubleArray &) - 
// insert a DevVarDoubleArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarDoubleArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        DoubleSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		DoubleSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarDoubleArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarDoubleArray *) - 
// insert a DevVarDoubleArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarDoubleArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        DoubleSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarDoubleArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarStringArray &) - 
// insert a DevVarStringArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarStringArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        StringSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		StringSeq[i] = datum[i];
	
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarStringArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarStringArray *) - 
// insert a DevVarStringArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarStringArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        StringSeq = datum;
	
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarStringArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarFloatArray &) - 
// insert a DevVarFloatArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarFloatArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        FloatSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		FloatSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarFloatArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarFloatArray *) - 
// insert a DevVarFloatArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarFloatArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        FloatSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarFloatArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarBooleanArray &) - 
// insert a DevVarBooleanArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarBooleanArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        BooleanSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		BooleanSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarBooleanArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarBooleanArray *) - 
// insert a DevVarBooleanArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarBooleanArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        BooleanSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarBooleanArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarUShortArray &) - 
// insert a DevVarUShortArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarUShortArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        UShortSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		UShortSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarUShortArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarUShortArray *) - 
// insert a DevVarUShortArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarUShortArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        UShortSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (UCharSeq.operator->() != NULL)
		delete UCharSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarUShortArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarCharArray &) - 
// insert a DevVarCharArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarCharArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        UCharSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		UCharSeq[i] = datum[i];
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();                                                   
}

void DeviceAttribute::insert(const DevVarCharArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarCharArray *) - 
// insert a DevVarCharArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarCharArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        UCharSeq = datum;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (FloatSeq.operator->() != NULL)
		delete FloatSeq._retn();
	if (BooleanSeq.operator->() != NULL)
		delete BooleanSeq._retn();
	if (UShortSeq.operator->() != NULL)
		delete UShortSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();                                                   
}

void DeviceAttribute::insert(DevVarCharArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;                                           
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			DeviceAttribute class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DeviceAttribute &da)
{

//
// Print date
//

	if (da.time.tv_sec != 0)
	{
		char *tmp_date = asctime(localtime((time_t *)&da.time.tv_sec));
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << da.time.tv_sec << "," << da.time.tv_usec << " sec) : ";
	}

//
// print attribute name
//

	o_str << da.name;
	
//
// print dim_x and dim_y
//

	o_str << " (dim_x = " << da.dim_x << ", dim_y = " << da.dim_y << ", ";
	o_str << "w_dim_x = " << da.ext->w_dim_x << ", w_dim_y = " << da.ext->w_dim_y << ", ";
	
//
// Print quality
//

	o_str << "Data quality factor = ";
	switch (da.quality)
	{
	case Tango::ATTR_VALID:
		o_str << "VALID)" << endl;
		break;
		
	case Tango::ATTR_INVALID:
		o_str << "INVALID)";
		break;
		
	case Tango::ATTR_ALARM:
		o_str << "ALARM)" << endl;
		break;
		
	case Tango::ATTR_CHANGING:
		o_str << "CHANGING)" << endl;
		break;

	case Tango::ATTR_WARNING:
		o_str << "WARNING)" << endl;
		break;
	}
	
//
// Print data (if valid)
//

	if (da.quality != Tango::ATTR_INVALID)
	{
		if (da.is_empty() == true)
			o_str << "No data in DeviceAttribute object";
		else
		{
			if (da.LongSeq.operator->() != NULL)
				o_str << *(da.LongSeq.operator->());	
			else if (da.ShortSeq.operator->() != NULL)
				o_str << *(da.ShortSeq.operator->());	
			else if (da.DoubleSeq.operator->() != NULL)
				o_str << *(da.DoubleSeq.operator->());	
			else if (da.FloatSeq.operator->() != NULL)
				o_str << *(da.FloatSeq.operator->());
			else if (da.BooleanSeq.operator->() != NULL)
				o_str << *(da.BooleanSeq.operator->());	
			else if (da.UShortSeq.operator->() != NULL)
				o_str << *(da.UShortSeq.operator->());
			else if (da.UCharSeq.operator->() != NULL)
				o_str << *(da.UCharSeq.operator->());	
			else if (da.StringSeq.operator->() != NULL)
				o_str << *(da.StringSeq.operator->());
			else
				o_str << DevStateName[da.d_state];	
		}
	}
	
	return o_str;
}


} // End of Tango namespace
